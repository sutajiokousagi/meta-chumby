# We remove gtk-icon-cache, as we don't have GTK installed
inherit gnomebase gconf mime

EXTRA_OECONF += "--enable-introspection=no --disable-scrollkeeper"

