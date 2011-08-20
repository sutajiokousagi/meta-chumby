DESCRIPTION = "Utilities for driving NeTV"
LICENSE = "BSD"

inherit update-rc.d

INITSCRIPT_NAME = "netv_service"
INITSCRIPT_PARAMS = "defaults 50 50"

PR = "r46"

PACKAGE_ARCH = "${MACHINE}"

SRC_URI = "file://helpers/dumpreg.c \
	file://helpers/putreg.c \
	file://helpers/parse-edid.c \
	file://helpers/snoop.c \
	file://helpers/setbox.c \
	file://helpers/modeline.c \
	file://helpers/compute_ksv.c \
	file://helpers/derive_km.c \
	file://helpers/writecached_Km.c \
	file://helpers/fpga_ctl.c \
	file://helpers/chumby_xilinx.h \
	file://fpga/hdmi_overlay.bin \
	file://fpga/hdmi_720p.bin \
	file://fpga/min720p.edid \
	file://fpga/min1080p24.edid \
	file://fpga/1080p24_720p_480p.edid \
	file://fpga/41-chumby-netv.rules \
	file://helpers/dumptiming.c \
	file://helpers/netv_service \
	file://helpers/matchmoded.c \
	file://helpers/matchmoded_timings.h \
"

S = "${WORKDIR}"

do_compile() {
    cd ${S}
    ${CC} ${CFLAGS} ${LDFLAGS} -o dumpreg helpers/dumpreg.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o dumptiming helpers/dumptiming.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o putreg helpers/putreg.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o parse-edid.o -c helpers/parse-edid.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o snoop helpers/snoop.c parse-edid.o
    ${CC} ${CFLAGS} ${LDFLAGS} -o setbox helpers/setbox.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o modeline helpers/modeline.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o matchmoded helpers/matchmoded.c

    ${CC} ${CFLAGS} ${LDFLAGS} -c helpers/compute_ksv.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o derive_km helpers/derive_km.c compute_ksv.o
    ${CC} ${CFLAGS} ${LDFLAGS} -o writecached_Km helpers/writecached_Km.c

    ${CC} ${CFLAGS} ${LDFLAGS} -o fpga_ctl helpers/fpga_ctl.c
}

do_install() {
        install -d ${D}${sysconfdir}/init.d
	install -m 0755 helpers/netv_service ${D}${sysconfdir}/init.d/netv_service

	install -d ${D}${bindir}
	install -m 0755 dumpreg ${D}${bindir}
	install -m 0755 dumptiming ${D}${bindir}
	install -m 0755 putreg ${D}${bindir}
	install -m 0755 snoop ${D}${bindir}
	install -m 0755 setbox ${D}${bindir}
	install -m 0755 modeline ${D}${bindir}
	install -m 0755 derive_km ${D}${bindir}
	install -m 0755 writecached_Km ${D}${bindir}
	install -m 0755 fpga_ctl ${D}${bindir}

	install -d ${D}${sbindir}
	install -m 0755 matchmoded ${D}${sbindir}

	install -d ${D}/${base_libdir}/firmware
	install -m 0644 fpga/hdmi_overlay.bin ${D}${base_libdir}/firmware/
	install -m 0644 fpga/hdmi_720p.bin ${D}${base_libdir}/firmware/
	install -m 0644 fpga/min720p.edid ${D}${base_libdir}/firmware/
	install -m 0644 fpga/min1080p24.edid ${D}${base_libdir}/firmware/
	install -m 0644 fpga/1080p24_720p_480p.edid ${D}${base_libdir}/firmware/

	install -d ${D}${base_libdir}/udev/rules.d
	install -m 0644 fpga/41-chumby-netv.rules ${D}${base_libdir}/udev/rules.d

	install -d ${DEPLOY_DIR_IMAGE}
	install -m 0644 fpga/hdmi_720p.bin ${DEPLOY_DIR_IMAGE}
}

FILES_${PN} = "${bindir} ${sbindir}"
FILES_${PN} += "${base_libdir}/firmware/"
FILES_${PN} += "${base_libdir}/udev/rules.d/"
FILES_${PN} += "${sysconfdir}/init.d/"

pkg_postinst_${PN}_append() {
	config_util --cmd=putblock --dev=/dev/mmcblk0p1 --block=720p < ${base_libdir}/firmware/hdmi_720p.bin
}
