inherit task
PR = "r33"
DEPENDS += "iw"

CHUMBY_KERNEL_MODULES = "\
    kernel-module-cfg80211 kernel-module-chumby-xilinx \
    kernel-module-g-cdc kernel-module-g-ether kernel-module-g-file-storage \
    kernel-module-g-serial kernel-module-g-zero \
    kernel-module-unionfs \
"

RDEPENDS_${PN} = " \
#      lighttpd & plugins
       lighttpd \
       lighttpd-module-fastcgi \

# Testing
       lighttpd-module-php \
       sqlite \

#      NeTVServer & Webkit browser (Control Panel)
       chumby-netvserver \
       chumby-netvbrowser \
       netv-controlpanel \

#      FPGA stuff
       netv-utils \

       watchdog \
       iw \

# Update scripts and crontab entry
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
