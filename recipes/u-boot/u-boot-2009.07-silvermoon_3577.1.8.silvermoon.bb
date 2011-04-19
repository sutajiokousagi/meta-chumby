inherit chumbysg-git chumby-info

require u-boot.inc

PR = "r0"

PROVIDES = "virtual/bootloader"
RPROVIDES_${PN} = "virtual/bootloader"
COMPATIBLE_MACHINE = "chumby-silvermoon"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_u-boot-2009.07_${RVERSION};subpath=src;protocol=${CHUMBY_GIT_PROTOCOL} \
           file://0001-fix-makefile.patch \
           file://0002-fix-cmdline.patch \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

DEFAULT_PREFERENCE = "-1"
DEFAULT_PREFERENCE_chumby-silvermoon = "1"

