ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
dtbo-$(CONFIG_ARCH_BLAIR) += moonstone-overlay.dtbo
dtbo-$(CONFIG_ARCH_BLAIR) += sunstone-overlay.dtbo

moonstone-overlay.dtbo-base := moonstone.dtb
sunstone-overlay.dtbo-base := moonstone.dtb
else
dtb-$(CONFIG_ARCH_BLAIR) += moonstone.dtb
endif
