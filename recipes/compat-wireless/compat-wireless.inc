DESCRIPTION = "Latest wireless drivers"
HOMEPAGE = "http://wireless.kernel.org/en/users/Download"
SECTION = "kernel/modules"
LICENSE = "GPL"
RDEPENDS = "wireless-tools"
PR = "r3"

COMPAT_WIRELESS_VERSION = "2011-04-17"

SRC_URI = "http://wireless.kernel.org/download/compat-wireless-2.6/compat-wireless-${COMPAT_WIRELESS_VERSION}.tar.bz2 \
	"

TAG = "${@'${COMPAT_WIRELESS_VERSION}'.replace('-', '')}"
PV = "0.${TAG}"

S = "${WORKDIR}/compat-wireless-${COMPAT_WIRELESS_VERSION}"

inherit module

EXTRA_OEMAKE = "KLIB_BUILD=${STAGING_KERNEL_DIR} KLIB=${D}"

do_configure_append() {
	sed -i "s#@./scripts/update-initramfs## " Makefile
}

do_install() {
	oe_runmake DEPMOD=echo DESTDIR="${D}" INSTALL_MOD_PATH="${D}" LDFLAGS="" install-modules
}

SRC_URI[md5sum] = "96d047a7cef1f0541e741290f64b466c"
SRC_URI[sha256sum] = "633e302019c328e0f8fba8d69927e533dd4f70d41afbe007c506f709d2bd6059"
