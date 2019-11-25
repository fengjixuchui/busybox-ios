export THEOS=/opt/theos
export THEOS_DEVICE_IP=127.0.0.1
export THEOS_DEVICE_PORT=22
export SDKVERSION=9.3
TARGET=iphone:latest:9.3
ARCHS=armv7 arm64

include $(THEOS)/makefiles/common.mk

TOOL_NAME = busybox
${TOOL_NAME}_CFLAGS = -I. -Dsetbit -Wno-error 
${TOOL_NAME}_LDFLAGS = -lresolv
${TOOL_NAME}_FILES = main.c coreutils/libbb/libbb.c
${TOOL_NAME}_FILES += $(wildcard coreutils/*.c)
${TOOL_NAME}_CODESIGN_FLAGS = -S./${TOOL_NAME}.xml

include $(THEOS_MAKE_PATH)/tool.mk
