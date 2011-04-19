inherit chumbysg-git chumby-info

DESCRIPTION = "Flash player ALSA helper"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Henry Groover"
LICENSE = "GPLv2"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_utils-1.0_${RVERSION};subpath=src;protocol=${CHUMBY_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

do_compile() {
    ${CXX} ${CFLAGS} ${LDFLAGS} aplaypipe.cpp -o aplaypipe -DCNPLATFORM_${CNPLATFORM} `pkg-config alsa --cflags --libs`
}

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 aplaypipe ${D}${sbindir}
}

