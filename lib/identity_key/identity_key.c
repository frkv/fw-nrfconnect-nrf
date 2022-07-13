/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <string.h>
#include <identity_key.h>
#include <nrf_cc3xx_platform_kmu.h>
#include <nrf_cc3xx_platform_ctr_drbg.h>
#include <nrf_cc3xx_platform_identity_key.h>
#include <hw_unique_key.h>
#include "hw_unique_key_internal.h"
#include <zephyr/sys/util.h>

/* Reuse HUK printing and panic macros */
#define IDENTITY_KEY_PRINT_VAL 		HUK_PRINT_VAL
#define IDENTITY_KEY_PRINT 		HUK_PRINT
#define IDENTITY_KEY_PANIC 		HUK_PANIC

#define ERR_IDENTITY_KEY_MISSING 	0x15501
#define ERR_IDENTITY_KEY_FAILED		0x15502


// Use 'copy_of_uicr_word_read' instead of 'nrfx_nvmc_uicr_word_read'
// until it has become available everywhere. Everywhere consists of
// TF-M's copy of nrfx, nrfx, hal_nordic in Zephyr and NCS.

static uint32_t copy_of_uicr_word_read(uint32_t const volatile *address)
{
	uint32_t value = *address;

#if NRF91_ERRATA_7_ENABLE_WORKAROUND
	__DSB();
#endif

	return value;
}


bool identity_key_mkek_is_written(void)
{
	return hw_unique_key_is_written(HUK_KEYSLOT_MKEK);
}

bool identity_key_is_written(void)
{
	NRF_KMU->SELECTKEYSLOT = KMU_SELECT_SLOT(NRF_KMU_SLOT_KIDENT);

	// Qualify that the permission corresponds to a key that can be
	// read from the CPU (and is not pushable)
	if (copy_of_uicr_word_read(&NRF_UICR_S->KEYSLOT.CONFIG[NRF_KMU_SLOT_KIDENT].PERM) !=
		NRF_CC3XX_PLATFORM_KMU_IDENTITY_KEY_PERMISSIONS) {
		NRF_KMU->SELECTKEYSLOT = 0;
		return false;
	}

	NRF_KMU->SELECTKEYSLOT = 0;
	return true;
}

int identity_key_read(uint8_t key[IDENTITY_KEY_SIZE_BYTES])
{
	int err;

	/* MKEK is required to retrieve key */
	if (!identity_key_mkek_is_written()) {
		return -ERR_IDENTITY_KEY_MKEK_MISSING;
	}

	if (!identity_key_is_written()) {
		return -ERR_IDENTITY_KEY_MISSING;
	}

	/* Retrieve the identity key */
	err = nrf_cc3xx_platform_identity_key_retrieve(NRF_KMU_SLOT_KIDENT, key);
	if (err != NRF_CC3XX_PLATFORM_SUCCESS) {
		return -ERR_IDENTITY_KEY_FAILED;
	}

	return err;
}

#if defined(CONFIG_IDENTITY_KEY_RANDOM)
void identity_key_write(uint8_t key[IDENTITY_KEY_SIZE_BYTES])
{
	int err = nrf_cc3xx_platform_identity_key_store(NRF_KMU_SLOT_KIDENT, key);
	if (err != NRF_CC3XX_PLATFORM_SUCCESS)
	{
		IDENTITY_KEY_PRINT_VAL("The identity key writing to: ", NRF_KMU_SLOT_KIDENT);
		IDENTITY_KEY_PRINT_VAL("  failed with error code: ", err);
		IDENTITY_KEY_PANIC();
	}
}
#endif /* CONFIG_IDENTITY_KEY_RANDOM */


