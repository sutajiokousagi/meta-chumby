inherit chumbysg-git chumby-info

DESCRIPTION = "chumby adapters for NetworkManager"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Sean Cross"
LICENSE = "BSD"
PR = "r3"
DEPENDS = "dbus dbus-glib networkmanager glib-2.0 libxml2"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/${PN};protocol=${CHUMBY_GIT_PROTOCOL} \
           file://network_configs \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ap_scan macgen.sh network_adapter_list.sh network_status.sh start_network ${D}${bindir}
    install -d ${D}/psp
    install -m 0755 ../network_configs ${D}/psp/network_configs
}

FILES_${PN} += "/psp"
