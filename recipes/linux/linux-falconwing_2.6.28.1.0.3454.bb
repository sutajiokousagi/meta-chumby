require recipes/linux/linux.inc

PR = "r0"

COMPATIBLE_MACHINE = "chumby-falconwing"

PROVIDES = "virtual/kernel"

SRC_URI = "http://files.chumby.com/source/falconwing/build3454/linux-2.6.28.mx233-falconwing-1.0.7-053111.tgz \
           file://defconfig \
"
S = "${WORKDIR}/linux-2.6.28.mx233-falconwing-1.0.7"

# Mark archs/machines that this kernel supports
DEFAULT_PREFERENCE = "-1"
DEFAULT_PREFERENCE_chumby-falconwing = "1"

SRC_URI[md5sum] = "29029e3c2d964e1bb32ae5c58f91460e"
SRC_URI[sha256sum] = "d458547493ab14071c624c26d7eda06c2af633884cff9f8399f312ef1b27467c"
