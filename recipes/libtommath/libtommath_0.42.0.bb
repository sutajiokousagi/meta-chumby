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

SRC_URI = "http://libtom.org/files/ltm-${PV}.tar.bz2"

inherit autotools

do_stage() {
	oe_libinstall -a libtommath ${STAGING_LIBDIR}/
	install -m 0644 ${S}/*.h ${STAGING_INCDIR}/
}

do_install() {
	:
}

SRC_URI[md5sum] = "7380da904b020301be7045cb3a89039b"
SRC_URI[sha256sum] = "7b5c258304c34ac5901cfddb9f809b9b3b8ac7d04f700cf006ac766a923eb217"
