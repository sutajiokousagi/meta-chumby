# Package Revision number.  Increase this to force the package to
# rebuild.  Do this every time you make a change, and want to force
# the package to update on your device.
PR = "r1"

# Differently-licensed packages have their source code placed into
# different directories.  This is important for compliance when making
# commercial releases.
LICENSE = "BSD"


# Inherit the update-rc.d.bbclass file located in openembedded/classes/.
# This will take care of setting up startup links when the package is
# installed.
inherit update-rc.d


# Tell the update-rc.d package which program will be used as the startup
# script.
# The script will be called with the "start" command at system
# startup, the "stop" command at system shutdown, and the "restart" command
# when the package is updated.
INITSCRIPT_NAME = "init-example-script"

# Run the command at step 90 during startup, and step 10 during shutdown.
# Because the numbers go from 00 - 99, larger numbers will be run later on.
# If your program is required for system startup, put a low number here.
# If it's user-facing or less critical for system startup, put a higher
# number here.
# As a tradition, the shutdown number should be 100-startup_number.  That
# way scripts are stopped in the reverse order they were started in.  Since
# our startup number here is 90, the shutdown number will be 100-90 or 10.
INITSCRIPT_PARAMS = "defaults 90 10"


# This is where source files are brought in from.
SRC_URI = "file://init-example-1.c \
           file://init-example-2.c \
           file://init-example.h \
           file://init-example-script \
"

# Point the "S"ource directory at the working directory.
S = "${WORKDIR}"


# This defines the "compile" step.  
# "bitbake hello-world -c compile".  This is a standard shell script.  Any
# errors that are encountered here will cause the step to fail.  We compile
# two object files here as an example of how to link two source files.
do_compile() {
    ${CC} -c init-example-1.c -o init-example-1.o ${CFLAGS}
    ${CC} -c init-example-2.c -o init-example-2.o ${CFLAGS}
    ${CC} init-example-1.o init-example-2.o -o init-example ${LDFLAGS}
}


# This defines the "install" step.  Files you install here are candidates
# to be packaged up.  The destination path should always begin with ${D},
# otherwise bitbake will try to install files to your development system,
# which will fail.
do_install() {

    # Install binaries to /usr/bin
    install -d ${D}/usr/bin
    install -m 0755 init-example ${D}/usr/bin

    # Install startup script
    install -d ${D}/etc/init.d
    install -m 0755 init-example-script ${D}/etc/init.d
}

# Add the startup script to the list of files to be packaged.  Any files
# that are installed but not packaged will cause a warning to be printed
# during the bitbake process.
FILES_${PN} += "/etc/init.d/*"
