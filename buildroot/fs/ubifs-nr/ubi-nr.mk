#############################################################
#
# Embed the ubifs image into an ubi one
#
#############################################################

UBI2_UBINIZE_OPTS := -m $(BR2_TARGET_ALTFS_UBIFS_MINIOSIZE)
UBI2_UBINIZE_OPTS += -p $(BR2_TARGET_ALTFS_UBI_PEBSIZE)
ifneq ($(BR2_TARGET_ALTFS_UBI_SUBSIZE),0)
UBI2_UBINIZE_OPTS += -s $(BR2_TARGET_ALTFS_UBI_SUBSIZE)
endif

CUSTFS_UBI_DEPENDENCIES = custfs-ubifs

define CUSTFS_UBI_CMD
	cp fs/ubifs-nr/ubinize.cfg . ;\
	chmod +w ubinize.cfg ;\
	echo "image=$@fs" \
		>> ./ubinize.cfg ;\
	$(HOST_DIR)/usr/sbin/ubinize -o $@ $(UBI2_UBINIZE_OPTS) ubinize.cfg ;\
	rm ubinize.cfg
endef

$(BINARIES_DIR)/custfs.ubi: $(CUSTFS_UBI_DEPENDENCIES)
	@$(call MESSAGE,"Generating alternative \(non-root\) filesystem image custfs.ubi")
	echo "$(CUSTFS_UBI_CMD)" >> $(FAKEROOT_SCRIPT)
	chmod a+x $(FAKEROOT_SCRIPT)
	$(HOST_DIR)/usr/bin/fakeroot -- $(FAKEROOT_SCRIPT)
	-@rm -f $(FAKEROOT_SCRIPT)

custfs-ubi-show-depends:
	@echo $(CUSTFS_UBI_DEPENDENCIES)

custfs-ubi: $(BINARIES_DIR)/custfs.ubi

ifeq ($(BR2_TARGET_ALTFS_UBI),y)
TARGETS += custfs-ubi
endif
