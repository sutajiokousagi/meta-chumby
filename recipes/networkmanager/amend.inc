PR .= "-amend"
SRC_URI += "file://0001-add-chumby-plugin.patch \
            file://0002-fix-username-attribute.patch \
            file://nm-system-settings-chumby.conf \
"

do_compile_append () {
	cp ../nm-system-settings-chumby.conf ../nm-system-settings.conf
}

do_configure_prepend() {
	autoreconf
}
