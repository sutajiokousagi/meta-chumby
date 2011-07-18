include compat-wireless.inc

PR .= "-amend-r1"

do_configure() {
	cd ${S}
        ./scripts/driver-select ath9k_htc
}
