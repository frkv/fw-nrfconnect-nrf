#
# Copyright (c) 2019 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

# Dependencies
# -nrf_cc310_mbedcrypt
# -mbedtls_platform_setup that is not glued
# -Access to replacement mbedtls_platform_context (that is not glued)
# -sha-256 that is not glued
# -entropy that is not glued


zephyr_library()
zephyr_library_sources_ifdef(CONFIG_CC310_HW cc310_hw.c)