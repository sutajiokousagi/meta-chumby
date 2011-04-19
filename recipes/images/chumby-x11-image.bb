# Chumby X11 test image

inherit image chumby-info

IMAGE_PREPROCESS_COMMAND = "create_etc_timestamp"

XSERVER ?= "xserver-xorg \
           xf86-input-evdev \
           xf86-input-mouse \
           xf86-video-fbdev \
           xf86-input-keyboard \
"

SPLASH = "exquisite exquisite-themes exquisite-theme-angstrom"

export IMAGE_BASENAME = "Beagleboard-demo-image"

DEPENDS = "task-base"


IMAGE_INSTALL += "task-boot \
# stuff below needs a proper task
	    initscripts update-rc.d \
	    dropbear \
	    networkmanager cnetworkmanager \
	    xinetd \
            util-linux-ng-mount util-linux-ng-umount \
	    udev bash wpa-supplicant wireless-tools \
	    rt73-firmware \
	    tslib tslib-calibrate vim \
	    regutil-${CNPLATFORM} \
	    task-base kernel-modules \
	    task-x11 \
	    ${XSERVER} \
	    ${SPLASH} \
	    strace \
	    screen \
	    midori \
	   "

export IMAGE_BASENAME = "chumby-x11-image"
IMAGE_LINGUAS = ""


