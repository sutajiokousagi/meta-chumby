include compat-wireless.inc

SRC += "file://add-phy-virt.patch"

do_configure() {
	cd ${S}
        ./scripts/driver-select ath9k_htc
}
