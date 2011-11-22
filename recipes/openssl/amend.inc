do_compile_prepend() {
	if [ -e ${S}/engines/openssl.ld ]
	then
		cp ${S}/engines/openssl.ld ${S}/engines/ccgost
	fi
}
