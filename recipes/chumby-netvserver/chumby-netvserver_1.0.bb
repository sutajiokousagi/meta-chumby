inherit chumbysg-git chumby-info qt4e
inherit update-rc.d

DESCRIPTION = "Hardware bridge for NeTV; implemented as a FastCGI server"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Torin"
LICENSE = "GPLv3"
PR = "r211"
DEPENDS = "qt4-embedded fastcgi"
RDEPENDS_${PN} = "task-qt4e-minimal curl fastcgi"

SRC_URI = "${CHUMBYSG_GIT_HOST}/${PN};protocol=${CHUMBYSG_GIT_PROTOCOL}"

SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git/src"
DOCROOT = "${WORKDIR}/git/etc/docroot"

INITSCRIPT_NAME = "chumby-netvserver"
INITSCRIPT_PARAMS = "defaults 30 70"

do_install() {
    install -d ${D}${bindir}
    install -d ${D}${sysconfdir}/init.d
    install -d ${D}/usr/share/netvserver
    install -d ${D}/usr/share/netvserver/cookies
    install -d ${D}/usr/share/netvserver/docroot/scripts
    install -d ${D}/usr/share/netvserver/docroot/html_test
    install -d ${D}/usr/share/netvserver/docroot/html_remote
    install -d ${D}/usr/share/netvserver/docroot/html_config
    install -d ${D}/usr/share/netvserver/docroot/html_update
    install -d ${D}/usr/share/netvserver/docroot/html_motor
    install -d ${D}/usr/share/netvserver/docroot/tests
    install -d ${D}/usr/share/netvserver/docroot/tmp

    install -m 0755 ${S}/bin/NeTVServer                      ${D}${bindir}
    install -m 0755 ${WORKDIR}/git/etc/NeTVServer.ini        ${D}${sysconfdir}
    install -m 0755 ${WORKDIR}/git/etc/chumby-netvserver.sh  ${D}${sysconfdir}/init.d/chumby-netvserver
    install -m 0755 ${DOCROOT}/favicon.ico                   ${D}/usr/share/netvserver/docroot
    install -m 0755 ${DOCROOT}/html_remote.html              ${D}/usr/share/netvserver/docroot

    cp -rf ${DOCROOT}/html_test                              ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/html_remote                            ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/html_config                            ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/html_update                            ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/html_motor                             ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/tests	                             	 ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/scripts                                ${D}/usr/share/netvserver/docroot
    chmod +x ${D}/usr/share/netvserver/docroot/scripts/*
}

# Cron jobs
pkg_postinst_${PN}() {
#!/bin/sh -e

	# Symlink to volatile memory for caching downloaded thumbnails
	if [ ! -e /usr/share/netvserver/docroot/tmp/netvserver ]; then
    	ln -sf /tmp /usr/share/netvserver/docroot/tmp/netvserver
	fi

	# Create a symlink for lighttpd to point to
	if [ ! -e /www/netvserver ]; then
	   mkdir /www || true
 	   ln -s /usr/share/netvserver/docroot /www/netvserver
	   echo "created default docroot symlink /www/netvserver -> /usr/share/netvserver/docroot"
	fi

	# Cron job: Check for valid Internet connection & otherwise respawn wlan interface
    ROOTCRON=/var/cron/tabs/root

#    if [ -e ${ROOTCRON} ];
#	then
		# Check valid Internet access & respawn wlan interface if necessary
		if grep -q '^[^#].*check_network.sh' $ROOTCRON
		then
			echo "cron job for check_network.sh already exists"
		else
			echo "05,35 * * * * /usr/share/netvserver/docroot/scripts/check_network.sh" >> $ROOTCRON
			echo "added new cron job for check_network.sh"
		fi

		# Automatic update cpanel git repository once every day
		if grep -q '^[^#].*updatecpanel.sh' $ROOTCRON
		then
			echo "cron job for updatecpanel.sh already exists"
		else
			echo "24 0 * * * /usr/share/netvserver/docroot/scripts/updatecpanel.sh >> /var/log/cron_updatecpanel.log 2>&1" >> $ROOTCRON
			echo "added new cron job for updatecpanel.sh"
		fi

		# Automatic update /psp/homepage symlink every hour
		if grep -q '^[^#].*psphomepage.sh' $ROOTCRON
		then
			echo "cron job for psphomepage.sh already exists"
		else
			echo "39 * * * * /usr/share/netvserver/docroot/scripts/psphomepage.sh >> /var/log/cron_psphomepage.log 2>&1" >> $ROOTCRON
			echo "added new cron job for psphomepage.sh"
		fi

		echo "restarting cron..."
  		/etc/init.d/cron restart
#	fi


	# Patching lighttpd.conf
	CONF=/etc/lighttpd.conf

	# Ignore lighttpd conf patching for /bridge if it's already present
	if grep -q 'bridge.socket' ${CONF}
	then
		exit 0
		echo "lighttpd is already patched for /bridge"
	else

		# Add our configuration to the lighttpd conf file
		cat >> ${CONF} <<EOL

fastcgi.server += (
    "/bridge" =>
        ((
          "socket" => "/tmp/bridge.socket",
          "check-local" => "disable",
        ))
)

EOL
		echo "added lighttpd config for NeTVServer (/tmp/bridge.socket)"
	fi

	# Ensure mod_fastcgi is enabled
	sed  's/.*"mod_fastcgi".*/                                "mod_fastcgi",/' -i ${CONF}

	#
	# Allow execution of shell script
	#

	# Ensure mod_cgi is enabled
	sed  's/.*"mod_cgi".*/                             	  "mod_cgi",/' -i ${CONF}

	# Shell script files (.sh) are not to be downloaded as static files
	sed 's|".pl", ".fcgi"|".pl", ".sh", ".fcgi"|g' -i /etc/lighttpd.conf

	# Ignore lighttpd conf patching for CGI if it's already present
	if grep -q '".sh" => ""' ${CONF}
	then
		exit 0
		echo "lighttpd is already patched for .sh"
	else

		# Add our configuration to the lighttpd conf file
		# /usr/bin/perl is not needed in this config
	cat >> ${CONF} <<EOL

cgi.assign += ( ".pl"  => "",
             	".sh" => "",
                ".cgi" => "" )

EOL
		echo "added lighttpd config for CGI"
	fi

	echo "Restarting lighttpd..."
	/etc/init.d/lighttpd restart
}

pkg_postrm_${PN}() {
    
	CONF=/etc/lighttpd.conf

	# Remove config for /bridge
	if grep -q 'bridge.socket' ${CONF}
	then
		# Figure out what line our little addition starts on
		LINE=$(($(grep -n bridge.socket ${CONF} | cut -d: -f1 | head -n1)-3))
		echo "${PN} configuration begins on line ${LINE}"

		# Remove our config from the file
		sed ${LINE},$((${LINE}+6))d -i ${CONF}
	fi

	# Note: Leave fastcgi on in case it's used elsewhere
	# Note: Leave CGI/Perl on

	exit 0
}

# this puts it into a tidy package
FILES_${PN}-dbg += "/usr/share/netvserver/.debug"
FILES_${PN} += "/usr/share/netvserver"
FILES_${PN} += ${bindir}
FILES_${PN} += ${sysconfdir}
FILES_${PN} += ${sysconfdir}/init.d
FILES_${PN} += "/www"

