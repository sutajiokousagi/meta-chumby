PR .= "-amend-r0"

do_install_prepend() {
	sed 's|"/www/pages/"|"/www/netvserver"|g' -i ${WORKDIR}/lighttpd.conf
	install -d ${D}/www
	ln -sf /usr/share/netvserver/docroot ${D}/www/netvserver
}
