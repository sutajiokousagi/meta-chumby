# Chumby image build with flashplayer

inherit image chumby-info

IMAGE_PREPROCESS_COMMAND = "create_etc_timestamp"

DEPENDS = "task-base"
IMAGE_ROOTFS_SIZE_chumby-silvermoon-netv = "500000"

# Build these packages (if they're available) but don't install them.
RRECOMMENDS_${PN} += "keygen gdb vim emacs libpcap iw task-sdk-native"

CHUMBY_EXTRA_PACKAGES = ""
CHUMBY_EXTRA_PACKAGES_chumby-silvermoon-netv = "\
	cpid chumby-qt-remote monitor-reset-button\
	lsof file iotop sysstat chumby-otg-debug\
	kernel-module-cfg80211 kernel-module-chumby-xilinx kernel-module-configfs \
	kernel-module-g-cdc kernel-module-g-ether kernel-module-g-file-storage kernel-module-g-serial kernel-module-g-zero \
	kernel-module-mac80211 \
	kernel-module-scsi-wait-scan \
	kernel-module-snd-hwdep kernel-module-snd-rawmidi \
	kernel-module-snd-usb-audio kernel-module-snd-usb-caiaq kernel-module-snd-usb-lib \
	kernel-module-unionfs \
	task-chumby-netv \

#	Access point operation
	dnsmasq \
	hostap-daemon \

#	Qt monster libraries
	jpeg \
	qt4-embedded-plugin-imageformat-gif \
	qt4-embedded-plugin-imageformat-ico \
	qt4-embedded-plugin-imageformat-jpeg \
"
CHUMBY_EXTRA_PACKAGES_chumby-falconwing = "cpid kernel-modules"
CHUMBY_EXTRA_PACKAGES_chumby-wintergrasp = "cpid kernel-modules"

#
# dropbear, openssh or none
#
DISTRO_SSH_DAEMON ?= "openssh"


IMAGE_INSTALL += "task-boot \
	task-chumby-support \
# stuff below needs a proper task
	initscripts update-rc.d \
	tzdata tzdata-misc tzdata-asia tzdata-americas tzdata-europe tzdata-australia tzdata-posix \
	${DISTRO_SSH_DAEMON} \
	xinetd \
        util-linux-ng-mount util-linux-ng-umount \
	udev bash bash-sh wpa-supplicant wireless-tools \
	alsa-utils-aplay alsa-utils-amixer \
	task-base \
	strace \
	cron \
	stat \
	${CHUMBY_EXTRA_PACKAGES} \
"

IMAGE_LINGUAS = ""
