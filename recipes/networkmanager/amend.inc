PR_pn-networkmanager .= "r10"
SRC_URI_pn-networkmanager = "\
    http://ftp.gnome.org/pub/GNOME/sources/NetworkManager/0.8/NetworkManager-0.8.1.tar.bz2 \
    file://remove-gtk-doc-make.patch \
    file://10-dont_require_ifup_for_lo.patch \
    file://nm-system-settings.conf \
    file://NetworkManager \
    file://gtk-doc.make \
    file://0001-respect-libnl-flags-also-in-backends-ip6-manager.patch \
            file://0001-add-chumby-plugin.patch \
            file://0002-fix-username-attribute.patch \
            file://0003-add-config-file.patch \
            file://0004-remove-duplicate-code-from-chumby-plugin.patch \
            file://0005-cleanup-after-error-condition.patch \
            file://0006-add-psp-network_config-support.patch \
            file://0007-fix-wireless-security-config.patch \
            file://0008-make-xml-parser-more-robust.patch \
            file://0009-make-security-tokens-persistent.patch \
            file://0010-fix-segfault-on-bad-network-configs.patch \
            file://0011-add-destructor-to-nm-chumby-connection.patch \
            file://0012-fix-hostapd-segfault.patch \
            file://nm-system-settings-chumby.conf \
"

do_compile_append_pn-networkmanager () {
	cp ../nm-system-settings-chumby.conf ../nm-system-settings.conf
}

do_configure_prepend_pn-networkmanager() {
	autoreconf
}
