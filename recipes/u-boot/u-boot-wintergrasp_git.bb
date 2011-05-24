inherit chumbysg-git chumby-info

require u-boot.inc

PROVIDES = ""
RPROVIDES = ""

PR ="r3"

SRC_URI = "${CHUMBYSG_GIT_HOST}/wintergrasp/u-boot-2009.08${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL} \
           file://0000-base-patches.patch \
"

#Relook after trip
#           file://0001-wintergrasp-Add-custom-board-type.patch \
#           file://0002-ethernet-Set-MAC-address-from-configuration.patch \
#           file://0003-u-boot-Fix-compile-due-to-missing-symbol-error.patch \
#           file://0004-u-boot-Speedup-ext2-access.patch \
#           file://0005-u-boot-Add-wintergrasp-config-to-Makefile.patch \
#           file://0006-update-wintergrasp-config.patch \
#"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"
