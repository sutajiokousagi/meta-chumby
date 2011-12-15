inherit chumbysg-git

DESCRIPTION = "Lua motor control library for NeTV board"
LICENSE = "BSD"
#HOMEPAGE = "http://luaforge.net/projects/luasocket"

RDEPENDS_${PN} += "lua5.1"

PR = "r0"
S = "${WORKDIR}/git"

SRC_URI = "${CHUMBYSG_GIT_HOST}/${PN}.git;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "f74c6afa263b25f062d3b58a68476c9944cdef61"

LUA_LIB_DIR =  "${libdir}/lua/5.1"
LUA_SHARE_DIR = "${datadir}/lua/5.1"

do_compile() {
	oe_runmake
}

do_install() {
	mkdir -p ${D}/${LUA_LIB_DIR}
	cp ${S}/motor.so.2.0.2 ${D}/${LUA_LIB_DIR}/motor.so
}

FILES_${PN} = "${LUA_LIB_DIR}/motor.so"
