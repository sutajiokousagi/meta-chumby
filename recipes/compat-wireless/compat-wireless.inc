DESCRIPTION = "Latest wireless drivers"
HOMEPAGE = "http://wireless.kernel.org/en/users/Download"
SECTION = "kernel/modules"
LICENSE = "GPL"
RDEPENDS = "wireless-tools"
PR = "r1"

COMPAT_WIRELESS_VERSION = "2012-01-01"

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

SRC_URI[md5sum] = "2539d0c7283188dadfa312f42f5a82cb"
SRC_URI[sha256sum] = "d567c118c1600087e374483e8b49f748adce3c5727e21333c5f2e986c70de82a"
