/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <logging/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <psa/crypto.h>
#include <psa/crypto_extra.h>
#include <crypto_common.h>

LOG_MODULE_REGISTER(aes_ccm, LOG_LEVEL_DBG);

/* ====================================================================== */
/*				Global variables/defines for the AES CCM example		  */

#define NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE (100)
#define NRF_CRYPTO_EXAMPLE_AES_ADDITIONAL_SIZE (35)
#define NRF_CRYPTO_EXAMPLE_AES_CCM_NONCE_SIZE (13)
#define NRF_CRYPTO_EXAMPLE_AES_CCM_TAG_LENGTH (16)

/* AES sample nonce, DO NOT USE IN PRODUCTION */
static uint8_t m_nonce[NRF_CRYPTO_EXAMPLE_AES_CCM_NONCE_SIZE] = {
	'S', 'A', 'M', 'P', 'L', 'E', ' ', 'N', 'O', 'N', 'C', 'E'
};

/* Below text is used as plaintext for encryption/decryption */
static uint8_t m_plain_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE] = {
	"Example string to demonstrate basic usage of AES CCM mode."
};

/* Below text is used as additional data for authentication */
static uint8_t m_additional_data[NRF_CRYPTO_EXAMPLE_AES_ADDITIONAL_SIZE] = {
	"Example string of additional data"
};

static uint8_t m_encrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE +
				NRF_CRYPTO_EXAMPLE_AES_CCM_TAG_LENGTH];

static uint8_t m_decrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];

static psa_key_handle_t key_handle;
/* ====================================================================== */

int crypto_finish(void)
{
	psa_status_t status;

	/* Destroy the key handle */
	status = psa_destroy_key(key_handle);
	if (status != PSA_SUCCESS) {
		PRINT_ERROR("psa_destroy_key", status);
		return APP_ERROR;
	}

	return APP_SUCCESS;
}

int generate_key(void)
{
	psa_status_t status;

	PRINT_MESSAGE("Generating random AES key...");

	/* Configure the key attributes */
	psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

	psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
	psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_VOLATILE);
	psa_set_key_algorithm(&key_attributes, PSA_ALG_CCM);
	psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
	psa_set_key_bits(&key_attributes, 128);

	/* Generate a random key. The key is not exposed to the application,
	 * we can use it to encrypt/decrypt using the key handle
	 */
	status = psa_generate_key(&key_attributes, &key_handle);
	if (status != PSA_SUCCESS) {
		PRINT_ERROR("psa_generate_key", status);
		return APP_ERROR;
	}

	/* After the key handle is acquired the attributes are not needed */
	psa_reset_key_attributes(&key_attributes);

	PRINT_MESSAGE("AES key generated successfully!");

	return APP_SUCCESS;
}

int encrypt_ccm_aes(void)
{
	uint32_t output_len;
	psa_status_t status;

	PRINT_MESSAGE("Encrypting using AES CCM MODE...");

	/* Encrypt the plaintext and create the authentication tag */
	status = psa_aead_encrypt(key_handle,
							  PSA_ALG_CCM,
							  m_nonce,
							  sizeof(m_nonce),
							  m_additional_data,
							  sizeof(m_additional_data),
							  m_plain_text,
							  sizeof(m_plain_text),
							  m_encrypted_text,
							  sizeof(m_encrypted_text),
							  &output_len);

	if (status != PSA_SUCCESS) {
		PRINT_ERROR("psa_aead_encrypt", status);
		return APP_ERROR;
	}

	PRINT_MESSAGE("Encryption successful!");
	PRINT_HEX("Nonce", m_nonce, sizeof(m_nonce));
	PRINT_HEX("Additional data", m_additional_data, sizeof(m_additional_data));
	PRINT_HEX("Plaintext", m_plain_text, sizeof(m_plain_text));
	PRINT_HEX("Encrypted text", m_encrypted_text, sizeof(m_encrypted_text));

	PRINT_MESSAGE("Encryption successful!");

	return APP_SUCCESS;
}

int decrypt_ccm_aes(void)
{
	uint32_t output_len;
	psa_status_t status;

	PRINT_MESSAGE("Decrypting using AES CCM MODE...");

	/* Decrypt the encrypted data and authenticate the tag */
	status = psa_aead_decrypt(key_handle,
							  PSA_ALG_CCM,
							  m_nonce,
							  sizeof(m_nonce),
							  m_additional_data,
							  sizeof(m_additional_data),
							  m_encrypted_text,
							  sizeof(m_encrypted_text),
							  m_decrypted_text,
							  sizeof(m_decrypted_text),
							  &output_len);

	if (status != PSA_SUCCESS) {
		PRINT_ERROR("psa_aead_decrypt", status);
		return APP_ERROR;
	}

	PRINT_HEX("Decrypted text", m_decrypted_text, sizeof(m_decrypted_text));

	/* Check the validity of the decryption */
	if (memcmp(m_decrypted_text, m_plain_text, NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE) != 0) {
		PRINT_MESSAGE("Error: Decrypted text doesn't match the plaintext");
		return APP_ERROR;
	}

	PRINT_MESSAGE("Decryption and authentication successful!");

	return APP_SUCCESS;
}

int main(void)
{
	int status;

	PRINT_MESSAGE("Starting AES CCM example...");
	status = crypto_init();
	if (status != APP_SUCCESS) {
		PRINT_MESSAGE(APP_ERROR_MESSAGE);
		return APP_ERROR;
	}

	status = generate_key();
	if (status != APP_SUCCESS) {
		PRINT_MESSAGE(APP_ERROR_MESSAGE);
		return APP_ERROR;
	}

	status = encrypt_ccm_aes();
	if (status != APP_SUCCESS) {
		PRINT_MESSAGE(APP_ERROR_MESSAGE);
		return APP_ERROR;
	}

	status = decrypt_ccm_aes();
	if (status != APP_SUCCESS) {
		PRINT_MESSAGE(APP_ERROR_MESSAGE);
		return APP_ERROR;
	}

	status = crypto_finish();
	if (status != APP_SUCCESS) {
		PRINT_MESSAGE(APP_ERROR_MESSAGE);
		return APP_ERROR;
	}

	PRINT_MESSAGE(APP_SUCCESS_MESSAGE);

	return APP_SUCCESS;
}
