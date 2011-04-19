inherit chumbysg-git chumby-info

require u-boot.inc

PROVIDES = ""
RPROVIDES = ""

PR ="r1"

SRC_URI = "${CHUMBYSG_GIT_HOST}/wintergrasp/u-boot-2009.08;protocol=${CHUMBY_GIT_PROTOCOL} \
           file://u-boot-2009.08.patch \
"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"
