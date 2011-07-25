PR .= "-amend-r1"
SRC_URI += "file://ssh_host_dsa_key.pub.chumby \
            file://ssh_host_rsa_key.pub.chumby \
            file://ssh_host_dsa_key.chumby \
            file://ssh_host_rsa_key.chumby \
            file://sshd_config.chumby \
"

do_install_append() {
	install -d ${D}${sysconfdir}/ssh

	install -m 0644 ${WORKDIR}/sshd_config.chumby ${D}${sysconfdir}/ssh/sshd_config

	install -m 0644 ${WORKDIR}/ssh_host_dsa_key.pub.chumby ${D}${sysconfdir}/ssh/ssh_host_dsa_key.pub
	install -m 0644 ${WORKDIR}/ssh_host_rsa_key.pub.chumby ${D}${sysconfdir}/ssh/ssh_host_rsa_key.pub

	install -m 0600 ${WORKDIR}/ssh_host_dsa_key.chumby ${D}${sysconfdir}/ssh/ssh_host_dsa_key
	install -m 0600 ${WORKDIR}/ssh_host_rsa_key.chumby ${D}${sysconfdir}/ssh/ssh_host_rsa_key
}
