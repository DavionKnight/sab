# make ramdisk from directory ./rootfs
TMPDIR = ./tmp
#ROOTFS_DIR = ./nm81_nfs-2014-6-26
ROOTFS_DIR = ./rfs
#ROOTFS_DIR =/home/sxl/nfs/tmp_nm81-nfs/ramfs
MKIMAGE = ./mkimage
FS = ext3
MKFS = mkfs.$(FS)
LOAD_ADDR=0x0
ENTRY_ADDR=0x0
TARGET_DIR =  ~/tftpboot
COUNT = 50
BLOCK_SIZE = 1M

all:rootfs



#make a null ramdisk file
.PHONY: FORCE

make_tmp:FORCE
	if [ ! -d $(TMPDIR) ]; then mkdir $(TMPDIR); fi

ramdisk: make_tmp FORCE
	dd if=/dev/zero of=ramdisk bs=$(BLOCK_SIZE) count=$(COUNT)
	sudo $(MKFS) -m 0 -F -N 5000  -b 4096 ./ramdisk
#	-sudo umount $(TMPDIR)
	sudo mount -t $(FS) -o loop ./ramdisk $(TMPDIR)
	sudo cp -dpR $(ROOTFS_DIR)/* $(TMPDIR)
	sudo cp extfiles/demo $(TMPDIR)
	sudo cp extfiles/fpga $(TMPDIR)
	sudo cp extfiles/wdt.ko $(TMPDIR)/usr/bin/
	sudo umount $(TMPDIR)


ramdisk.gz: ramdisk FORCE
	gzip -f9 ramdisk

rootfs: ramdisk.gz
	$(MKIMAGE) \
 -A ppc -O linux -T ramdisk -C gzip  \
 -n "Linux ramdisk" \
 -d $< $@	
#	cp  $@ ../1604C/rootfs

uramdisk: ramdisk
	$(MKIMAGE) \
 -A ppc -O linux -T ramdisk -C gzip -a $(KERNEL_LOAD_ADDR) \
-e $(KERNEL_ENTRY) -n "Linux ramdisk" \
-d ramdisk uramdisk
	cp uramdisk $(TARGET_DIR)

clean:
	sudo umount $(TMPDIR)
	rm uramdisk ramdisk.gz uramdisk.gz

