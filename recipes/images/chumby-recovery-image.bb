# Install basic files only
IMAGE_INSTALL = "base-files base-passwd mtd-utils kobs-ng"
IMAGE_LINGUAS = ""

# Use busybox as login manager
IMAGE_LOGIN_MANAGER = "busybox"

# Include minimum init and init scripts
IMAGE_DEV_MANAGER = "udev"
IMAGE_INIT_MANAGER = "sysvinit sysvinit-pidof"
IMAGE_INITSCRIPTS = ""

inherit image
