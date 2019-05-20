/**
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <init.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zephyr.h>
#include <entropy.h>

#if !defined(CONFIG_MBEDTLS_CFG_FILE)
#include "mbedtls/config.h"
#else
#include CONFIG_MBEDTLS_CFG_FILE
#endif /* CONFIG_MBEDTLS_CFG_FILE */

#include "mbedtls/platform.h"
#include "platform_alt.h"

struct cc310_hw_dev_data {
	uint32_t 	rng_workbuff[1528];
	uint32_t 	is_initialized;
};

#define DEV_DATA(dev) \
	((struct cc310_hw_dev_data*)(dev)->driver_data)

static void cc310_hw_isr(struct device *device)
{
	/* Forward the ISR to a named function */
}

static int cc310_hw_init(struct device *dev)
{
	struct cc310_hw_dev_data * dev_data;
	int ret;

	static mbedtls_platform_context context;

	cc310_hw_isr(NULL);

	__ASSERT_NO_MSG(dev != NULL);

	dev_data = DEV_DATA(dev);

	__ASSERT_NO_MSG(dev_data != NULL);

        //context.cc310[0] = (uint32_t) &dev_data->rng_workbuff;

	//ret = mbedtls_platform_setup((mbedtls_platform_context*)dev_data);
	ret = mbedtls_platform_setup(&context);

	if(ret != 0)
		return ret;
	else
		return 0;
}

static struct cc310_hw_dev_data cc310_hw_data = {0,};

DEVICE_AND_API_INIT(cc310_hw, CONFIG_ENTROPY_NAME,
		    cc310_hw_init,
		    &cc310_hw_data,
		    NULL,
		    PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
		    NULL);

