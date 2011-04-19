inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby stub bootloader - loads chumby bootloader"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Sean Cross"
LICENSE = "BSD"
PR = "r0"

COMPATIBLE_MACHINE = "chumby-falconwing"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-private/https_internal.chumby.com_firmware_bootstream-1.0_${RVERSION};subpath=src;protocol=${CHUMBY_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src/chumby_stub"

# Force do_compile() to use standard cpp.  For some reason, ARM cpp doesn't work.
do_compile() {
     make CC="${CC}" AS="${AS}" CPP="cpp" LD="${LD}" \
        OBJCOPY="${OBJCOPY}" OBJDUMP="${OBJDUMP}" STRIP="${STRIP}" \
        CHUMBY_STUB=chumby_stub CHUMBY_STUB_ROM=chumby_stub.rom \
        CFLAGS="-Wall -nostdinc -fno-builtin -Os -Iinclude -DCHUMBY_USB_FIX -DCHUMBY_SHELL" \
        LDFLAGS="-static -nostdlib -T output/chumby_stub.ld -L$(dirname $(${CC} -print-libgcc-file-name)) -lgcc"
}

# This generates a chumby_stub bootstream blob

# chumby_stub is an ELF, with no gnu_hash.
INSANE_SKIP_${PN} = True

FILES_${PN} = "/boot"

do_install() {
    install -d ${D}/boot
    install -m 0755 ${S}/chumby_stub ${D}/boot/chumby_stub
}

# Copy the resulting file to the image directory
do_deploy() {
    install -d ${DEPLOY_DIR_IMAGE}
    install ${S}/chumby_stub ${DEPLOY_DIR_IMAGE}/chumby_stub
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/chumby_stub
}
do_deploy[dirs] = "${S}"
addtask deploy before do_package_stage after do_compile
