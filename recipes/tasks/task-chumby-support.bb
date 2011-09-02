inherit task
PR = "r5"

CHUMBY_TASK_EXTRAS = ""
CHUMBY_TASK_EXTRAS_chumby-silvermoon-netv = " \
    chumby-qt-reset \
    chumby-qt-remote \
    monitor-reset-button \
"

RDEPENDS_${PN} = " \
    ${CHUMBY_TASK_EXTRAS} \
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
