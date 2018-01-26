C_SRC = $(wildcard kernel/*.c drivers/*.c)
CPP_SRC = $(wildcard kernel/*.cpp drivers/*.cpp)
BOOT_SRC = $(wildcard boot/*.s)

C_OBJS = $(C_SRC:.c=.o)
CPP_OBJS = $(CPP_SRC:.cpp=.o)
BOOT_OBJS = $(BOOT_SRC:.s=.o)

CPP_DEPS = $(CPP_OBJS:.o=.d)



CC = i686-elf-gcc
CXX = i686-elf-g++
ASM = i686-elf-as

CC_FLAGS = -ffreestanding -O2 -Wall -Wextra
CXX_FLAGS = -ffreestanding -O2 -Wall -Wextra -MMD -I. #-fno-exceptions -fno-rtti
ASM_FLAGS = 

all: myos.iso
	@echo $(CPP_DEPS)

clean:
	-@rm -f $(C_OBJS) $(CPP_OBJS) $(CPP_DEPS) $(BOOT_OBJS) *.bin $(wildcard boot/*.o boot/*.bin)

myos.iso: myos.bin
	@cp $< grub/isodir/boot/$<
	@grub-mkrescue -o $@ grub/isodir


myos.bin: $(BOOT_OBJS) $(CPP_OBJS) $(C_OBJS) linker.ld
	$(CXX) -T linker.ld -o $@ $(CXX_FLAGS) -nostdlib $(BOOT_OBJS) $(CPP_OBJS) $(C_OBJS) -lgcc

%.o: %.s
	$(ASM) $(ASM_FLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

-include $(CPP_DEPS)