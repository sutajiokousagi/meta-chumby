PR = "r1"

export IMAGE_BASENAME = "xfce46-image"

XSERVER ?= "xserver-xorg \
           xf86-input-evdev \
           xf86-input-mouse \
           xf86-video-fbdev \
           xf86-input-keyboard \
"

DEPENDS = "virtual/xserver ${MACHINE_TASK_PROVIDER} task-xfce46-base task-xfce46-extras  \
    claws-mail \
    pidgin \
    xmms \
    cpid monitor-reset-button \
    lsof file iotop sysstat chumby-otg-debug \
    networkmanager \
    gnome-keyring \
    gimp \
    network-manager-applet \
    xdm \
    squeeze \
    gigolo \
    laptop-support \
    bash \
    task-sdk-native \
"

IMAGE_INSTALL = "${XSERVER} ${MACHINE_TASK_PROVIDER} task-xfce46-base task-xfce46-extras \
    claws-mail \
    pidgin \
    xmms \
    cpid monitor-reset-button\
    lsof file iotop sysstat chumby-otg-debug\
    kernel-module-cfg80211 kernel-module-chumby-xilinx kernel-module-configfs \
    kernel-module-g-cdc kernel-module-g-ether kernel-module-g-file-storage kernel-module-g-serial kernel-module-g-zero \
    kernel-module-mac80211 \
    kernel-module-scsi-wait-scan \
    kernel-module-snd-hwdep kernel-module-snd-rawmidi \
    kernel-module-snd-usb-audio kernel-module-snd-usb-caiaq kernel-module-snd-usb-lib \
    kernel-module-unionfs \
    networkmanager \
    gnome-keyring \
    gimp \
    network-manager-applet \
    xfwm4-themes \
    xmms-plugin-effect-echo \
    xmms-plugin-effect-stereo \
    xmms-plugin-effect-voice \
    xmms-plugin-general-song-change \
    xmms-plugin-input-cdaudio \
    xmms-plugin-input-mikmod \
    xmms-plugin-input-mpg123 \
    xmms-plugin-input-tonegen \
    xmms-plugin-input-vorbis \
    xmms-plugin-input-wav \
    xmms-plugin-output-disk-writer \
    squeeze \
    gigolo \
    laptop-support \
    bash bash-sh \
    task-sdk-native \
"

inherit image
IMAGE_ROOTFS_SIZE = "1500000"
