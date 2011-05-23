DESCRIPTION = "Utilities for driving NeTV"
LICENSE = "BSD"

PR = "r1"

SRC_URI = "file://helpers/dumpreg.c \
	file://helpers/putreg.c \
	file://helpers/parse-edid.c \
	file://helpers/snoop.c \
	file://helpers/setbox.c \
	file://helpers/modeline.c \
	file://helpers/compute_ksv.c \
	file://helpers/derive_km.c \
	file://helpers/fpga_ctl.c \
	file://helpers/chumby_xilinx.h \
	file://fpga/hdmi_overlay.bin \
"

S = "${WORKDIR}"

do_compile() {
    cd ${S}
    ${CC} ${CFLAGS} ${LDFLAGS} -o dumpreg helpers/dumpreg.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o putreg helpers/putreg.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o parse-edid.o -c helpers/parse-edid.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o snoop helpers/snoop.c parse-edid.o
    ${CC} ${CFLAGS} ${LDFLAGS} -o setbox helpers/setbox.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o modeline helpers/modeline.c

    ${CC} ${CFLAGS} ${LDFLAGS} -c helpers/compute_ksv.c
    ${CC} ${CFLAGS} ${LDFLAGS} -o derive_km helpers/derive_km.c compute_ksv.o

    ${CC} ${CFLAGS} ${LDFLAGS} -o fpga_ctl helpers/fpga_ctl.c
}

do_install() {
	install -d ${D}/usr/bin
	install -m 0755 dumpreg ${D}/usr/bin
	install -m 0755 putreg ${D}/usr/bin
	install -m 0755 snoop ${D}/usr/bin
	install -m 0755 setbox ${D}/usr/bin
	install -m 0755 modeline ${D}/usr/bin
	install -m 0755 derive_km ${D}/usr/bin
	install -m 0755 fpga_ctl ${D}/usr/bin

	install -d ${D}/${base_libdir}/firmware
	install -m 0644 fpga/hdmi_overlay.bin ${D}/${base_libdir}/firmware/
}

FILES_${PN} = "/usr/bin"
FILES_${PN} += "${base_libdir}/firmware/"
PACKAGE_ARCH = "${MACHINE}"
