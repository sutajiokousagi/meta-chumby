include compat-wireless.inc

do_configure() {
	cd ${S}
        ./scripts/driver-select ath9k_htc
}
