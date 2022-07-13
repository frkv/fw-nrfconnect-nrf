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
#include <identity_key.h>
#include <psa/crypto.h>


static int generate_key(uint8_t key[IDENTITY_KEY_SIZE_BYTES])
{
	psa_status_t status;
	psa_key_handle_t key_handle;
	size_t olen;

	/* Initialize PSA Crypto */
	status = psa_crypto_init();
	if(status != PSA_SUCCESS) {
		printk("Failure: Could not initialize PSA crypto!\n\r");
		return -ERR_IDENTITY_KEY_INTERNAL_ERROR;
	}

	/* Configure the key attributes */
	psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

	/* Configure the key attributes for Curve type secp256r1*/
	/* This key needs to be exported from the volatile storage */
	psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_EXPORT);
	psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_VOLATILE);
	psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
	psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
	psa_set_key_bits(&key_attributes, 256);

	status = psa_generate_key(&key_attributes, &key_handle);
	if (status != PSA_SUCCESS) {
		printk("psa_generate_key failed! (Error: %d)\n\r", status);
		return -ERR_IDENTITY_KEY_GENERATE_FAILED;
	}

	status = psa_export_key(key_handle, key, IDENTITY_KEY_SIZE_BYTES, &olen);
	if (status != PSA_SUCCESS) {
		printk("psa_export_key failed! (Error: %d)\n\r", status);
		return -ERR_IDENTITY_KEY_EXPORT_FAILED;
	}

	if (olen != IDENTITY_KEY_SIZE_BYTES)
	{
		printk("Exported key size failed! (got: %d, expected: %d)\n\r", olen, IDENTITY_KEY_SIZE_BYTES);
		return -ERR_IDENTITY_KEY_EXPORT_FAILED;
	}

	status = psa_destroy_key(key_handle);
	if (status != PSA_SUCCESS)
	{
		printk("Exported key size failed! (got: %d, expected: %d)\n\r", olen, IDENTITY_KEY_SIZE_BYTES);
		return -ERR_IDENTITY_KEY_INTERNAL_ERROR;
	}

	return 0;
}


void main(void)
{
	int err;

	uint8_t key[IDENTITY_KEY_SIZE_BYTES];
	uint8_t key_copy[IDENTITY_KEY_SIZE_BYTES];

	printk("Generating random HUK keys\n\r");
	hw_unique_key_write_random();
	
	if(identity_key_is_written()){
		printk("Failure: Identity key slot already written. Exiting!\n\r");
		goto exit;
	}
		
	printk("Generating the identity key\n\r");
	err = generate_key(key);
	if (err != 0)
	{
		printk("Failure: Generate key failed, Exiting!!\n\r");
		goto exit;
	}

	printk("Writing the identity key to KMU\n\r");
	identity_key_write(key);
	
	printk("Reading back the identity key to check\n\r");
	err = identity_key_read(key_copy);
	if (err != 0)
	{
		printk("Failure: Reading back the identity key failed. Exiting!\n\r");
		goto exit;
	}

	if (memcmp(key, key_copy, IDENTITY_KEY_SIZE_BYTES) != 0)
	{
		printk("Falure: Identity key is not correctly written. Exiting!\n\r");
		goto exit;
	}

	printk("Success!\n\r");
exit:
	memset(key, 0, IDENTITY_KEY_SIZE_BYTES);
	memset(key_copy, 0, IDENTITY_KEY_SIZE_BYTES);
}
