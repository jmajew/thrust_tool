##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

# Compiler options here. nosys, nano, rdimon
ifeq ($(USE_OPT),)
#  USE_OPT = -O2 --specs=nosys.specs -fomit-frame-pointer -falign-functions=16
  USE_OPT = -O0 -ggdb -specs=nano.specs -specs=nosys.specs -falign-functions=16
##  USE_OPT = -O2 -ggdb --specs=nosys.specs -fomit-frame-pointer -falign-functions=16
##  USE_OPT = -O0 -ggdb --specs=nosys.specs 
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT = 
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = -std=gnu++17 -fno-exceptions -fno-rtti -fno-use-cxa-atexit
endif

# Enable this if you want the linker to remove unused code and data.
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# Linker extra options here.
ifeq ($(USE_LDOPT),)
  USE_LDOPT =
endif

# Enable this if you want link time optimizations (LTO).
ifeq ($(USE_LTO),)
  USE_LTO = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
ifeq ($(USE_SMART_BUILD),)
  USE_SMART_BUILD = yes
endif

#
# Build global options
##############################################################################

##############################################################################
# Architecture or project specific options
#

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
ifeq ($(USE_PROCESS_STACKSIZE),)
  USE_PROCESS_STACKSIZE = 0x400
endif

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  USE_EXCEPTIONS_STACKSIZE = 0x400
endif

# Enables the use of FPU (no, softfp, hard).
ifeq ($(USE_FPU),)
  USE_FPU = hard
endif

# FPU-related options.
ifeq ($(USE_FPU_OPT),)
#  USE_FPU_OPT = -mfloat-abi=$(USE_FPU) -mfpu=fpv4-sp-d16 -fsingle-precision-constant
  USE_FPU_OPT = -mfloat-abi=$(USE_FPU) -mfpu=fpv4-sp-d16
endif

#
# Architecture or project specific options
##############################################################################

##############################################################################
# Project, target, sources and paths
#

# Define project name here
PROJECT = thrust++

# Target settings.
MCU  = cortex-m4

# Imported source files and paths.
CHIBIOS  := ../../../ChibiOS.git
#CHIBIOS  := ../../ChibiOS_18.2.1

SRCDIR   := ./src
AUTOBUILD_ROOT   := $(SRCDIR)
CONFDIR  := $(SRCDIR)/cfg
BUILDDIR := ./build
DEPDIR   := ./.dep

# Licensing files.
include $(CHIBIOS)/os/license/license.mk
# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f4xx.mk

# HAL-OSAL files (optional).
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F4xx/platform.mk

include target/QQ_THRUST_V2/board_cur.mk
#include $(CHIBIOS)/os/hal/boards/ST_NUCLEO64_F401RE/board.mk

include $(CHIBIOS)/os/hal/osal/rt-nil/osal.mk

# RTOS files (optional).
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMv7-M/compilers/GCC/mk/port.mk

# Auto-build files in ./source recursively.
include $(CHIBIOS)/tools/mk/autobuild.mk

# Other files (optional).
include $(CHIBIOS)/os/test/test.mk
include $(CHIBIOS)/test/rt/rt_test.mk
include $(CHIBIOS)/test/oslib/oslib_test.mk
include $(CHIBIOS)/os/hal/lib/streams/streams.mk
include $(CHIBIOS)/os/various/shell/shell.mk

include $(CHIBIOS)/os/various/cpp_wrappers/chcpp.mk

# Define linker script file here
LDSCRIPT= target/STM32F401xE_mod.ld
#LDSCRIPT= $(STARTUPLD)/STM32F401xE.ld



#SHELL := /bin/bash

REVISION = unknown
REVISION = $(shell git rev-parse --short HEAD)

#REVISION = $(bash git rev-parse --short HEAD)
#REVISION = $(shell -c "git rev-parse --short HEAD")
#REVISION = $(bash -c "git rev-parse --short HEAD")
#REVISION = $(@git rev-parse --short HEAD)



# rwildcard definition
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))


# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.

#QQCSRC := $(call rwildcard, src/, *.c)

CSRC = $(ALLCSRC) \
       $(TESTSRC) 
       
#       \
#       $(CHIBIOS)/os/various/syscalls.c 

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.

#QQCPPSRC := $(call rwildcard, src/, *.cpp)

CPPSRC = $(ALLCPPSRC) 

#\
#         $(CHIBIOS)/os/various/cpp_wrappers/syscalls_cpp.cpp


# List ASM source files here.
ASMSRC = $(ALLASMSRC)

# List ASM with preprocessor source files here.
ASMXSRC = $(ALLXASMSRC)

# Inclusion directories.
INCDIR = $(CONFDIR) $(ALLINC) $(TESTINC)

# Define C warning options here.
CWARN = -Wall -Wextra -Wundef -Wstrict-prototypes

# Define C++ warning options here.
CPPWARN = -Wall -Wextra -Wundef

#
# Project, target, sources and paths
##############################################################################

##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
#UDEFS =
#UDEFS = -DBOARD_OTG_NOVBUSSENS -DSHELL_CONFIG_FILE -D'__REVISION__="[$(REVISION)]"'
#UDEFS = -D_DEBUG  -DBOARD_OTG_NOVBUSSENS -DSHELL_CONFIG_FILE -D'__REVISION__="$(REVISION)"'
UDEFS = -D_DEBUG  -DBOARD_OTG_NOVBUSSENS -DSHELL_CONFIG_FILE -D'__REVISION__="[$(REVISION)]"'
#UDEFS = -D_DEBUG  -DSHELL_CONFIG_FILE -D'__REVISION__="[$(REVISION)]"'

# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR = $(SRCDIR)

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS = -lm

#
# End of user section
##############################################################################

##############################################################################
# Common rules
#

#RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
# include $(RULESPATH)/arm-none-eabi.mk
# include $(RULESPATH)/rules.mk

##############################################################################
# Compiler settings
#

TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CPPC = $(TRGT)g++
# Enable loading with g++ only if you need C++ runtime support.
# NOTE: You can use C++ even without C++ support if you are careful. C++
#       runtime support makes code size explode.
#LD   = $(TRGT)gcc
LD   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
AR   = $(TRGT)ar
OD   = $(TRGT)objdump
SZ   = $(TRGT)size
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary

NM   = $(TRGT)nm

#
# Compiler settings
##############################################################################

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
include $(RULESPATH)/arm-none-eabi.mk
include $(RULESPATH)/rules.mk

#
# Common rules
##############################################################################

##############################################################################
# Custom rules
#

$(BUILDDIR)/$(PROJECT).size : $(BUILDDIR)/$(PROJECT).elf
	$(NM) -t d -S -C --size-sort $(BUILDDIR)/$(PROJECT).elf > $(BUILDDIR)/$(PROJECT).size
	
$(BUILDDIR)/$(PROJECT).asm : $(BUILDDIR)/$(PROJECT).elf
	$(OD) -D -S  $(BUILDDIR)/$(PROJECT).elf > $(BUILDDIR)/$(PROJECT).asm
	
disa : $(BUILDDIR)/$(PROJECT).size $(BUILDDIR)/$(PROJECT).asm
	$(SZ) $(BUILDDIR)/$(PROJECT).elf


print-%  : ; @echo $* = $($*)
    
    
#
# Custom rules
##############################################################################
