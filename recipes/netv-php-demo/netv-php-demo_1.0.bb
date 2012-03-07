inherit chumby-info

DESCRIPTION = "Install php-cgi, mod-rewrite and configure lighttpd for PHP support"
HOMEPAGE = "http://www.kosagi.com/"
AUTHOR = "Torin"
LICENSE = "GPLv3"
PR = "r1"
DEPENDS = " "
RDEPENDS_${PN} = "php-cgi lighttpd lighttpd-module-auth lighttpd-module-rewrite fastcgi"

do_compile() {
	true
}

do_install() {
    install -d ${D}/media/storage/docroot

	# Add our configuration to the lighttpd conf file
	cat >> ${D}/media/storage/docroot/phpinfo.php <<EOL
<?php phpinfo(); ?>
EOL

}

# Cron job to check network condition every 30 minutes
pkg_postinst() {
#!/bin/sh -e

	# Patch lighttpd.conf
	CONF=/etc/lighttpd.conf

	# Ignore lighttpd conf munging if it's already present
	if grep -q '/usr/bin/php-cgi' ${CONF}
	then
		exit 0
	fi

	# Add our configuration to the lighttpd conf file
	cat >> ${CONF} <<EOL
fastcgi.server += ( ".php" =>
                               ( "localhost" =>
                                 (
                                   "socket" => "/tmp/php-fastcgi.socket",
                                   "bin-path" => "/usr/bin/php-cgi"
                                 )
                               )
                            )
EOL

	# Ensure mod_fastcgi is enabled
	sed  's/.*"mod_fastcgi".*/                                "mod_fastcgi",/' -i ${CONF}

	# Ensure mod_rewrite is enabled
	sed  's/.*"mod_rewrite".*/                                "mod_rewrite",/' -i ${CONF}

	/etc/init.d/lighttpd restart
}

pkg_postrm_${PN}() {
    
	CONF=/etc/lighttpd.conf

	if ! grep -q 'fastcgi.server += ( ".php"' ${CONF}
	then
		echo "php-cgi not present in lighttpd conf"
		exit 0
	fi

	# Figure out what line our little addition starts on
	LINE=$(($(grep -n 'fastcgi.server += ( ".php"' ${CONF} | cut -d: -f1 | head -n1)-3))
	echo "${PN} configuration begins on line ${LINE}"

	# Remove our config from the file
	sed ${LINE},$((${LINE}+7))d -i ${CONF}

	# Note: Leave fastcgi & mod_rewrite on in case it's used elsewhere

	exit 0
}

# this puts it into a tidy package
FILES_${PN} += "/media/storage/docroot"
