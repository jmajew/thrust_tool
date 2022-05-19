# List of all the board related files.
BOARDSRC = target/ST_NUCLEO64_F401RE/board.c

# Required include directories
BOARDINC = target/ST_NUCLEO64_F401RE

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)
