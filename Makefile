# Project Name
TARGET = is31fl3731_demo

# Sources
CPP_SOURCES = examples/basic_demo.cpp \
               lib/is31fl3731/is31fl3731.cpp \
               lib/is31fl3731_graphics/IS31FL3731_Graphics.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Add include paths
CPPFLAGS += -I.

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
