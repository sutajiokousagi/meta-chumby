SRC_URI += "file://fstab.chumby"
PR .= "-amend-r7"
do_install_append() {
    install -m 0644 ${WORKDIR}/fstab.chumby ${D}${sysconfdir}/fstab
}

# We don't need all of the dirs mentioned in the default dirs755.  Mostly
# extra directories in /media.  Remove them from the list here.
# Also, change root's home path.
dirs755 = "/bin /boot /dev ${sysconfdir} ${sysconfdir}/default \
           ${sysconfdir}/skel /lib /mnt /proc /home /sbin /psp \
           ${prefix} ${bindir} ${docdir} /usr/games ${includedir} \
           ${libdir} ${sbindir} ${datadir} \
           ${datadir}/common-licenses ${datadir}/dict ${infodir} \
           ${mandir} ${datadir}/misc ${localstatedir} \
           ${localstatedir}/backups ${localstatedir}/lib \
           /sys ${localstatedir}/lib/misc ${localstatedir}/spool \
           ${localstatedir}/volatile ${localstatedir}/volatile/cache \
           ${localstatedir}/volatile/lock/subsys \
           ${localstatedir}/volatile/log \
           ${localstatedir}/volatile/run \
           /mnt /media /media/ram /media/net /media/storage \
"
