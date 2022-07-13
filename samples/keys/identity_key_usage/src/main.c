/*
 * Copyright (c) 2022 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include <stdio.h>
#include <hw_unique_key.h>
#include <psa/crypto.h>
#include <nrf_cc3xx_platform.h>
#include <nrf_cc3xx_platform_ctr_drbg.h>
#include <nrf_cc3xx_platform_defines.h>
#include <identity_key.h>

#define IDENTITY_KEY_PUBLIC_KEY_SIZE (65)
uint8_t m_pub_key[IDENTITY_KEY_PUBLIC_KEY_SIZE];

void hex_dump(uint8_t *buff, uint32_t len)
{
	for (int i = 0; i < len; i++) {
		printk("%s%02x", i && ((i % 32) == 0) ? "\n" : "", buff[i]);
	}
	printk("\n");
}

void main(void)
{
	int err;
	psa_status_t status;
	psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
	psa_key_id_t key_id_out = 0;
	uint8_t key[IDENTITY_KEY_SIZE_BYTES];
	size_t olen;

	err = nrf_cc3xx_platform_init();
	if (err != NRF_CC3XX_PLATFORM_SUCCESS)
	{
		printk("nrf_cc3xx_platform_init returned error: %d\n\r", err);
		return;
	}

	printk("Initializing PSA crypto.\n\r");

	status = psa_crypto_init();
	if (status != PSA_SUCCESS) {
		printk("psa_crypto_init returned error: %d\n\r", status);
		return;
	}

	/* Configure the key attributes for Curve type secp256r1*/
	psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH);
	psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_VOLATILE);
	psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
	psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
	psa_set_key_bits(&key_attributes, 256);


	if (!identity_key_is_written())
	{
		printk("No identity key found in KMU (required). Exiting!\n\r");
		return;
	}


	printk("Reading the identity key.\n\r");
	err = identity_key_read(key);
	if (err != 0){
		printk("Identity key read failed! (Error: %d). Exiting!\n\r", err);
		return;
	}

	printk("Importing the identity key into PSA crypto.\n\r");
	status = psa_import_key(&key_attributes, key, IDENTITY_KEY_SIZE_BYTES, &key_id_out);
	if (status != PSA_SUCCESS) {
		printk("psa_import_key failed! (Error: %d). Exiting!\n\r", status);
		return;
	}

	/* Clear out the key once it is imported */
	memset(key, 0, IDENTITY_KEY_SIZE_BYTES);


	printk("Exporting the public key corresponding to the identity key\n\r");
	status = psa_export_public_key(
			key_id_out,
			m_pub_key,
			sizeof(m_pub_key), 
			&olen);


	if (status != PSA_SUCCESS) {
		printk("psa_export_public_key failed! (Error: %d). Exiting!\n\r", status);
		return;
	}

	if (olen != IDENTITY_KEY_PUBLIC_KEY_SIZE)
	 {
		printk("Output length is invalid! (Expected %d, got %d). Exiting!\n\r",
			IDENTITY_KEY_PUBLIC_KEY_SIZE, olen);
		return;
	}

	printk("\n\r");
	printk("Public key:\n\r");
	hex_dump(m_pub_key, IDENTITY_KEY_PUBLIC_KEY_SIZE/2);
	hex_dump(m_pub_key + IDENTITY_KEY_PUBLIC_KEY_SIZE/2, IDENTITY_KEY_PUBLIC_KEY_SIZE/2);
	printk("\n\r");

	printk("Success!\n\r");
}
