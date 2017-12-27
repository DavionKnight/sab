# make ramdisk from directory ./rootfs
TMPDIR = ./tmp
#ROOTFS_DIR = ./nm81_nfs-2014-6-26
ROOTFS_DIR = ./rfs
#ROOTFS_DIR =/home/sxl/nfs/tmp_nm81-nfs/ramfs
MKIMAGE = mkimage
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
	dd if=/dev/zero of=ramdisk1 bs=$(BLOCK_SIZE) count=$(COUNT)
	dd if=/dev/zero of=ramdisk2 bs=$(BLOCK_SIZE) count=$(COUNT)
	sudo $(MKFS) -m 0 -F -N 5000  -b 4096 ./ramdisk1
	sudo $(MKFS) -m 0 -F -N 5000  -b 4096 ./ramdisk2
	sudo mount -t $(FS) -o loop ./ramdisk1 $(TMPDIR)
	sudo cp -dpR $(ROOTFS_DIR)/* $(TMPDIR)
	sudo cp extfiles/getmsg $(TMPDIR)/usr/bin/
	sudo sed -i 's/sys_pkg/ht201_sys/g' $(TMPDIR)/etc/init.d/rcS
	sleep 1
	sudo umount $(TMPDIR)
	sleep 1
	sudo mount -t $(FS) -o loop ./ramdisk2 $(TMPDIR)
	sudo cp -dpR $(ROOTFS_DIR)/* $(TMPDIR)
	sudo cp extfiles/getmsg $(TMPDIR)/usr/bin/
	sudo sed -i 's/sys_pkg/ht201e_sys/g' $(TMPDIR)/etc/init.d/rcS
	sleep 1
	sudo umount $(TMPDIR)
	sleep 1


ramdisk.gz: ramdisk FORCE
	gzip -f9 ramdisk1
	gzip -f9 ramdisk2

rootfs: ramdisk.gz
	$(MKIMAGE) \
 -A arm -O linux -T ramdisk -C gzip  \
 -n "Linux ramdisk" \
 -d ramdisk1.gz  $@	
	cp  $@ rootfs-ht201
	$(MKIMAGE) \
 -A arm -O linux -T ramdisk -C gzip  \
 -n "Linux ramdisk" \
 -d ramdisk2.gz  $@	
	cp  $@ rootfs-ht201e

uramdisk: ramdisk
	$(MKIMAGE) \
 -A arm -O linux -T ramdisk -C gzip -a $(KERNEL_LOAD_ADDR) \
-e $(KERNEL_ENTRY) -n "Linux ramdisk" \
-d ramdisk uramdisk
	cp uramdisk $(TARGET_DIR)

clean:
	sudo umount $(TMPDIR)
	rm uramdisk ramdisk.gz uramdisk.gz

