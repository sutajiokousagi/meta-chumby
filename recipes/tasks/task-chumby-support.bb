inherit task
PR = "r1"

RDEPENDS_${PN} = " \
    regutil config-util \
    cpi dcid keygen \
    gnupg \
    chumby-networkmanager \
    imgtool \
    chumby-wget \
    chumby-updates \
    networkmanager chumby-networkmanager \
    \
    procps strace \
"

RRECOMMENDS_${PN} = " \
    perl \
"
