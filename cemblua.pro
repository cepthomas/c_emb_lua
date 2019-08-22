
TEMPLATE = app
CONFIG += console strict_c
CONFIG -= app_bundle qt c11 gui
# Keep it honest - treat warnings as errors.
QMAKE_CFLAGS += -Werror


SRC_PATH = "./source"
LUA_PATH = "./lua-5.3.5/src"

# Copy lua scripts.
copydata.commands = $(COPY_DIR) $$PWD/source/*.lua $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata


INCLUDEPATH += \
    $$SRC_PATH \
    $$LUA_PATH


### Main app ###
SOURCES += \
    $$SRC_PATH/main.c \
    $$SRC_PATH/common.c \
    $$SRC_PATH/exec.c \
    $$SRC_PATH/board.c \
    $$SRC_PATH/luainterop.c \
    $$SRC_PATH/c2lua.c \
    $$SRC_PATH/lua2c.c

HEADERS += \
    $$SRC_PATH/common.h \
    $$SRC_PATH/exec.h \
    $$SRC_PATH/board.h \
    $$SRC_PATH/luainterop.h \
    $$SRC_PATH/c2lua.h \
    $$SRC_PATH/lua2c.h


### Lua ###
# Could be a lib if so inclined.
SOURCES += \
    $$LUA_PATH/lapi.c \
    $$LUA_PATH/lauxlib.c \
    $$LUA_PATH/lbaselib.c \
    $$LUA_PATH/lbitlib.c \
    $$LUA_PATH/lcode.c \
    $$LUA_PATH/lcorolib.c \
    $$LUA_PATH/lctype.c \
    $$LUA_PATH/ldblib.c \
    $$LUA_PATH/ldebug.c \
    $$LUA_PATH/ldo.c \
    $$LUA_PATH/ldump.c \
    $$LUA_PATH/lfunc.c \
    $$LUA_PATH/lgc.c \
    $$LUA_PATH/linit.c \
    $$LUA_PATH/liolib.c \
    $$LUA_PATH/llex.c \
    $$LUA_PATH/lmathlib.c \
    $$LUA_PATH/lmem.c \
    $$LUA_PATH/loadlib.c \
    $$LUA_PATH/lobject.c \
    $$LUA_PATH/lopcodes.c \
    $$LUA_PATH/loslib.c \
    $$LUA_PATH/lparser.c \
    $$LUA_PATH/lstate.c \
    $$LUA_PATH/lstring.c \
    $$LUA_PATH/lstrlib.c \
    $$LUA_PATH/ltable.c \
    $$LUA_PATH/ltablib.c \
    $$LUA_PATH/ltm.c \
#    $$LUA_PATH/lua.c \
#    $$LUA_PATH/luac.c \
    $$LUA_PATH/lundump.c \
    $$LUA_PATH/lutf8lib.c \
    $$LUA_PATH/lvm.c \
    $$LUA_PATH/lzio.c

HEADERS += \
    $$LUA_PATH/lapi.h \
    $$LUA_PATH/lauxlib.h \
    $$LUA_PATH/lcode.h \
    $$LUA_PATH/lctype.h \
    $$LUA_PATH/ldebug.h \
    $$LUA_PATH/ldo.h \
    $$LUA_PATH/lfunc.h \
    $$LUA_PATH/lgc.h \
    $$LUA_PATH/llex.h \
    $$LUA_PATH/llimits.h \
    $$LUA_PATH/lmem.h \
    $$LUA_PATH/lobject.h \
    $$LUA_PATH/lopcodes.h \
    $$LUA_PATH/lparser.h \
    $$LUA_PATH/lprefix.h \
    $$LUA_PATH/lstate.h \
    $$LUA_PATH/lstring.h \
    $$LUA_PATH/ltable.h \
    $$LUA_PATH/ltm.h \
    $$LUA_PATH/lua.h \
    $$LUA_PATH/luaconf.h \
    $$LUA_PATH/lualib.h \
    $$LUA_PATH/lundump.h \
    $$LUA_PATH/lvm.h \
    $$LUA_PATH/lzio.h
