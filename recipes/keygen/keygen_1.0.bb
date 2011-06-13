inherit chumbysg-git-private
DESCRIPTION = "Cryptoprocessor daemon, used in lieu of a real processor"
LICENSE = "BSD"

#inherit update-rc.d

PR = "r0"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/src/keygen"
DEPENDS = "beecrypt"
RDEPENDS_${PN} = "beecrypt"

do_compile() {
    cd ${S}
    ${CC} keygen.c -o keygen ${CFLAGS} ${LDFLAGS} -lbeecrypt
}

do_install() {
	install -d ${D}/usr/bin
	install -m 0755 keygen ${D}/usr/bin
}

#INITSCRIPT_NAME = "netv_service"
#INITSCRIPT_PARAMS = "defaults 50 50"
