# Builds the boot partition for a chumby Falconwing-based platform.
# Note that bootstream-chumby.bin is a binary-blob combination of
# chumby_stub, and the Freescale-provided bootlets that set up power,
# clocks, and RAM.  It is provided as a blob because Freescale provides
# elftosb2 as a blob.  If you would like to recreate these from scratch and
# are on an x86 Linux machine, you'll need the following files:
#   http://files.chumby.com/source/falconwing/bootloader/elftosb2;name=elftosb2
#   http://files.chumby.com/source/falconwing/bootloader/falconwing_chumby_sb.db;name=chumbyconfig
#   http://files.chumby.com/source/falconwing/bootloader/falconwing_factory_sb.db;name=factoryconfig

SECTION = "bootloaders"
PRIORITY = "optional"
LICENSE = "GPLv2"
PR = "r8"

SRC_URI = "file://bootstream-chumby.bin"

COMPATIBLE_MACHINE = "chumby-falconwing"
PACKAGE_ARCH = "${MACHINE}"
PROVIDES = "virtual/chumby-bootimage"

do_install () {
    install -d ${DEPLOY_DIR_IMAGE}
    install -m 0755 ${WORKDIR}/bootstream-chumby.bin ${DEPLOY_DIR_IMAGE}
}

