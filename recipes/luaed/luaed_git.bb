inherit chumbysg-git

DESCRIPTION = "Web-based Lua IDE and editor"
LICENSE = "BSD"

RDEPENDS_${PN} += "lua-server"

PR = "r0"
S = "${WORKDIR}/git"

SRC_URI = "${CHUMBYSG_GIT_HOST}/${PN}.git;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"

do_compile() {
	true
}

do_install() {
	mkdir -p ${D}/www/pages/luaed
	cp -r ${S}/* ${D}/www/pages/luaed
}

FILES_${PN} = "/www/pages/luaed"
