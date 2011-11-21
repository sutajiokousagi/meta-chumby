DESCRIPTION = "Latest wireless drivers"
HOMEPAGE = "http://wireless.kernel.org/en/users/Download"
SECTION = "kernel/modules"
LICENSE = "GPL"
RDEPENDS = "wireless-tools"
PR = "r0"

COMPAT_WIRELESS_VERSION = "2011-06-01"

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

SRC_URI[md5sum] = "6ea4dc9aba93449c10e7fb5bd161096b"
SRC_URI[sha256sum] = "e8007170f1c6925ee922f1b0e996a77546feef6786d17cacb24640d891fdb603"
