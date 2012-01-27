PR .= "-amend-r5"

do_install_prepend() {
	install -d ${D}/www
	ln -sf /usr/share/netvserver/docroot ${D}/www/netvserver
}

# This needs to be in a postinst script rather than in do_install_prepend
# so that it is ensured that it will run.
# If we left it in postinst, then any upgraded config files would be placed
# in /etc/lighttpd.conf-opkg, and they wouldn't ever get used.  Furthermore,
# changes such as enabling lua-server or chumby-netvserver would get removed.
pkg_postinst_${PN}() {
	# Log files needs to be elsewhere due to read-only filesystem
	sed 's|"/www/logs|"/var/log|g' -i /etc/lighttpd.conf

	# wwwroot is a symlink so that we can dynamically relocate it later
	sed 's|"/www/pages/"|"/www/netvserver"|g' -i /etc/lighttpd.conf

	exit 0
}
