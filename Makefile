C_SRC = $(wildcard kernel/*.c drivers/*.c)
CPP_SRC = $(wildcard kernel/*.cpp drivers/*.cpp)
BOOT_SRC = $(wildcard boot/*.s)
ASM_SRC_ASM = $(wildcard kernel/*.asm drivers/*.asm)
#ASM_SRC_S = $(wildcard kernel/*.S drivers/*.S)
#ASM_SRC_s = $(wildcard kernel/*.s drivers/*.s)

C_OBJS = $(C_SRC:.c=.o)
CPP_OBJS = $(CPP_SRC:.cpp=.o)
BOOT_OBJS = $(BOOT_SRC:.s=.o)
ASM_OBJS = $(ASM_SRC_ASM:.asm=.o) $(ASM_SRC_S:.S=.o) $(ASM_SRC_s:.s=.o)

C_DEPS = $(C_OBJS:.o=.d)
CPP_DEPS = $(CPP_OBJS:.o=.d)


WARNINGS_FLAGS += -Wall
WARNINGS_FLAGS += -Wextra
#WARNINGS_FLAGS += -fpermissive
#WARNINGS_FLAGS += --pedantic
#WARNINGS_FLAGS += -Werror
#WARNINGS_FLAGS += -Wfatal-errors

CC = i686-elf-gcc
CXX = i686-elf-g++
ASM = i686-elf-as

CC_FLAGS	= $(WARNINGS_FLAGS) -ffreestanding -O2 -MMD -I.
CXX_FLAGS	= $(WARNINGS_FLAGS) -ffreestanding -O2 -MMD -I. #-fno-exceptions -fno-rtti
ASM_FLAGS	= 

all: myos.iso
	@echo $(CPP_DEPS)

clean:
	-@rm -f $(C_OBJS) $(CPP_OBJS) $(CPP_DEPS) $(BOOT_OBJS) *.bin $(wildcard boot/*.o boot/*.bin) $(ASM_OBJS)

myos.iso: myos.bin
	@cp $< grub/isodir/boot/$<
	@grub-mkrescue -o $@ grub/isodir


myos.bin: $(BOOT_OBJS) $(CPP_OBJS) $(C_OBJS) $(ASM_OBJS) linker.ld
	@echo $(BOOT_OBJS)
	@echo $(CPP_OBJS)
	@echo $(C_OBJS)
	@echo $(ASM_OBJS)
	$(CXX) -T linker.ld -o $@ $(CXX_FLAGS) -nostdlib $(BOOT_OBJS) $(CPP_OBJS) $(C_OBJS) $(ASM_OBJS) -lgcc

%.o: %.s
	$(ASM) $(ASM_FLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

%.o: %.asm
	nasm $(ASM_FLAGS) -f elf $< -o $@

-include $(CPP_DEPS)
-include $(C_DEPS)