

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

GET_SRC = $(call rwildcard,$1/,*.$2)




C_SRC += $(call GET_SRC,kernel,c)
C_SRC += $(call GET_SRC,drivers,c)
C_SRC += $(call GET_SRC,Libraries,c)
C_SRC += $(call GET_SRC,Utils,c)
C_SRC += $(call GET_SRC,Std,c)
CPP_SRC += $(call GET_SRC,kernel,cpp)
CPP_SRC += $(call GET_SRC,drivers,cpp)
CPP_SRC += $(call GET_SRC,Libraries,cpp)
#CPP_SRC += $(call GET_SRC,Libraries,cc)
CPP_SRC += $(call GET_SRC,Std,cpp)
CPP_SRC += $(call GET_SRC,Utils,cpp)
CPP_SRC += $(call GET_SRC,Support,cpp)
CPP_SRC += $(call GET_SRC,boot,cpp)
BOOT_SRC = $(call GET_SRC,boot,s)
#ASM_SRC_ASM = $(wildcard kernel/*.asm drivers/*.asm)
ASM_SRC_ASM = $(call GET_SRC,kernel,asm) $(call GET_SRC,drivers,asm)
#ASM_SRC_S = $(wildcard kernel/*.S drivers/*.S)
#ASM_SRC_s = $(wildcard kernel/*.s drivers/*.s)

HDD2 = Dummy.img
HDD2_GEN = Tools/Dummy_Img/Main.out
HDD2_SRC = $(wildcard Tools/Dummy_Img/*.cpp)

C_OBJS = $(C_SRC:.c=.o)
_CPP_OBJS = $(CPP_SRC:.cpp=.o)
CPP_OBJS = $(_CPP_OBJS:.cc=.o)
BOOT_OBJS = $(BOOT_SRC:.s=.o)
ASM_OBJS = $(ASM_SRC_ASM:.asm=.o) $(ASM_SRC_S:.S=.o) $(ASM_SRC_s:.s=.o)

C_DEPS = $(C_OBJS:.o=.d)
CPP_DEPS = $(CPP_OBJS:.o=.d)


WARNINGS_FLAGS += -Wall
WARNINGS_FLAGS += -Wextra
#WARNINGS_FLAGS += -fpermissive
#WARNINGS_FLAGS += --pedantic
#WARNINGS_FLAGS += -Werror
WARNINGS_FLAGS += -Wfatal-errors
WARNINGS_FLAGS += -Werror=ignored-qualifiers
WARNINGS_FLAGS += -Werror=attributes

CC = i686-elf-gcc
CXX = i686-elf-g++
ASM = i686-elf-as

SYSROOT = sysroot
CRTBEGIN_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

NO_WARN = -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-comment

BOTH_FLAGS += -DFIXED_PAGE_STRUCT
BOTH_FLAGS += -D_TRACE
BOTH_FLAGS += $(NO_WARN)
BOTH_FLAGS += -DDEBUG
#BOTH_FLAGS += -DDEBUG_IDENTITY_DIR
BOTH_FLAGS += -ILibraries
BOTH_FLAGS += $(SQLITE_FLAGS)
BOTH_FLAGS += -I./Std
#BOTH_FLAGS += -D__USING_CXXRT__
BOTH_FLAGS += -DPTHREAD_IGNORE
#BOTH_FLAGS += -D__NOSTDLIB__
#BOTH_FLAGS += -D_GNU_SOURCE
BOTH_FLAGS += -DLIBCXXRT_WEAK_LOCKS



SQLITE_FLAGS += -DSQLITE_THREADSAFE=0
SQLITE_FLAGS += -DSQLITE_OMIT_LOAD_EXTENSION
SQLITE_FLAGS += -DSQLITE_OS_OTHER=1
SQLITE_FLAGS += -DSQLITE_TEMP_STORE=3



CFLAGS		= -nostdlib $(WARNINGS_FLAGS) -ffreestanding -Og -MMD -I. -Werror-implicit-function-declaration --sysroot=$(SYSROOT) $(BOTH_FLAGS)
CXX_FLAGS	= -std=c++14
CXX_FLAGS  += -nostdlib
CXX_FLAGS  += $(WARNINGS_FLAGS)
CXX_FLAGS  += -ffreestanding
CXX_FLAGS  += -Og
CXX_FLAGS  += -MMD
CXX_FLAGS  += -I.
CXX_FLAGS  += -fno-exceptions
CXX_FLAGS  += -fno-rtti
CXX_FLAGS  += --sysroot=$(SYSROOT)
CXX_FLAGS  += $(BOTH_FLAGS)
ASM_FLAGS	= 

all: .fake $(HDD2) myos.iso
	#@echo $(CPP_DEPS)

.fake:
	@echo '   '
	@echo '$$OBJS = ' $(BOOT_OBJS) $(CRTBEGIN_OBJ) $(CPP_OBJS) $(C_OBJS) $(ASM_OBJS) $(CRTEND_OBJ)
	@echo '   '

clean:
	-@rm -f $(C_OBJS) $(CPP_OBJS) $(CPP_DEPS) $(BOOT_OBJS) *.bin $(wildcard boot/*.o boot/*.bin) $(ASM_OBJS)
	-@rm -f $(CPP_DEPS) $(C_DEPS)

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

%.o: %.cc
	$(CXX) $(CXX_FLAGS) -c $< -o $@

%.o: %.asm
	@nasm $(ASM_FLAGS) -f elf $< -o $@

-include $(CPP_DEPS)
-include $(C_DEPS)

$(HDD2): $(HDD2_GEN)
	./$(HDD2_GEN) $(HDD2)

$(HDD2_GEN): $(HDD2_SRC)
	$(CXX) -o $(HDD2_GEN) $^
	