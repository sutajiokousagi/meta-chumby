DESCRIPTION = "Chumby boot blobs - miscellaneous binary blobs"
HOMEPAGE = "http://www.chumby.com/"
LICENSE = "BSD"

PROVIDES = "chumby-blobs"
COMPATIBLE_MACHINE = "chumby-wintergrasp"
PR = "r0"

SRC_URI = "file://boot_prep \
           file://power_prep"

FILES_${PN} = "/boot"

do_compile() {
    true
}

do_install() {
    install -d ${D}/boot
    install -m 0755 ${WORKDIR}/boot_prep ${D}/boot
    install -m 0755 ${WORKDIR}/power_prep ${D}/boot
}

do_runstrip() {
    true
}

# Copy the resulting file to the image directory
do_deploy() {
    install -d ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/boot_prep ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/power_prep ${DEPLOY_DIR_IMAGE}

    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/boot_prep
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/power_prep
}

addtask deploy before do_package_stage after do_compile
