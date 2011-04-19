inherit chumbysg-git chumby-info

require recipes/linux/linux.inc

PR = "r0"

COMPATIBLE_MACHINE = "chumby-wintergrasp"

PROVIDES = "virtual/kernel"

SRC_URI = "${CHUMBYSG_GIT_HOST}/wintergrasp/linux-2.6.35.3-wintergrasp;subpath=src;protocol=${CHUMBY_GIT_PROTOCOL} \
           file://0000-base-patches.patch \
           file://0001-pm-Ignore-spurrious-power-switch.patch \
           file://defconfig \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

# Mark archs/machines that this kernel supports
DEFAULT_PREFERENCE = "-1"
DEFAULT_PREFERENCE_chumby-wintergrasp = "1"
