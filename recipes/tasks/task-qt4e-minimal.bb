DESCRIPTION = "QT4 Embedded minimal Image Feed"
RREPLACES_${PN} = "task-qt4e-base"
PR = "r1"
LICENSE = "MIT"

inherit task

RDEPENDS_${PN} = " \
	libqt-embeddedclucene4 \
	libqt-embeddedcore4 \
	libqt-embeddeddbus4 \
	libqt-embeddedgui4 \
	libqt-embeddedhelp4 \
	libqt-embeddedmultimedia4 \
	libqt-embeddednetwork4 \
	libqt-embeddedscript4 \
	libqt-embeddedscripttools4 \
	libqt-embeddedsql4 \
	libqt-embeddedsvg4 \
	libqt-embeddedwebkit4 \
	libqt-embeddedxml4 \
	qt4-embedded-fonts-ttf-dejavu \
	qt4-embedded-fonts-ttf-vera \
	qt4-embedded-plugin-iconengine-svgicon \
	qt4-embedded-plugin-imageformat-gif \
	qt4-embedded-plugin-imageformat-ico \
	qt4-embedded-plugin-imageformat-jpeg \
	qt4-embedded-plugin-imageformat-mng \
	qt4-embedded-plugin-imageformat-svg \
	qt4-embedded-plugin-imageformat-tiff \
	qt4-embedded-plugin-mousedriver-tslib \
	qt4-embedded-plugin-phonon-backend-gstreamer \
	qt4-embedded-plugin-script-dbus \
	qt4-embedded-plugin-sqldriver-sqlite2 \
	qt4-embedded-plugin-sqldriver-sqlite \
"
