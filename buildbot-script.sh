#!/bin/sh
export PATH="${PWD}/bitbake-1.12.0/bin:$PATH"
export BBPATH="${PWD}/meta-chumby-private:${PWD}/meta-chumby:${PWD}/openembedded"
export TOPDIR="${PWD}"
umask 000
nice ionice -c3 bitbake angstrom-version $*
retval=$?
exit $retval
