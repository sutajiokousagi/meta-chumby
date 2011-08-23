#!/bin/sh
# This script can be driven by buildbot to drive openembedded builds.
# If you're doing development from the command line, source "setup-chumby.sh"
# instead of running this script.
#
# This script expects the following environment variables to be set:
#            DISTRO = "angstrom-2010.x"
#            builder = name-of-the-build-machine
#            buildnumber = some-number
#            DISTRO_TYPE = "debug" or "release"
#            BB_ENV_EXTRAWHITE = 'FEED_DEPLOYDIR_BASE_URI DISTRO_TYPE PARALLEL_MAKE BB_NUMBER_THREADS MACHINE DISTRO TOPDIR buildnumber builder'
#            FEED_DEPLOYDIR_BASE_URI = publicly-accessible URL

export PATH="${PWD}/bitbake-1.12.0/bin:$PATH"
export BBPATH="${PWD}/meta-chumby-private:${PWD}/meta-chumby:${PWD}/openembedded"
export TOPDIR="${PWD}"
umask 000
nice ionice -c3 bitbake angstrom-version $*
retval=$?
exit $retval
