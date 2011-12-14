inherit chumbysg-git chumby-info

DESCRIPTION = "chumby adapters for NetworkManager"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Sean Cross"
LICENSE = "BSD"
PR = "r9"
DEPENDS = "dbus dbus-glib networkmanager glib-2.0 libxml2"

SRC_URI = "${CHUMBYSG_GIT_HOST}/${PN}.git;protocol=${CHUMBYSG_GIT_PROTOCOL} \
"
SRCREV = "c12a964b7092138c1f268965e3b1aeb7f464219e"
S = "${WORKDIR}/git"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 signal_strength ap_scan macgen.sh network_adapter_list.sh network_status.sh start_network ${D}${bindir}
    install -d ${D}/psp
}

