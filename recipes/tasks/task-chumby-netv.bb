inherit task
PR = "r5"

RDEPENDS_${PN} = " \
#      NeTVServer & Webkit browser (Control Panel)
       chumby-netvserver \
       chumby-netvbrowser \
       netv-controlpanel \

#      FPGA stuff
       netv-utils \

# Update scripts and conrtab entry
       chumby-updates \
"

RRECOMMENDS_${PN} = " \
    perl \
"
