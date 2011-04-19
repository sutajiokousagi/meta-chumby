inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby basic init scripts"
SECTION = "base"
PRIORITY = "required"
LICENSE = "GPL"
PR = "r0"

COMPATIBLE_MACHINE = "chumby-${FNPLATFORM}"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-2.0_${FNPLATFORMVER}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-${FNPLATFORM}_${FNPLATFORMVER}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           file://0001-force-mount-all.patch \
           file://0002-set-regioncode.patch \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src/rfs1/etc"

do_compile () {
    echo '#!/usr/bin/perl'                                             > hwconfig.pl
    echo '# $Id: hwconfig.pl 8544 2009-05-11 19:09:17Z henry $'       >> hwconfig.pl
    echo '# Perl globals for hardware config'                         >> hwconfig.pl
    echo '#'                                                          >> hwconfig.pl
    echo '# Usage:'                                                   >> hwconfig.pl
    echo '# require "/etc/hwconfig.pl";'                              >> hwconfig.pl
    echo ''                                                           >> hwconfig.pl
    echo '$CNPLATFORM="'${CNPLATFORM}'";'                             >> hwconfig.pl
    echo '$HAS_CRYPTO=$ENV{"HAS_CP"};'                                >> hwconfig.pl
    echo '$USB_TESTDIR="/sys/devices/platform/pxa168-ehci/usb1/1-1";' >> hwconfig.pl
    echo '$NETWORKIF="wlan0";'                                        >> hwconfig.pl
    echo ''                                                           >> hwconfig.pl
    echo 'if ( -f "/psp/hwconfig.pl")'                                >> hwconfig.pl
    echo '{'                                                          >> hwconfig.pl
    echo '	require "/psp/hwconfig.pl";'                          >> hwconfig.pl
    echo '}'                                                          >> hwconfig.pl
    echo ''                                                           >> hwconfig.pl
    echo 'return 1;'                                                  >> hwconfig.pl
    echo ''                                                           >> hwconfig.pl

    echo '#!/bin/sh'                                                   > hwconfig
    echo '# $Id: hwconfig 8544 2009-05-11 19:09:17Z henry $'          >> hwconfig
    echo '# platform-specific hardware config'                        >> hwconfig
    echo 'CNPLATFORM="'${CNPLATFORM}'"'                               >> hwconfig
    echo 'HAS_CRYPTO=${HAS_CP:-0}'                                    >> hwconfig
    echo 'NETWORKIF=wlan0'                                            >> hwconfig
}

do_install () {
#
# Create directories and install device independent scripts
#
    install -d ${D}/proc
    install -d ${D}/sys
    install -d ${D}/mnt
    install -d ${D}/var
    install -d ${D}/tmp
    install -d ${D}/usr/chumby/scripts
    install -d ${D}/root

    install -d ${D}${sysconfdir}/init.d

    install -m 0755    ${WORKDIR}/src/rfs1/etc/init.d/rcS               ${D}${sysconfdir}/init.d
    install -m 0755    ${WORKDIR}/src/rfs1/etc/init.d/rcS.background    ${D}${sysconfdir}/init.d
    install -m 0644    ${WORKDIR}/src/rfs1/etc/inittab                  ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/fstab                    ${D}${sysconfdir}
    cp -af             ${WORKDIR}/src/rfs1/etc/mtab                     ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/issue                    ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/motd                     ${D}${sysconfdir}

    install -m 0644    ${WORKDIR}/src/rfs1/etc/shells                   ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/bashrc                   ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/profile                  ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/csh.cshrc                ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/csh.login                ${D}${sysconfdir}

    install -m 0644    ${WORKDIR}/src/rfs1/etc/host.conf                ${D}${sysconfdir}
    cp -af             ${WORKDIR}/src/rfs1/etc/hostname                 ${D}${sysconfdir}
    cp -af             ${WORKDIR}/src/rfs1/etc/hosts                    ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/hosts.allow              ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/hosts.deny               ${D}${sysconfdir}

    install -m 0644    ${WORKDIR}/src/rfs1/etc/inputrc                  ${D}${sysconfdir}
    install -m 0755    ${WORKDIR}/src/rfs1/etc/ld.so.conf               ${D}${sysconfdir}
    cp -af             ${WORKDIR}/src/rfs1/etc/ld.so.preload            ${D}${sysconfdir}

    cp -af             ${WORKDIR}/src/rfs1/etc/timezone                 ${D}${sysconfdir}

    install -m 0644    ${WORKDIR}/src/rfs1/etc/passwd                   ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/shadow                   ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/group                    ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/nsswitch.conf            ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/pwdb.conf                ${D}${sysconfdir}

    install -m 0644    ${WORKDIR}/src/rfs1/etc/services                 ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/protocols                ${D}${sysconfdir}

    install -m 0644    ${WORKDIR}/src/rfs1/etc/DIR_COLORS               ${D}${sysconfdir}
    install -m 0644    ${WORKDIR}/src/rfs1/etc/DIR_COLORS.xterm         ${D}${sysconfdir}

    install -m 0644    ${WORKDIR}/src/rfs1/etc/sysctl.conf              ${D}${sysconfdir}

    install -m 0755    ${WORKDIR}/src/rfs1/usr/chumby/scripts/create_storage_partition.sh ${D}/usr/chumby/scripts

    # Add in custom video sizing
    echo "export VIDEO_X_RES=${MACHINE_DISPLAY_WIDTH_PIXELS}"                                >> ${D}${sysconfdir}/profile
    echo "export VIDEO_Y_RES=${MACHINE_DISPLAY_HEIGHT_PIXELS}"                               >> ${D}${sysconfdir}/profile
    echo "export VIDEO_RES=${MACHINE_DISPLAY_WIDTH_PIXELS}x${MACHINE_DISPLAY_HEIGHT_PIXELS}" >> ${D}${sysconfdir}/profile
    echo "export CNPLATFORM=${CNPLATFORM}"                                                   >> ${D}${sysconfdir}/profile
    echo "export HAS_CP=${MACHINE_HAS_CRYPTOPROCESSOR}"                                      >> ${D}${sysconfdir}/profile

    install -m 0755   hwconfig.pl ${D}${sysconfdir}
    install -m 0755   hwconfig    ${D}${sysconfdir}

    ln -sf /mnt/storage/psp ${D}/psp
    ln -sf /mnt             ${D}/media
}

FILES_${PN} = "/etc /proc /sys /tmp /psp /mnt /var /media /usr/chumby/scripts /root"
