inherit task
PR = "r23"

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
       netv-recovery \
"

RRECOMMENDS_${PN} = " \
	ca-certificates \
	perl \
"
