inherit chumby-info

DESCRIPTION = "WenQuanYi (Spring of Letters) Chinese font"
HOMEPAGE = "http://sourceforge.net/projects/wqy"
AUTHOR = "fangq"
LICENSE = "GPL"
PR = "r1"

SRC_URI = "${SOURCEFORGE_MIRROR}/wqy/wqy-microhei-${PV}.tar.gz"
SRC_URI[md5sum] = "a124c5c6606f4f3b733d3477380e9d2f"
SRC_URI[sha256sum] = "2802ac8023aa36a66ea6e7445854e3a078d377ffff42169341bd237871f7213e"
S = "${WORKDIR}/wqy-microhei"

# It doesn't work if installed to /usr/share/font
do_install() {
        install -d ${D}/usr/lib/fonts
        install -m 0644 wqy-microhei.ttc ${D}/usr/lib/fonts
}

# this puts it into a package
FILES_${PN} += "/usr/lib/fonts/wqy-microhei.ttc"


