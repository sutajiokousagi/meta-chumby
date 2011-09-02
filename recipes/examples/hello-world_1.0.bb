# Package Revision number.  Increase this to force the package to
# rebuild.  Do this every time you make a change, and want to force
# the package to update on your device.
PR = "r1"

# This is where source files are brought in from.
# You can have multiple URIs here.  They can be space-delimited.
SRC_URI = "file://hello-world.c"

# Point the "S"ource directory at the working directory.  During the
# do_unpack stage -- which is inherited from base.bbclass -- all source
# code defined in SRC_URI is uncompressed to ${WORKDIR}.  Because the
# source code for this recipe has no directory structure, we'll just point
# it to the working directory.
# For projects that are pulled from git, this would usually be
# "${WORKDIR}/git".
S = "${WORKDIR}"


# This defines the "compile" step.  To run just the "compile" step, run
# "bitbake hello-world -c compile".  This is a standard shell script.  Any
# errors that are encountered here will cause the step to fail.
do_compile() {
    ${CC} hello-world.c -o hello-world ${CFLAGS} ${LDFLAGS}
}


# This defines the "install" step.  Files you install here are candidates
# to be packaged up.  The destination path should always begin with ${D},
# otherwise bitbake will try to install files to your development system,
# which will fail.
do_install() {
    install -d ${D}/usr/bin
    install -m 0755 hello-world ${D}/usr/bin
}

# Anything that gets installed to /usr/bin, /usr/lib, /usr/sbin, or a
# handful of other paths will automatically get packaged.  Files going to
# unusual locations, such as /lib/firmware, will not automatically get
# packaged.  To add files and directories to get packaged, add them to this
# variable and uncomment it.
# FILES_${PN} += "/usr/weird"
