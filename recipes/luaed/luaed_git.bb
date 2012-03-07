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



pkg_postinst_${PN}() {
        CONF=/etc/lighttpd.conf

	if grep -q "/www/pages/luaed" ${CONF}
	then
		exit 0
	fi


	# Make /luaed/ point to the luaed directory
	echo 'alias.url = ( "/luaed/" => "/www/pages/luaed/" )' >> ${CONF}

        # Ensure mod_alias is enabled
        sed  's/.*"mod_alias".*/                                "mod_alias",/' -i ${CONF}


        /etc/init.d/lighttpd restart

	exit 0
}


pkg_postrm_${PN}() {
	sed '/\/www\/pages\/luaed\//d' -i /etc/lighttpd.conf
	exit 0
}

FILES_${PN} = "/www/pages/luaed"
