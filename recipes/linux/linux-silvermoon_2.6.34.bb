inherit chumbysg-git chumby-info

require recipes/linux/linux.inc

PR = "r43"
RDEPENDS_${PN} = "config-util"

COMPATIBLE_MACHINE = "kovan"

SRC_URI = "https://github.com/xobs/linux/zipball/kovan"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

# Mark archs/machines that this kernel supports
DEFAULT_PREFERENCE = "-1"
DEFAULT_PREFERENCE_kovan = "1"

pkg_postinst_kernel_append() {
    config_util --cmd=putblock --dev=/dev/mmcblk0p1 --block=krnA < /boot/zImage
}
