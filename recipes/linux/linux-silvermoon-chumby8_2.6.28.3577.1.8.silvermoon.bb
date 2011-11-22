inherit chumbysg-git chumby-info

require recipes/linux/linux.inc

PR = "r0"

COMPATIBLE_MACHINE = "chumby-silvermoon-chumby8"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/linux-2.6.28-silvermoon.git;subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           file://0001-disable-cache-thingy.patch \
	   file://0002-fix-touchscreen-init.patch \
           file://defconfig \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

# Mark archs/machines that this kernel supports
DEFAULT_PREFERENCE = "-1"
DEFAULT_PREFERENCE_chumby-silvermoon-chumby8 = "1"
