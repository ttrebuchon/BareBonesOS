

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

GET_SRC = $(call rwildcard,$1/,*.$2)




C_SRC = $(call GET_SRC,kernel,c) $(call GET_SRC,drivers,c) $(call GET_SRC,Libraries,c)
CPP_SRC = $(call GET_SRC,kernel,cpp) $(call GET_SRC,drivers,cpp) $(call GET_SRC,Libraries,cpp)
BOOT_SRC = $(call GET_SRC,boot,s)
#ASM_SRC_ASM = $(wildcard kernel/*.asm drivers/*.asm)
ASM_SRC_ASM = $(call GET_SRC,kernel,asm) $(call GET_SRC,drivers,asm)
#ASM_SRC_S = $(wildcard kernel/*.S drivers/*.S)
#ASM_SRC_s = $(wildcard kernel/*.s drivers/*.s)

HDD2 = Dummy.img
HDD2_GEN = Tools/Dummy_Img/Main.out
HDD2_SRC = $(wildcard Tools/Dummy_Img/*.cpp)

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

SYSROOT = sysroot
CRTBEGIN_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

NO_WARN = -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function

BOTH_FLAGS += -DFIXED_PAGE_STRUCT -D_TRACE $(NO_WARN)
CFLAGS	= -nostdlib $(WARNINGS_FLAGS) -ffreestanding -Og -MMD -I. -Werror-implicit-function-declaration --sysroot=$(SYSROOT) $(BOTH_FLAGS)
CXX_FLAGS	= -std=c++14 -nostdlib $(WARNINGS_FLAGS) -ffreestanding -Og -MMD -I. -fno-exceptions -fno-rtti --sysroot=$(SYSROOT) $(BOTH_FLAGS)
ASM_FLAGS	= 

all: $(HDD2) myos.iso
	#@echo $(CPP_DEPS)

clean:
	-@rm -f $(C_OBJS) $(CPP_OBJS) $(CPP_DEPS) $(BOOT_OBJS) *.bin $(wildcard boot/*.o boot/*.bin) $(ASM_OBJS)

myos.iso: myos.bin
	@cp $< grub/isodir/boot/$<
	@grub-mkrescue -o $@ grub/isodir


myos.bin: $(BOOT_OBJS) $(CRTBEGIN_OBJ) $(CPP_OBJS) $(C_OBJS) $(ASM_OBJS) $(CRTEND_OBJ) linker.ld
	$(CXX) -T linker.ld -o $@ $(CXX_FLAGS) -nostdlib $(BOOT_OBJS) $(CRTBEGIN_OBJ) $(CPP_OBJS) $(C_OBJS) $(ASM_OBJS) $(CRTEND_OBJ) -lgcc

%.o: %.s
	$(ASM) $(ASM_FLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

%.o: %.asm
	@nasm $(ASM_FLAGS) -f elf $< -o $@

-include $(CPP_DEPS)
-include $(C_DEPS)

$(HDD2): $(HDD2_GEN)
	./$(HDD2_GEN) $(HDD2)

$(HDD2_GEN): $(HDD2_SRC)
	$(CXX) -o $(HDD2_GEN) $^
	