# List of all the board related files.
BOARDSRC = $(CHIBIOS)/os/hal/boards/QQ_THRUST_V1/board.c

# Required include directories
BOARDINC = $(CHIBIOS)/os/hal/boards/QQ_THRUST_V1

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)
