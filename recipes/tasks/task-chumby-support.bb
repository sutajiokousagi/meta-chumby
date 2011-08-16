inherit task
PR = "r3"

RDEPENDS_${PN} = " \
    regutil config-util \
    cpi dcid keygen \
    gnupg \
    chumby-networkmanager \
    imgtool \
    chumby-wget \
    chumby-updates \
    strace procps \
    networkmanager chumby-networkmanager \
    \
"

RRECOMMENDS_${PN} = " \
    perl \
"
