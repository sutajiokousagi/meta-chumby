PR .= "-amend-r1"

do_install_prepend() {
	# wwwroot is a symlink so that we can dynamically relocate it later
	sed 's|"/www/pages/"|"/www/netvserver"|g' -i ${WORKDIR}/lighttpd.conf
	install -d ${D}/www
	ln -sf /usr/share/netvserver/docroot ${D}/www/netvserver

	# log files needs to be elsewhere due to read-only filesystem
	sed 's|"/www/logs/"|"/tmp/"|g' -i ${WORKDIR}/lighttpd.conf
}
