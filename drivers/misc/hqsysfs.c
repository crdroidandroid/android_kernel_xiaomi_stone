// SPDX-License-Identifier: GPL-2.0-only
//
// Copyright (C) 2015 MediaTek Inc.
//               2024 The LineageOS Project
//

#include <linux/module.h>
#include <linux/device.h>
#include <linux/soc/qcom/smem.h>
#include <misc/hqsys_pcba.h>

#define SMEM_ID_VENDOR1 135

static struct class *huaqin_class;
static struct device *huaqin_device;
static struct kobject *hw_info_kobj;
static bool pcba_initialized = false;
static PCBA_CONFIG huaqin_pcba_config = PCBA_UNKNOW;

static const struct pcba_info pcba[] = {
	{ PCBA_UNKNOW, "PCBA_UNKNOW" },
	{ PCBA_M17_P0_1_CN, "PCBA_M17_P0-1_CN" },
	{ PCBA_M17_P0_1_GL, "PCBA_M17_P0-1_GL" },
	{ PCBA_M17_P0_1_IN, "PCBA_M17_P0-1_IN" },
	{ PCBA_M17_P0_1_CN_NEW, "PCBA_M17_P0-1_CN_NEW" },
	{ PCBA_M17P_P0_1_IN, "PCBA_M17P_P0-1_IN" },
	{ PCBA_M17P_P0_1_GL, "PCBA_M17P_P0-1_GL" },
	{ PCBA_M17P_P0_1_ID, "PCBA_M17P_P0-1_ID" },

	{ PCBA_M17_P1_CN, "PCBA_M17_P1_CN" },
	{ PCBA_M17_P1_GL, "PCBA_M17_P1_GL" },
	{ PCBA_M17_P1_IN, "PCBA_M17_P1_IN" },
	{ PCBA_M17_P1_CN_NEW, "PCBA_M17_P1_CN_NEW" },
	{ PCBA_M17P_P1_IN, "PCBA_M17P_P1_IN" },
	{ PCBA_M17P_P1_GL, "PCBA_M17P_P1_GL" },
	{ PCBA_M17P_P1_ID, "PCBA_M17P_P1_ID" },

	{ PCBA_M17_P1_1_CN, "PCBA_M17_P1-1_CN" },
	{ PCBA_M17_P1_1_GL, "PCBA_M17_P1-1_GL" },
	{ PCBA_M17_P1_1_IN, "PCBA_M17_P1-1_IN" },
	{ PCBA_M17_P1_1_CN_NEW, "PCBA_M17_P1-1_CN_NEW" },
	{ PCBA_M17P_P1_1_IN, "PCBA_M17P_P1-1_IN" },
	{ PCBA_M17P_P1_1_GL, "PCBA_M17P_P1-1_GL" },
	{ PCBA_M17P_P1_1_ID, "PCBA_M17P_P1-1_ID" },

	{ PCBA_M17_P2_CN, "PCBA_M17_P2_CN" },
	{ PCBA_M17_P2_GL, "PCBA_M17_P2_GL" },
	{ PCBA_M17_P2_IN, "PCBA_M17_P2_IN" },
	{ PCBA_M17_P2_CN_NEW, "PCBA_M17_P2_CN_NEW" },
	{ PCBA_M17P_P2_IN, "PCBA_M17P_P2_IN" },
	{ PCBA_M17P_P2_GL, "PCBA_M17P_P2_GL" },
	{ PCBA_M17P_P2_ID, "PCBA_M17P_P2_ID" },

	{ PCBA_M17_MP_CN, "PCBA_M17_MP_CN" },
	{ PCBA_M17_MP_GL, "PCBA_M17_MP_GL" },
	{ PCBA_M17_MP_IN, "PCBA_M17_MP_IN" },
	{ PCBA_M17_MP_CN_NEW, "PCBA_M17_MP_CN_NEW" },
	{ PCBA_M17P_MP_IN, "PCBA_M17P_MP_IN" },
	{ PCBA_M17P_MP_GL, "PCBA_M17P_MP_GL" },
	{ PCBA_M17P_MP_ID, "PCBA_M17P_MP_ID" },
};

PCBA_CONFIG get_huaqin_pcba_config(void)
{
	PCBA_CONFIG *pcba_config = NULL;
	size_t size;

	if (!pcba_initialized) {
		pcba_config = (PCBA_CONFIG *)qcom_smem_get(
			QCOM_SMEM_HOST_ANY, SMEM_ID_VENDOR1, &size);
		if (pcba_config) {
			pr_err("pcba config = %d 0x%x.\n", *(pcba_config),
			       *(pcba_config));
			if (*(pcba_config) > PCBA_UNKNOW &&
			    *(pcba_config) < PCBA_END) {
				huaqin_pcba_config = *pcba_config;
			} else {
				huaqin_pcba_config = PCBA_UNKNOW;
			}
		} else {
			pr_err("pcba config failed\n");
			huaqin_pcba_config = PCBA_UNKNOW;
		}
		pcba_initialized = true;
	}

	return huaqin_pcba_config;
}

static ssize_t pcba_config_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	int i;
	huaqin_pcba_config = get_huaqin_pcba_config();

	for (i = 0; i < ARRAY_SIZE(pcba); i++) {
		if (huaqin_pcba_config == pcba[i].pcba_config) {
			return sprintf(buf, "%s\n", pcba[i].pcba_name);
		}
	}

	return sprintf(buf, "PCBA_UNKNOWN\n");
}

static struct kobj_attribute attr_pcba_config = __ATTR_RO(pcba_config);

static struct attribute *huaqin_attrs[] = { &attr_pcba_config.attr, NULL };

static const struct attribute_group huaqin_attr_group = {
	.attrs = huaqin_attrs,
};

static int __init huaqin_sysfs_early_init(void)
{
	get_huaqin_pcba_config();
	return 0;
}

static int __init huaqin_sysfs_init(void)
{
	int ret;

	huaqin_class = class_create(THIS_MODULE, "huaqin");
	if (IS_ERR(huaqin_class))
		return PTR_ERR(huaqin_class);

	huaqin_device = device_create(huaqin_class, NULL, 0, NULL, "interface");
	if (IS_ERR(huaqin_device)) {
		ret = PTR_ERR(huaqin_device);
		goto err_device;
	}

	hw_info_kobj = kobject_create_and_add("hw_info", &huaqin_device->kobj);
	if (!hw_info_kobj) {
		ret = -ENOMEM;
		goto err_kobj;
	}

	ret = sysfs_create_group(hw_info_kobj, &huaqin_attr_group);
	if (ret)
		goto err_group;

	return 0;

err_group:
	kobject_put(hw_info_kobj);
err_kobj:
	device_destroy(huaqin_class, 0);
err_device:
	class_destroy(huaqin_class);
	return ret;
}

static void __exit huaqin_sysfs_exit(void)
{
	sysfs_remove_group(hw_info_kobj, &huaqin_attr_group);
	kobject_put(hw_info_kobj);
	device_destroy(huaqin_class, 0);
	class_destroy(huaqin_class);
}

subsys_initcall(huaqin_sysfs_early_init);
module_init(huaqin_sysfs_init);
module_exit(huaqin_sysfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huaqin");
MODULE_DESCRIPTION("Huaqin PCBA sysfs interface");
