
SRC_URI = "\
           file://cleanup \
           file://my-asound.conf \
           file://my-NetworkManager.conf \
           file://pq_overlay.bin \
           file://pq-setfb.c \
           file://startup \
           file://pidgin.desktop \
           file://launcher-26.rc \
           file://launcher-29.rc \
           file://panels.xml \
           file://01-ntpd \
"

S = "${WORKDIR}"

do_compile() {
    ${CC} pq-setfb.c -o pq-setfb ${CFLAGS} ${LDFLAGS}
}

do_install() {
    install -d ${D}/usr/sbin
    install -d ${D}/usr/share/pq
    install -d ${D}/usr/share/applications
    install -d ${D}/lib/firmware
    install -d ${D}/etc/init.d

    install -m 0755 pq-setfb ${D}/usr/sbin

    install -m 0755 pidgin.desktop ${D}/usr/share/applications

    install -m 0755 my-asound.conf ${D}/usr/share/pq
    install -m 0755 my-NetworkManager.conf ${D}/usr/share/pq
    install -m 0755 panels.xml ${D}/usr/share/pq
    install -m 0755 launcher-26.rc ${D}/usr/share/pq
    install -m 0755 launcher-29.rc ${D}/usr/share/pq
    install -m 0755 01-ntpd ${D}/usr/share/pq

    install -m 0755 startup ${D}/etc/init.d
    install -m 0755 cleanup ${D}/etc/init.d

    install -m 0755 pq_overlay.bin ${D}/lib/firmware
}

pkg_postinst_${PN}() {
    config_util --cmd=putblock --block=720p < /lib/firmware/pq_overlay.bin
    update-rc.d startup defaults 2  98
    update-rc.d cleanup defaults 98 98
    update-rc.d -f netv_service remove
}

FILES_${PN} += "/lib/firmware/* /etc/init.d/* /usr/share/pq/* /usr/sbin/* /usr/share/applications/*"
