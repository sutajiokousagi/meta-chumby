inherit task
PR = "r25"

CHUMBY_KERNEL_MODULES = "\
    kernel-module-cfg80211 kernel-module-chumby-xilinx \
    kernel-module-g-cdc kernel-module-g-ether kernel-module-g-file-storage \
    kernel-module-g-serial kernel-module-g-zero \
    kernel-module-unionfs \
"

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
       git \

# A separate kernel
       netv-recovery \
       ${CHUMBY_KERNEL_MODULES} \

# Chinese font
       font-wenquanyi \
"

RRECOMMENDS_${PN} = " \
       ca-certificates \
       perl \
"
