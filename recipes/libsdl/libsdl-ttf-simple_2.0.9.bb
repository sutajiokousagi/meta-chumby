DESCRIPTION = "Simple DirectMedia Layer truetype font library."
SECTION = "libs"
PRIORITY = "optional"
DEPENDS = "virtual/libsdl freetype"
RDEPENDS_${PN} = "freetype"
LICENSE = "LGPL"
PR = "r0"

SRC_URI = "http://www.libsdl.org/projects/SDL_ttf/release/SDL_ttf-${PV}.tar.gz"

S = "${WORKDIR}/SDL_ttf-${PV}"
EXTRA_OECONF += "SDL_CONFIG=${STAGING_BINDIR_CROSS}/sdl-config "

TARGET_CC_ARCH += "${LDFLAGS}"

do_compile() { 
   ${CC} -c SDL_ttf.c -o SDL_ttf.o \
       `${STAGING_BINDIR_CROSS}/sdl-config --cflags` \
       `pkg-config freetype2 --cflags` \
       ${CFLAGS} ${LDFLAGS} ${LIBS} -lz \
       -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" \
       -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE=\"SDL_ttf\" \
       -DVERSION=\"2.0.9\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 \
       -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 \
       -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 \
       -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 \
       -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1
    ${AR} crv libSDL-ttf-simple.a SDL_ttf.o
    ${RANLIB} libSDL-ttf-simple.a
}

do_install() {
    install -d ${D}/usr/lib
    install -d ${D}/usr/include
    install -m 0755 libSDL-ttf-simple.a ${D}/usr/lib
    install -m 0755 SDL_ttf.h ${D}/usr/include
}

SRC_URI[md5sum] = "6dd5a85e4924689a35a5fb1cb3336156"
SRC_URI[sha256sum] = "b4248876798b43d0fae1931cf8ae249f4f67a87736f97183f035f34aab554653"
