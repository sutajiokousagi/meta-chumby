DESCRIPTION = "Dnsmasq is a lightweight, easy to configure DNS forwarder and DHCP server."
HOMEPAGE = "http://www.thekelleys.org.uk/dnsmasq/doc.html"
SECTION = "console/network"
LICENSE = "GPL"

PR = "r2-amend-r6"

# This used to be the OTG-debug package, but that was deprecated
# in order to separate functionality.  We make this RRECOMMEND the
# otg-debug package in order to force it to install when this package
# is updated.
RRECOMMENDS_${PN} = "chumby-otg-debug"

#at least versions 2.15 and prior are moved to the archive folder on the server
SRC_URI = "http://www.thekelleys.org.uk/dnsmasq/${@['archive/', ''][float(bb.data.getVar('PV',d,1).split('.')[1]) > 15]}dnsmasq-${PV}.tar.gz;name=dnsmasq-${PV} \
           file://init \
           file://dnsmasq.conf"

INITSCRIPT_NAME = "dnsmasq"
INITSCRIPT_PARAMS = "defaults"

do_install () {
        oe_runmake "PREFIX=${D}${prefix}" \
                   "BINDIR=${D}${bindir}" \
                   "MANDIR=${D}${mandir}" \
                   install
        install -d ${D}${sysconfdir}/ ${D}${sysconfdir}/init.d ${D}${sysconfdir}/dnsmasq.d
        install -m 644 ${WORKDIR}/dnsmasq.conf ${D}${sysconfdir}/
        install -m 755 ${WORKDIR}/init ${D}${sysconfdir}/init.d/dnsmasq
}

CONFFILES_${PN} = "${sysconfdir}/dnsmasq.conf"
