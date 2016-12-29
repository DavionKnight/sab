#############################################################
#
# Build the ubifs custom (non-root) filesystem image
#
#############################################################

UBIFS2_OPTS := -e $(BR2_TARGET_ALTFS_UBIFS_LEBSIZE) -c $(BR2_TARGET_ALTFS_UBIFS_MAXLEBCNT) -m $(BR2_TARGET_ALTFS_UBIFS_MINIOSIZE)

ifeq ($(BR2_TARGET_ALTFS_UBIFS_RT_ZLIB),y)
UBIFS2_OPTS += -x zlib
endif
ifeq ($(BR2_TARGET_ALTFS_UBIFS_RT_LZO),y)
UBIFS2_OPTS += -x lzo
endif
ifeq ($(BR2_TARGET_ALTFS_UBIFS_RT_NONE),y)
UBIFS2_OPTS += -x none
endif

CUSTFS_UBIFS_DEPENDENCIES = host-mtd host-fakeroot $(if $(BR2_TARGET_ALTFS_UBIFS_LZMA),host-lzma)

$(BINARIES_DIR)/custfs.ubifs: $(CUSTFS_UBIFS_DEPENDENCIES)
	@$(call MESSAGE,"Generating custom \(non-root\) filesystem image custfs.ubifs")
	echo "$(HOST_DIR)/usr/sbin/mkfs.ubifs -d $(BR2_TARGET_ALTFS_UBIFS_DIR) $(UBIFS2_OPTS) -o $@" >> $(FAKEROOT_SCRIPT)
	chmod a+x $(FAKEROOT_SCRIPT)
	$(HOST_DIR)/usr/bin/fakeroot -- $(FAKEROOT_SCRIPT)
	-@rm -f $(FAKEROOT_SCRIPT)
ifeq ($(BR2_TARGET_ALTFS_UBIFS_GZIP),y)
	gzip -9 -c $@ > $@.gz
endif
ifeq ($(BR2_TARGET_ALTFS_UBIFS_BZIP2),y)
	bzip2 -9 -c $@ > $@.bz2
endif
ifeq ($(BR2_TARGET_ALTFS_UBIFS_LZMA),y)
	$(LZMA) -9 -c $@ > $@.lzma
endif

custfs-ubifs-show-depends:
	@echo $(CUSTFS_UBIFS_DEPENDENCIES)

custfs-ubifs: $(BINARIES_DIR)/custfs.ubifs

ifeq ($(BR2_TARGET_ALTFS_UBIFS),y)
ifeq ($(call qstrip,$(BR2_TARGET_ALTFS_UBIFS_DIR)),)
$(error Source directory for custom UBIFS image is not specified, check your BR2_TARGET_ALTFS_UBIFS_DIR setting)
endif
TARGETS += custfs-ubifs
endif
