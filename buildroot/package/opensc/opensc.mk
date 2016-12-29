##########################################################
#
# opensc
#
#########################################################
OPENSC_VERSION = 0.12.2
OPENSC_SOURCE = opensc-$(OPENSC_VERSION).tar.gz
OPENSC_SITE = http://cznic.dl.sourceforge.net/project/opensc/OpenSC/opensc-$(OPENSC_VERSION)
OPENSC_SITE_METHOD = wget
OPENSC_DEPENDENCIES = pcsc-lite host-pkgconf
LCDAPI_INSTALL_STAGING = YES

PCSC_CFLAGS = -I$(STAGING_DIR)/usr/include/PCSC
LTLIB_CFLAGS = -L$(STAGING_DIR)/usr/lib
LTLIB_LIBS += -ldl

OPENSC_CONF_OPT += --disable-doc --enable-pcsc --with-pcsc-provider=$(STAGING_DIR)/usr/lib/libpcsclite.so

$(eval $(autotools-package))
