require opkg.inc

PR = "${INC_PR}"
PROVIDES =+ "virtual/update-alternatives-native"

target_libdir := "${libdir}"

inherit native
DEPENDS = "gpgme-native"

EXTRA_OECONF += "--with-opkglibdir=${target_libdir}"
