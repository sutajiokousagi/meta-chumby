
# Due to a bug, wpa_supplicant doesn't include service files by default.  Add them.
do_install_append () {
	if [ -e ${S}/dbus/fi.epitest.hostap.WPASupplicant.service ]
	then
		sed -i -e s:/sbin:${sbindir}:g ${S}/dbus/fi.epitest.hostap.WPASupplicant.service
		install -d ${D}/${datadir}/dbus-1
		install -m 644 ${S}/dbus/fi.epitest.hostap.WPASupplicant.service ${D}/${datadir}/dbus-1/system-services
	fi

	if [ -e ${S}/dbus/fi.w1.wpa_supplicant1.service ]
	then
		sed -i -e s:/sbin:${sbindir}:g ${S}/dbus/fi.w1.wpa_supplicant1.service
		install -d ${D}/${datadir}/dbus-1
		install -m 644 ${S}/dbus/fi.w1.wpa_supplicant1.service ${D}/${datadir}/dbus-1/system-services
	fi
}
