# The recovery image is rather strange in how it's deployed:
#  1) Copy defconfig from files/ to a copy of the kernel that's checked out locally
#  2) Enter devshell
#  3) Change to the local directory
#  4) Begin compilation.  Wait for failure.
#  5) Exit devshell
#  6) Run the do_compile step of compat-wireless-ath9k-htc
#  7) Enter the compat-wireless-ath9k-htc directory
#  8) Modify the run.do_compile script and point it at the path in step (1)
#  9) Run the modified do_compile script
# 10) Run "for i in $(find . -name '*.ko'); do cp $i ~/chumby-oe/meta-chumby/recipes/recovery/files/; done"
# 11) Run "bitbake chumby-recovery-image"
# 12) Copy the resulting .cpio into the directory defined in step (1)
# 13) Change to the directory in step (1) and run the do_compile kernel script
# 14) Copy arch/arm/boot/zImage to recovery files directory
inherit recovery-image
inherit chumbysg-git

COMPAT_WIRELESS_VERSION = "2011-04-17"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL};name=netv-recovery \
           ${CHUMBYSG_GIT_HOST}/chumby-sg/linux-2.6.28-silvermoon${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL};branch=netv;name=kernel \
           http://wireless.kernel.org/download/compat-wireless-2.6/compat-wireless-${COMPAT_WIRELESS_VERSION}.tar.bz2 \
           file://htc_9271.fw \
           file://defconfig \
"
S = "${WORKDIR}"
SRCREV = "${AUTOREV}"
PACKAGE_ARCH = "${MACHINE}"


COMPATIBLE_MACHINE = "chumby-silvermoon-netv"
ONLINE_PACKAGE_MANAGEMENT = "none"
IMAGE_FSTYPES = "cpio"
IMAGE_DEV_MANAGER = ""
IMAGE_LINGUAS = ""
IMAGE_INSTALL = "wpa-supplicant-simple freetype"

MACHINE_POSTPROCESS_COMMAND = ""


ROOTFS_POSTPROCESS_COMMAND += "populate_netv_recovery; "


DEPENDS = "libsdl-chumby-simple libsdl-ttf-simple"
RDEPENDS_${PN} = "libsdl-ttf-simple freetype wpa-supplicant-simple"

do_compile_kernel_pass1() {
	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS MACHINE
	export CROSS_COMPILE="${TARGET_PREFIX}"
	cp defconfig src/.config
	cd src; oe_runmake ARCH=arm prepare scripts; cd ..
}

do_compile_compat_wireless() {
	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
	export CROSS_COMPILE="${TARGET_PREFIX}"
	cd compat-wireless-${COMPAT_WIRELESS_VERSION}
	./scripts/driver-select ath9k_htc
	oe_runmake ARCH=arm KLIB=${WORKDIR}/src KLIB_BUILD=${WORKDIR}/src
	cd ..
}

do_compile_kernel_pass2() {
	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS MACHINE
	cp "${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.${IMAGE_FSTYPES}" ./src/recovery.cpio
	export CROSS_COMPILE="${TARGET_PREFIX}"
	cd src; oe_runmake ARCH=arm; cd ..
	cp src/arch/arm/boot/zImage ${DEPLOY_DIR_IMAGE}/recovery-mode
}


do_compile() {
	cd git
	LDFLAGS="${LDFLAGS}"
	oe_runmake MY_LIBS="-lm -lSDL-ttf-simple -lSDL-chumby-simple -lfreetype -lz -lpthread"
}

populate_netv_recovery() {
	install -d ${IMAGE_ROOTFS}
	install -d ${IMAGE_ROOTFS}/modules
	install -d ${IMAGE_ROOTFS}/firmware

	install -m 0755 ${WORKDIR}/git/netv-recovery ${IMAGE_ROOTFS}/init
	install -m 0755 ${WORKDIR}/git/AMD.ttf ${IMAGE_ROOTFS}

	for i in $(find ${WORKDIR}/compat-wireless-${COMPAT_WIRELESS_VERSION}/ -name '*.ko'); do cp $i ${IMAGE_ROOTFS}/modules; done

	install -m 0644 ${WORKDIR}/htc_9271.fw ${IMAGE_ROOTFS}/firmware
}

do_install() {
	install -d ${D}/boot
	install -m 0755 ${WORKDIR}/src/arch/arm/boot/zImage ${D}/boot/recovery-mode
}

def remove_tasks(deltasks, d):
    for task in filter(lambda k: d.getVarFlag(k, "task"), d.keys()):
        deps = d.getVarFlag(task, "deps")
        for preptask in deltasks:
            if preptask in deps:
                deps.remove(preptask)
        d.setVarFlag(task, "deps", deps)
python () {
    remove_tasks(["do_populate_sysroot", "do_package_update_index_ipk"], d)
}

addtask compile_kernel_pass1 after do_unpack before do_compile
addtask compile_compat_wireless after do_compile_kernel_pass1 before do_rootfs

addtask compile_kernel_pass2 after do_rootfs do_compile_compat_wireless before do_install

FILES_${PN} += "/init /AMD.ttf /modules/* /firmware/*"

SRC_URI[md5sum] = "96d047a7cef1f0541e741290f64b466c"
SRC_URI[sha256sum] = "633e302019c328e0f8fba8d69927e533dd4f70d41afbe007c506f709d2bd6059"
