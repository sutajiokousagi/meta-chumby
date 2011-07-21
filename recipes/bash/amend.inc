PR .= "-amend-r1"
pkg_postinst_bash-sh() {
	if test "x$D" != "x"; then exit 1; fi  # Don't do postinst on build system
	ln -sf /bin/bash /bin/sh
}
