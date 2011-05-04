PR .= "-amend-r1"
SRC_URI += "file://0001-add-chumby-plugin.patch \
            file://0002-fix-username-attribute.patch \
            file://0003-make-plugin-parsing-more-robust.patch \
            file://0004-cleanup-after-error.patch \
            file://nm-system-settings-chumby.conf \
"

do_compile_append () {
	cp ../nm-system-settings-chumby.conf ../nm-system-settings.conf
}

do_configure_prepend() {
	autoreconf
}
