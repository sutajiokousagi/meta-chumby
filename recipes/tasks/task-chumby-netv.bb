inherit task
PR = "r20"

DEPENDS += "netv-recovery-blob"

RDEPENDS_${PN} = " \
#      NeTVServer & Webkit browser (Control Panel)
       chumby-netvserver \
       chumby-netvbrowser \
       netv-controlpanel \

#      FPGA stuff
       netv-utils \

       watchdog \

# Update scripts and conrtab entry
       chumby-updates \

# A separate kernel
       netv-recovery-blob \
"

RRECOMMENDS_${PN} = " \
    perl \
"
