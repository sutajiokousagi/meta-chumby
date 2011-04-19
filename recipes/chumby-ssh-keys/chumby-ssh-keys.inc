inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby SSH keys"
SECTION = "base"
PRIORITY = "optional"
LICENSE = "GPL"
PR = "r0"

COMPATIBLE_MACHINE = "chumby-${RPLATFORM}"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-2.0_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-${CNPLATFORM}_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src/rfs1/etc"

do_install () {
#
# Create directories and install device independent scripts
#
    install -d ${D}/etc/ssh
#    install -m 0644    ${WORKDIR}/src/rfs1/etc/ssh_config ${D}${sysconfdir}/ssh
#    install -m 0644    ${WORKDIR}/src/rfs1/etc/sshd_config ${D}${sysconfdir}/ssh
    install -m 0600    ${WORKDIR}/src/rfs1/etc/ssh_host_dsa_key ${D}${sysconfdir}/ssh
    install -m 0600    ${WORKDIR}/src/rfs1/etc/ssh_host_dsa_key.pub ${D}${sysconfdir}/ssh
    install -m 0600    ${WORKDIR}/src/rfs1/etc/ssh_host_key ${D}${sysconfdir}/ssh
    install -m 0600    ${WORKDIR}/src/rfs1/etc/ssh_host_key.pub ${D}${sysconfdir}/ssh
    install -m 0600    ${WORKDIR}/src/rfs1/etc/ssh_host_rsa_key.pub ${D}${sysconfdir}/ssh
    install -m 0600    ${WORKDIR}/src/rfs1/etc/ssh_host_rsa_key ${D}${sysconfdir}/ssh
}

FILES_${PN} = "${sysconfdir}/ssh"
