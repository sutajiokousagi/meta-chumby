inherit chumbysg-git

DESCRIPTION = "Lua motor control library for NeTV board"
LICENSE = "BSD"
#HOMEPAGE = "http://luaforge.net/projects/luasocket"

DEPENDS += "fastcgi"
RDEPENDS_${PN} += "fastcgi lighttpd luaed"

PR = "r0"
S = "${WORKDIR}/git"

SRC_URI = "${CHUMBYSG_GIT_HOST}/${PN}.git;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"

do_compile() {
	oe_runmake
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${S}/lua-server ${D}${bindir}/lua-server
}
