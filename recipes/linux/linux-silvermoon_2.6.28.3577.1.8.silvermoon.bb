inherit chumbysg-git chumby-info

require recipes/linux/linux.inc

PR = "r0"

COMPATIBLE_MACHINE = "chumby-silvermoon"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_linux-2.6.28_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           file://0001-disable-cache-thingy.patch \
	   file://0002-fix-touchscreen-init.patch \
           file://defconfig \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

# Mark archs/machines that this kernel supports
DEFAULT_PREFERENCE = "-1"
DEFAULT_PREFERENCE_chumby-silvermoon = "1"
