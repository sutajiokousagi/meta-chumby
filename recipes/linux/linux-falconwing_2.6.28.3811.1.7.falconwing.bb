inherit chumbysg-git chumby-info

require recipes/linux/linux.inc

PR = "r0"

COMPATIBLE_MACHINE = "chumby-falconwing"

PROVIDES = "virtual/kernel"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_linux-2.6.28.mx233_${RVERSION};subpath=src;protocol=${CHUMBY_GIT_PROTOCOL} \
           file://defconfig \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

# Mark archs/machines that this kernel supports
DEFAULT_PREFERENCE = "-1"
DEFAULT_PREFERENCE_chumby-falconwing = "1"
