
# Due to a bug, wpa_supplicant doesn't include service files by default.  Add them.
do_install_append () {
	sed -i -e s:/sbin:${sbindir}:g ${S}/dbus/fi.epitest.hostap.WPASupplicant.service
	install -m 644 ${S}/dbus/fi.epitest.hostap.WPASupplicant.service ${D}/${datadir}/dbus-1/system-services

	sed -i -e s:/sbin:${sbindir}:g ${S}/dbus/fi.w1.wpa_supplicant1.service
	install -m 644 ${S}/dbus/fi.w1.wpa_supplicant1.service ${D}/${datadir}/dbus-1/system-services
}
