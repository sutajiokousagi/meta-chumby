SRC_URI += "file://fix-alignment-errors.patch file://fix-font-sigbus.patch"
PR .= "-amend-r3"
QT_CONFIG_FLAGS += " -qt-gfx-vnc -qt-gfx-transformed"
