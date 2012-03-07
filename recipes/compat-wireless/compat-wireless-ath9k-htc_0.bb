include compat-wireless.inc

PR .= "-amend-r3"

#SRC_URI += "file://0001-disable-leds-by-default.patch \
#            file://0002-light-off-by-default.patch \
#"

do_configure() {
	cd ${S}
        ./scripts/driver-select ath9k_htc
}
