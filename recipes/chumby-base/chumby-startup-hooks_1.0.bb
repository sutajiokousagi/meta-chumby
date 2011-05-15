SRC_URI = "file://userhook"

#RDEPENDS_${PN} = "chumby-startup-userhook0 chumby-startup-userhook1 chumby-startup-userhook2 chumby-startup-debugchumby"

do_compile() {
	sed s/userhook-base/userhook0/ < userhook > userhook0.sh
	sed s/userhook-base/userhook1/ < userhook > userhook1.sh
	sed s/userhook-base/userhook2/ < userhook > userhook2.sh
	sed s/userhook-base/debugchumby/ < userhook > debugchumby.sh
}

do_install() {
        install -d ${D}${sysconfdir}/init.d
	install -m 0755 userhook0.sh ${D}${sysconfdir}/init.d/userhook0.sh
	install -m 0755 userhook1.sh ${D}${sysconfdir}/init.d/userhook1.sh
	install -m 0755 userhook2.sh ${D}${sysconfdir}/init.d/userhook2.sh
	install -m 0755 debugchumby.sh ${D}${sysconfdir}/init.d/debugchumby.sh
}

S = "${WORKDIR}"

FILES_chumby-startup-userhook0 = "${sysconfdir}/init.d/userhook0"
FILES_chumby-startup-userhook1 = "${sysconfdir}/init.d/userhook1"
FILES_chumby-startup-userhook2 = "${sysconfdir}/init.d/userhook2"
FILES_chumby-startup-debugchumby = "${sysconfdir}/init.d/debugchumby"

INITSCRIPT_PACKAGES = "chumby-startup-userhook0 chumby-startup-userhook1 chumby-startup-userhook2 chumby-startup-debugchumby"
INITSCRIPT_NAME_chumby-startup-userhook0 = "userhook0.sh"
INITSCRIPT_NAME_chumby-startup-userhook1 = "userhook1.sh"
INITSCRIPT_NAME_chumby-startup-userhook2 = "userhook2.sh"
INITSCRIPT_NAME_chumby-startup-debugchumby = "debugchumby.sh"
INITSCRIPT_PARAMS_chumby-startup-userhook0 = "defaults 20 21"
INITSCRIPT_PARAMS_chumby-startup-userhook1 = "defaults 30 20"
INITSCRIPT_PARAMS_chumby-startup-userhook2 = "defaults 70 20"
INITSCRIPT_PARAMS_chumby-startup-debugchumby = "defaults 99 20"

