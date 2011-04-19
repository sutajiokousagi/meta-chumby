DESCRIPTION = "LibTomCrypt is a fairly comprehensive, modular \
and portable cryptographic toolkit that provides developers \
with a vast array of well known published block ciphers, \
one-way hash functions, chaining modes, pseudo-random number \
generators, public key cryptography and a plethora of other \
routines."
SECTION = "libs/math"
PRIORITY = "optional"
LICENSE = "PD"
PR = "r0"

SRC_URI = "http://libtom.org/files/tfm-${PV}.tar.bz2"

inherit autotools

do_stage() {
	oe_libinstall -a libtfm ${STAGING_LIBDIR}/
	install -m 0644 ${S}/src/headers/*.h ${STAGING_INCDIR}/
}

do_install() {
	:
}

SRC_URI[md5sum] = "821edbffb03502f0614c8717bda6fd54"
SRC_URI[sha256sum] = "aa854c6664c3ba173809326ad9d3c18918b52f84040658b247e2ce7527032021"
