inherit chumbysg-git chumby-info

require recipes/linux/linux.inc

PR = "r30"
RDEPENDS_${PN} = "config-util"

COMPATIBLE_MACHINE = "chumby-silvermoon-netv"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/linux-2.6.28-silvermoon${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL};branch=netv \
           file://defconfig \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

# Mark archs/machines that this kernel supports
DEFAULT_PREFERENCE = "-1"
DEFAULT_PREFERENCE_chumby-silvermoon-netv = "1"

pkg_postinst_kernel_append() {
    config_util --cmd=putblock --dev=/dev/mmcblk0p1 --block=krnA < /boot/zImage
}
