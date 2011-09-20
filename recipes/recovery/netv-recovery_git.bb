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
inherit chumbysg-git
SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL} \
           file://ath9k_common.ko \
           file://ath9k_htc.ko \
           file://ath9k_hw.ko \
           file://ath.ko \
           file://cfg80211.ko \
           file://compat.ko \
           file://compat_firmware_class.ko \
           file://mac80211.ko \
           file://rfkill_backport.ko \
           file://htc_9271.fw \
"
S = "${WORKDIR}/git"
SRCREV = "${AUTOREV}"
PREFERRED_PROVIDER_virtual/libsdl = "libsdl-chumby-simple"

DEPENDS = "libsdl-ttf-simple libsdl-chumby-simple"
RDEPENDS_${PN} = "libsdl-ttf-simple wpa-supplicant-simple libsdl-chumby-simple"

do_install() {
	install -d ${D}
	install -d ${D}/modules
	install -d ${D}/firmware

	install -m 0755 netv-recovery ${D}/init
	install -m 0755 AMD.ttf ${D}
	install -m 0644 ${WORKDIR}/ath9k_common.ko ${D}/modules
	install -m 0644 ${WORKDIR}/ath9k_htc.ko ${D}/modules
	install -m 0644 ${WORKDIR}/ath9k_hw.ko ${D}/modules
	install -m 0644 ${WORKDIR}/ath.ko ${D}/modules
	install -m 0644 ${WORKDIR}/cfg80211.ko ${D}/modules
	install -m 0644 ${WORKDIR}/compat.ko ${D}/modules
	install -m 0644 ${WORKDIR}/compat_firmware_class.ko ${D}/modules
	install -m 0644 ${WORKDIR}/mac80211.ko ${D}/modules
	install -m 0644 ${WORKDIR}/rfkill_backport.ko ${D}/modules

	install -m 0644 ${WORKDIR}/htc_9271.fw ${D}/firmware
}

FILES_${PN} += "/init /AMD.ttf /modules/* /firmware/*"
