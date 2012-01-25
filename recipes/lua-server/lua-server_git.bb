inherit chumbysg-git update-rc.d

DESCRIPTION = "Lua motor control library for NeTV board"
LICENSE = "BSD"
#HOMEPAGE = "http://luaforge.net/projects/luasocket"

DEPENDS += "fastcgi"
RDEPENDS_${PN} += "fastcgi lighttpd"
RRECOMMENDS_${PN} += "luaed"

PR = "r2"
S = "${WORKDIR}/git"

SRC_URI = "${CHUMBYSG_GIT_HOST}/${PN}.git;protocol=${CHUMBYSG_GIT_PROTOCOL} \
	   file://init.sh \
"
SRCREV = "${AUTOREV}"

INITSCRIPT_NAME = "lua-server"
INITSCRIPT_PARAMS = "defaults 80 20"


do_compile() {
	oe_runmake
}



do_install() {
	oe_runmake install PREFIX=${D}/usr

	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.sh ${D}${sysconfdir}/init.d/lua-server

	install -d ${D}/www/luaed/projects
}



pkg_postinst_${PN}() {
	CONF=/etc/lighttpd.conf

	# Ignore lighttpd conf munging if it's already present
	if grep -q 'lua.socket' ${CONF}
	then
		exit 0
	fi

	# Add our configuration to the lighttpd conf file
	cat >> ${CONF} <<EOL
fastcgi.server += (
    "/lua/" =>
        ((
          "socket" => "/tmp/lua.socket",
          "check-local" => "disable",
        )),
    "/file/" =>
        ((
          "socket" => "/tmp/lua.socket",
          "check-local" => "disable",
        )),
)
EOL


	# Ensure mod_fastgti is enabled
	sed  's/.*"mod_fastcgi".*/                                "mod_fastcgi",/' -i ${CONF}


	/etc/init.d/lighttpd restart

	exit 0
}



pkg_postrm_${PN}() {
        CONF=/etc/lighttpd.conf

	if ! grep -q 'lua.socket' ${CONF}
	then
		echo "lua-server not present in lighttpd conf"
		exit 0
	fi

	# Figure out what line our little addition starts on
	LINE=$(($(grep -n lua.socket ${CONF} | cut -d: -f1 | head -n1)-3))
	echo "${PN} configuration begins on line ${LINE}"

	# Remove our config from the file
	sed ${LINE},$((${LINE}+11))d -i ${CONF}


	# Note: Leave fastcgi on in case it's used elsewhere

	exit 0
}

FILES_${PN} += "${sysconfdir}/init.d /www/luaed/projects"
