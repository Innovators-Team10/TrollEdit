# Determine install host
IF ( WIN32 AND NOT CYGWIN)
	ADD_DEFINITIONS ( -DLUA_BUILD_AS_DLL )
ELSE ( )
	ADD_DEFINITIONS ( -DLUA_USE_POSIX -DLUA_USE_DLOPEN )
	SET ( LIBS m dl )
ENDIF ( )

# Build Libraries
SET(SRC_COCO src/lcoco.c)
SET(SRC_LIBJIT src/ljit_core.c src/ljit_mem.c src/ljit_dasm.c src/ljit_backend.c)
SET(SRC_JITCORE src/lapi.c src/lcode.c src/ldebug.c src/ldo.c src/ldump.c src/lfunc.c src/lgc.c src/llex.c src/lmem.c src/lobject.c src/lopcodes.c src/lparser.c src/lstate.c src/lstring.c src/ltable.c src/ltm.c src/lundump.c src/lvm.c src/lzio.c ${SRC_COCO} ${SRC_LIBJIT})
SET(SRC_LIBLUAJIT src/lauxlib.c src/lbaselib.c src/ldblib.c src/liolib.c src/lmathlib.c src/loslib.c src/ltablib.c src/lstrlib.c src/loadlib.c src/linit.c src/ljitlib.c)

INCLUDE_DIRECTORIES (dynasm src)
ADD_LIBRARY ( lua ${SRC_JITCORE} ${SRC_LIBLUAJIT})
TARGET_LINK_LIBRARIES ( lua ${LIBS})