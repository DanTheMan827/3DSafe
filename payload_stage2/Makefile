rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

INCLUDES	:=	source source/font source/fatfs source/nand

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

# include $(DEVKITARM)/3ds_rules

CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
LD := arm-none-eabi-ld
OC := arm-none-eabi-objcopy

name := $(shell basename $(CURDIR))

dir_source := source
dir_build := build
dir_data := data

# ASFLAGS := -mcpu=arm946e-s
# CFLAGS  := -Wall -Wextra -MMD -MP -mthumb -mthumb-interwork $(ASFLAGS) -fno-builtin -std=c99 -Wno-main -O2 -flto -ffast-math
CFLAGS  := -Wall -Wextra -mthumb -mthumb-interwork -flto -flto -ffast-math -O2 -std=c99 -g -Wpedantic -Wcast-align -pedantic -march=armv5te -mtune=arm946e-s -fomit-frame-pointer
LDFLAGS := -nostartfiles

CFLAGS	+=	$(INCLUDE) -DEXEC_A9LH -DARM9

objects = $(patsubst $(dir_source)/%.s, $(dir_build)/%.o, \
          $(patsubst $(dir_source)/%.c, $(dir_build)/%.o, \
          $(call rwildcard, $(dir_source), *.s *.c)))

bundledobjects = $(dir_build)/arm11.bin.o

define bin2o
	bin2s $< | $(AS) -o $(@)
	echo "extern const u8" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> $(dir_build)/bundled.h
	echo "extern const u32" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> $(dir_build)/bundled.h
endef

.PHONY: all
all: $(name).bin

.PHONY: clean
clean:
	@rm -rf $(dir_build) $(dir_data)

$(name).bin: $(dir_build)/$(name).elf
	$(OC) -S -O binary $< $@

$(dir_build)/$(name).elf: $(bundledobjects) $(objects)
	$(LINK.o) -T linker.ld $(OUTPUT_OPTION) $^

$(dir_build)/%.bin.o: $(dir_data)/%.bin
	@mkdir -p "$(@D)"
	@$(bin2o)

$(dir_build)/memory.o: CFLAGS += -O3

$(dir_build)/%.o: $(dir_source)/%.c
	@mkdir -p "$(@D)"
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(dir_build)/%.o: $(dir_source)/%.s
	@mkdir -p "$(@D)"
	$(COMPILE.s) $(OUTPUT_OPTION) $<
include $(call rwildcard, $(dir_build), *.d)
