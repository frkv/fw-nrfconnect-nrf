/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef IDENTITY_KEY_H_
#define IDENTITY_KEY_H_


/**
 * @file
 * @defgroup identity_key Identity key APIs
 * @{
 *
 * @brief API for identity key on CryptoCell devices with KMU
 */


#include "hw_unique_key.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Identity key size in bytes, corresponding to ECC secp256r1 */
#define IDENTITY_KEY_SIZE_BYTES		        (32)

/** @brief Error value when MKEK is not written in KMU */
#define ERR_IDENTITY_KEY_MKEK_MISSING 	    0x15501

/** @brief Error value when identity key is missing in KMU */
#define ERR_IDENTITY_KEY_MISSING 	        0x15501

/** @brief Error value when identity key can't be read */
#define ERR_IDENTITY_KEY_READ_FAILED	    0x15502

/** @brief Error value when identity key can't be generated */
#define ERR_IDENTITY_KEY_GENERATE_FAILED	0x15503

/** @brief Error value when identity key can't exported to public key */
#define ERR_IDENTITY_KEY_EXPORT_FAILED		0x15504

/** @brief Error value when identity key APIs has an internal error */
#define ERR_IDENTITY_KEY_INTERNAL_ERROR		0x15504

/**
 * @brief Function to check that the MKEK is present
 *
 * @details MKEK is a prerequisite for identity key APIs to encrypt
 *          and decrypt the key.
 *
 * @return true if MKEK is written, otherwise false
 */

bool identity_key_mkek_is_written(void);

/**
 * @brief Function to check if identity key is written
 *
 * @return true if MKEK is present, otherwise false
 */
bool identity_key_is_written(void);

/**
 * @brief Function to read the identity key from KMU
 *
 * @details The key is read from KMU and decrypted using
 *          the Master Key Encryption Key (MKEK).
 *
 * @param key   Buffer to hold the decrypted identity key
 *
 * @return Zero on success, otherwise a non-zero error code
 */
int identity_key_read(uint8_t key[IDENTITY_KEY_SIZE_BYTES]);

/**
 * @brief Function to write an identity key to KMU
 *
 * @details The identity key will be encrypted using the Master Key Encryption Key (MKEK).
 *          This API will call a panic-function if the write failed.
 *
 * @param key Buffer containing the identity key to write to KMU
 */
void identity_key_write(uint8_t key[IDENTITY_KEY_SIZE_BYTES]);


#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* IDENTITY_KEY_H_ */
