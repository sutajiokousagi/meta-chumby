PR .= "-amend-r4"
SRC_URI += "file://0001-add-chumby-plugin.patch \
            file://0002-fix-username-attribute.patch \
            file://0003-add-config-file.patch \
            file://0004-remove-duplicate-code-from-chumby-plugin.patch \
            file://0005-cleanup-after-error-condition.patch \
            file://0006-add-psp-network_config-support.patch \
            file://0007-fix-wireless-security-config.patch \
            file://0008-make-xml-parser-more-robust.patch \
            file://nm-system-settings-chumby.conf \
"

do_compile_append () {
	cp ../nm-system-settings-chumby.conf ../nm-system-settings.conf
}

do_configure_prepend() {
	autoreconf
}
