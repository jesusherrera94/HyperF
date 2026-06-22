# Project Name
TARGET = hyper_f

# Uncomment to use LGPL (like ReverbSc, etc.)
USE_DAISYSP_LGPL=1

# Library Locations
HOTHOUSE_DIR ?= ../../HothouseExamples
LIBDAISY_DIR ?= $(HOTHOUSE_DIR)/libdaisy
DAISYSP_DIR ?= $(HOTHOUSE_DIR)/DaisySP

# Sources and Hothouse header files
CPP_SOURCES = $(TARGET).cpp HotHouseAdapter.cpp FuzzProcessor.cpp dspPrimitives.cpp $(HOTHOUSE_DIR)/src/hothouse.cpp
C_INCLUDES = -I$(HOTHOUSE_DIR)/src/


# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

# Global includes
include $(HOTHOUSE_DIR)/src/Makefile
