

# obj-y := irq.o iproc_timer.o timer.o sysfs.o 
obj-y := irq.o timer-sp.o sysfs.o bcm5301x.o iproc-cache.o headsmp.o shm.o
obj-$(CONFIG_SMP) += platsmp.o


obj-$(CONFIG_LOCAL_TIMERS) += localtimer.o
obj-$(CONFIG_HOTPLUG_CPU)  += hotplug.o
obj-$(CONFIG_USB_GADGET_DWC_OTG) += lm.o 
obj-$(CONFIG_HAVE_CLK) += clock.o
obj-$(CONFIG_ARCH_IPROC) += iproc_cru.o
obj-$(CONFIG_IPROC_SW_RESET_RECORD) += swreset_rec.o

export DRIVERS_MMC_HOST_DIR := drivers/mmc/host/
export DRIVERS_MTD_DIR := drivers/mtd/
obj-y+=../../../../../bcmdrivers/
