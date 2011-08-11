# make all = Make target.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF (for use with AVR Studio 3.x or VMLAB).
#
# make extcoff = Convert ELF to AVR Extended COFF (for use with AVR Studio
#                4.07 or greater).
#
# make program = Download the hex file to the device, using avrdude.  Please
#                customize the avrdude settings below first!
#
# make filename.s = Just compile filename.c into the assembler code only

# Microcontroler's name
MCU = atmega32

# Oscillator Frequency Define in Hz
F_CPU = 16000000UL

# Uncomment for debug-build
#DEBUG = -DDEBUG

# Output format. (can be srec, ihex, binary)
FORMAT = ihex

# Target file name (without extension).
TARGET = idetrol

# List C source files here. (C dependencies are automatically generated.)
SRC = main.c \
	rs232/rs232.c \
	rs232/stdiowrap.c \
	irmp/irmp.c \
	spi/spi.c \
	ata.c \
	atapi.c \
	atapiplayer.c

# List Assembler source files here.
# Make them always end in a capital .S.  Files ending in a lowercase .s
# will not be considered source files but generated files (assembler
# output from the compiler), and will be deleted upon "make clean"!
# Even though the DOS/Win* filesystem matches both .s and .S the same,
# it will preserve the spelling of the filenames, and gcc itself does
# care about how the name is spelled on its command-line.
ASRC =


# Optimization level, can be [0, 1, 2, 3, s].
OPT = s

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
#DEBUGFORMAT = -gdwarf-2

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
EXTRAINCDIRS =


# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here
CDEFS =

# Place -I options here
CINCS =


# Compiler flags.
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = $(CDEFS) $(CINCS)
CFLAGS += -O$(OPT)
CFLAGS += $(DEBUGFORMAT)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fgnu89-inline
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(<:.c=.lst)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)
CFLAGS += -DF_CPU=$(F_CPU)
CFLAGS += $(DEBUG)


# Assembler flags.
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs
ASFLAGS += -DF_CPU=$(F_CPU)


#Additional libraries.

# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

PRINTF_LIB =

# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

SCANF_LIB =

MATH_LIB = -lm

# External memory options

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
# HACK:
LDFLAGS += -T /usr/x86_64-pc-linux-gnu/avr/lib/ldscripts/avr5.x



# Programming support using avrdude. Settings and variables.

# Programming hardware: alf avr910 avrisp bascom bsd
# dt006 pavr picoweb pony-stk200 sp12 stk200 stk500
#
# Type: avrdude -c ?
# to get a full listing.
#
AVRDUDE_PROGRAMMER = avrispmkII

# Specify JTAG/STK500v2 bit clock period (in us)
AVRDUDE_STK500V2_BITCLOCK = -B 10

# usb, com1 (serial port), lpt1 (parallel port)
AVRDUDE_PORT = usb

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep


# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude>
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_STK500V2_BITCLOCK)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)



# ---------------------------------------------------------------------------



# Define programs and commands.
SHELL = sh
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
COPY = cp
DOXYGEN = doxygen

# Colors
COLOR_GOOD=\e[32;01m
COLOR_WARN=\e[33;01m
COLOR_BAD=\e[31;01m
COLOR_HILITE=\e[36;01m
COLOR_BRACKET=\e[34;01m
COLOR_NORMAL=\e[0m


# Define Messages
# English
MSG_PROGRAM = " ${COLOR_GOOD}*${COLOR_NORMAL} Uploading: "
MSG_SIZE_BEFORE = " ${COLOR_GOOD}*${COLOR_NORMAL} Size before: "
MSG_SIZE_AFTER = " ${COLOR_GOOD}*${COLOR_NORMAL} Size after: "
MSG_COFF = " ${COLOR_GOOD}*${COLOR_NORMAL} Converting to AVR COFF: "
MSG_EXTENDED_COFF = " ${COLOR_GOOD}*${COLOR_NORMAL} Converting to AVR Extended COFF: "
MSG_FLASH = " ${COLOR_GOOD}*${COLOR_NORMAL} Creating load file for Flash: "
MSG_EEPROM = " ${COLOR_GOOD}*${COLOR_NORMAL} Creating load file for EEPROM: "
MSG_EXTENDED_LISTING = " ${COLOR_GOOD}*${COLOR_NORMAL} Creating Extended Listing: "
MSG_SYMBOL_TABLE = " ${COLOR_GOOD}*${COLOR_NORMAL} Creating Symbol Table: "
MSG_LINKING = " ${COLOR_GOOD}*${COLOR_NORMAL} Linking: "
MSG_COMPILING = " ${COLOR_GOOD}*${COLOR_NORMAL} Compiling: "
MSG_ASSEMBLING = " ${COLOR_GOOD}*${COLOR_NORMAL} Assembling: "
MSG_CLEANING = " ${COLOR_GOOD}*${COLOR_NORMAL} Cleaning Project: "
MSG_BUILDDOC = " ${COLOR_GOOD}*${COLOR_NORMAL} Building Documentation: "
MSG_BUILDDOCPDF = " ${COLOR_GOOD}*${COLOR_NORMAL} Building PDF Documentation: "
MSG_BUILDDOCIMG = " ${COLOR_GOOD}*${COLOR_NORMAL} Building Documentation Images: "




# Define all object files.
OBJ = $(SRC:.c=.o) $(ASRC:.S=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(SRC:.c=.lst)


# Compiler flags to generate dependency files.
### GENDEPFLAGS = -Wp,-M,-MP,-MT,$(*F).o,-MF,.dep/$(@F).d
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)





# Default target.
all: sizebefore build sizeafter

#build: elf hex eep lss sym
build: elf hex lss sym

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf
sizebefore:
	@if [ -f $(TARGET).elf ]; then echo -e $(MSG_SIZE_BEFORE); $(ELFSIZE); fi

sizeafter:
	@if [ -f $(TARGET).elf ]; then echo -e $(MSG_SIZE_AFTER); $(ELFSIZE); fi


# Program the device.
#program: $(TARGET).hex $(TARGET).eep
program: $(TARGET).hex
	@echo -e $(MSG_PROGRAM) $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)




# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000


coff: $(TARGET).elf
	@echo -e $(MSG_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-avr $< $(TARGET).cof
	@echo


extcoff: $(TARGET).elf
	@echo -e $(MSG_EXTENDED_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof
	@echo



# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo -e $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -S -R .eeprom $< $@
	@echo

%.eep: %.elf
	@echo -e $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@
	@echo

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo -e $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S $< > $@
	@echo

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo -e $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@
	@echo



# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo -e $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)
	@echo


# Compile: create object files from C source files.
%.o : %.c
	@echo -e $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@
	@echo


# Compile: create assembler files from C source files.
%.s : %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
%.o : %.S
	@echo -e $(MSG_ASSEMBLING) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $@
	@echo

# Images for documentation
imgdoc:
	@echo -e $(MSG_BUILDDOCIMG)
	@echo

# Doxygen docs
basedoc: $(SRC) Doxyfile imgdoc
	@echo -e $(MSG_BUILDDOC)
	doxygen Doxyfile
	@echo

# Doxygen pdf
pdf: basedoc
	@echo -e $(MSG_BUILDDOCPDF)
	(cd doc/latex && make pdf)
	@echo

doc: basedoc

clean:
	@echo -e $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).eep
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).cof
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).a90
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lnk
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(OBJ)
	$(REMOVE) $(LST)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) -r .dep
	$(REMOVE) -r doc/html
	$(REMOVE) -r doc/latex
	@echo


# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
