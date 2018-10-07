rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

GET_SRC_R = $(call rwildcard,../$1/,*.$2)

GET_SRC = $(wildcard ../$1/*.cpp ../$1/*.c)

LASTPART = $(shell basename $1)
LASTPARTS = $(foreach x,$1,$(call LASTPART,$x))

SRC = $(call GET_SRC_R,Utils,c) ../kernel/Memory/memcpy.c ../kernel/MetaInfo.cpp $(call GET_SRC,drivers/IDE) ../drivers/PCI.cpp $(wildcard Dummies/*.cpp Dummies/*.c) ../drivers/VGA_Stream.cpp ../drivers/VGA.cpp ../Libraries/sqlite3/sqlite3.c ../kernel/Error.c $(call GET_SRC,Support/sqlite) ../kernel/Interrupts/ISR.c ../kernel/Interrupts/IRQ_c.c
SRC += ../Std/stdlib.cpp
#SRC += $(call GET_SRC,Support/unwinder)
SRC += ../kernel/Memory/PhysicalMemory.cpp
SRC += ../kernel/Interrupts/irq_guard.cpp
SRC += ../kernel/Interrupts/Interrupts_cpp.cpp
SRC += ../boot/MultiBoot.cpp
SRC += $(call GET_SRC,kernel/Memory/Managers)
SRC += ../kernel/Scheduling/context.cpp
SRC += $(call GET_SRC_R,Utils,cpp)
SRC += ../kernel/scheduling/thread.cpp
SRC += ../kernel/Timer_c.cpp
SRC += $(call GET_SRC_R,Support/threading,cpp)
SRC += ../drivers/CPUID.cpp
SRC += ../drivers/APIC.cpp
#SRC += $(call GET_SRC_R,kernel/Filesystem/FAT32,cpp)
SRC += $(call GET_SRC_R,drivers/ACPI,cpp)
#SRC += ../kernel/Filesystem/Filesystem.cpp
SRC += $(call GET_SRC_R,kernel/Filesystem,cpp)
SRC += $(call GET_SRC_R,kernel/ResourceHandles,cpp)
#SRC += $(call GET_SRC_R,kernel/Filesystem/FAT32,cpp)
#SRC += $(call GET_SRC_R,kernel/Filesystem/Files,cpp)
#SRC += $(call GET_SRC_R,kernel/Filesystem/initrd,cpp)
SRC += $(call GET_SRC_R,kernel/Memory/PageRegions,cpp)
SRC += $(call GET_SRC_R,kernel/Memory/Heaps,cpp)
SRC += ../kernel/Memory/Heap.cpp
SRC += ../kernel/Memory/PageDir.cpp
SRC += ../kernel/Memory/Paging.cpp
#SRC += $(call GET_SRC_R,kernel/Memory,cpp)
SRC += $(call GET_SRC_R,kernel/Memory/Allocators,cpp)
#SRC += ../Std/pthread/pthread_cond_wait.cpp
SRC += $(call GET_SRC_R,Std/pthread,cpp)
SRC += $(call GET_SRC_R,drivers/Disk,cpp)
SRC += ../Std/errno_cpp.cpp
SRC += ../Std/errno.c
SRC += ../drivers/Driver.cpp
SRC += $(call GET_SRC_R,kernel/Memory/Driver,cpp)
SRC += $(call GET_SRC_R,kernel/Security,cpp)
SRC += $(call GET_SRC_R,kernel/Shell,cpp)
SRC += $(call GET_SRC_R,kernel/Shell,c)
SRC += $(call GET_SRC_R,kernel/Functions,cpp)

ASM_SRC = 
ASM_OBJS = $(ASM_SRC:.asm=.o)
ASM = $(word 1,$(CXX:-g++=-as))

GASM_SRC = Dummies/Utility_asm.S
GASM_OBJS = $(GASM_SRC:.S=.o)

C_OBJS = $(SRC:.c=.o)
OBJS = $(C_OBJS:.cpp=.o) $(ASM_OBJS) $(GASM_OBJS)

TESTSLIB_SRC = $(wildcard TestUtils/*.cpp)
TESTSLIB = $(TESTSLIB_SRC:.cpp=.o)
TESTSLIB_DEPS = $(TESTSLIB:.o=.d)

TESTS_SRC = $(wildcard *.cpp)
TESTS_SRC_C = $(wildcard *.c)
TESTS = $(TESTS_SRC:.cpp=.o) $(TESTS_SRC_C:.c=.o)



SHARED_LIB = SharedLib_Test
SHARED_LIB_NAME = Shared
SHARED_LIB_TARGET = $(SHARED_LIB)/lib$(SHARED_LIB_NAME).so
SHARED_LIB_SRC = $(wildcard $(SHARED_LIB)/*.cpp)
SHARED_LIB_OBJS = $(SHARED_LIB_SRC:.cpp=.o)





DEF_FLAGS += -DTESTING
DEF_FLAGS += -DSTACK_INFO_V
DEF_FLAGS += #-DINCLUDED_DEBUG_H_ENABLED
DEF_FLAGS += #-D"kassert(x)"="assert(x)"
DEF_FLAGS += -DDEBUG
DEF_FLAGS += #-DDEBUG_VERIFY
DEF_FLAGS += #-DTRACK_ALLOC
DEF_FLAGS += #-D__USE_MEM_POOL__
DEF_FLAGS += -DDEBUG_TRACK_QUERY
DEF_FLAGS += -D__USE_STD_NEW__
DEF_FLAGS += -DIGNORE_PORT_IO
DEF_FLAGS += -D_TRACE
DEF_FLAGS += -D"throw(...)"=""
DEF_FLAGS += -D"FS_INITRD_TEST_IMG"="\"../Tools/initrd/initrd.img\""


BOTH_FLAGS += $(DEF_FLAGS)
BOTH_FLAGS += -O0
BOTH_FLAGS += -I../Libraries
BOTH_FLAGS += -I../../CppUtilities
BOTH_FLAGS += -Werror
BOTH_FLAGS += #-Wfatal-errors
BOTH_FLAGS += #-fmax-errors=5
BOTH_FLAGS += -fmax-errors=1
BOTH_FLAGS += -Werror=nonnull
BOTH_FLAGS += -Werror=pragmas
BOTH_FLAGS += -Werror=int-to-pointer-cast
BOTH_FLAGS += -Werror=nonnull
BOTH_FLAGS += -fPIC
BOTH_FLAGS += -pie
#BOTH_FLAGS += -fgnu-tm
BOTH_FLAGS += #-DSQLITE_VFS_NATIVE_FS

BOTH_FLAGS += $(SQLITE_FLAGS)

SQLITE_FLAGS += -DSQLITE_THREADSAFE=0
SQLITE_FLAGS += -DSQLITE_OMIT_LOAD_EXTENSION
SQLITE_FLAGS += -DSQLITE_OS_OTHER=1
SQLITE_FLAGS += -DSQLITE_TEMP_STORE=3
SQLITE_FLAGS += #-DSQLITE_DEFAULT_FOREIGN_KEYS=1
SQLITE_FLAGS += #-DSQLITE_USE_URI=1

BASE_CXXFLAGS += -std=c++17 -I..
BASE_CXXFLAGS += -I../../../boost
BASE_CXXFLAGS += -fconcepts
#-ffreestanding -DDEBUG 
#BASE_CXXFLAGS += -DINCLUDED_NEW_HH
BASE_CXXFLAGS += $(BOTH_FLAGS)

CXXFLAGS += $(BASE_CXXFLAGS) -MMD

#BASE_CXXFLAGS += -fno-rtti
#BASE_CXXFLAGS += -fno-exceptions
CXXFLAGS += -Werror=delete-incomplete

CFLAGS += -MMD -I.. -ffreestanding -lgcc $(BOTH_FLAGS)

ASMFLAGS = -MMD









DEPS = $(OBJS:.o=.d) $(TESTS:.o=.d)



PREPROC_GET = PreProcTest.cxx
PREPROC_OUT = $(PREPROC_GET:.cxx=.pp)

ASM_OUT_SUFFIX = .asm.out.s


OTHER_OUT = $(PREPROC_OUT)
TARGET = Test.out

CXX_F = $(word 1,$(CXX))
CC_F = $(word 1,$(CC))


all: $(OTHER_OUT) $(TARGET) $(SHARED_LIB_TARGET)

clean: 
	-@rm -f $(OTHER_OUT)
	-@rm -f $(OBJS)
	-@rm -f $(TESTS)
	-@rm -f $(TESTSLIB)
	-@rm -f $(DEPS)
	-@rm -f $(TESTSLIB_DEPS)
	-@rm -f $(OPTIM_OUT)
	-@rm -f $(OBJS:.o=$(ASM_OUT_SUFFIX))
	-@rm -f $(TESTS:.o=$(ASM_OUT_SUFFIX))
	-@rm -f $(TARGET)
	@echo $(OBJS:.o=$(ASM_OUT_SUFFIX))
	@echo ' '
	@echo ' '
	@echo $(OBJS) $(TESTS) $(TESTSLIB)
	

.fake: 
	@echo ' '
	@echo ' '
	@echo '$$SRC = ' $(call LASTPARTS,$(SRC))
	@echo '  '
	@echo '$$OBJS = ' $(OBJS)
	@echo '  '
	@echo '$$TESTSLIB = ' $(call LASTPARTS,$(TESTSLIB))
	@echo '  '
	@echo '$$DEPS = ' $(call LASTPARTS,$(DEPS))
	@echo '  '
	@echo '$$CXXFLAGS = ' $(CXXFLAGS)
	@echo ' '
	@echo ' '
	@echo ' '
	@echo ' '


$(TARGET): $(OBJS) $(TESTS) $(TESTSLIB)
	@echo ' '
	-@rm -f $(TARGET)
	@echo $(CXX_F) $(call LASTPARTS,$^) ' > ' $(call LASTPART,$@)
	@echo ' '
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(TESTS) $(TESTSLIB) -lgcc -ldl
	@echo ' '
	echo $(ASM_OBJS)
	


%.o: %.cpp ForceRebuild.cpp
	@echo ' '
	@echo $(CXX_F) $(call LASTPART,$<) ' > ' $(call LASTPART,$@)
	@$(CXX) $(CXXFLAGS) -o $@ -c $<
	@echo $(CXX_F) $(call LASTPART,$<) ' > ' $(call LASTPART,$(@:.o=$(ASM_OUT_SUFFIX)))
	@#$(CXX) $(BASE_CXXFLAGS) -o $(@:.o=$(ASM_OUT_SUFFIX)) -S $<
	@echo ' '
	

%.o: %.c ForceRebuild.cpp
	@echo ' '
	@echo $(CC_F) $(call LASTPART,$<) ' > ' $(call LASTPART,$@)
	@$(CC) $(CFLAGS) -o $@ -c $<
	@echo ' '
	

%.o: %.asm
	@echo '    '
	@echo '    '
	@echo '    '
	$(ASM) -o $@ $<
	@echo '     '
	
%.o: %.S
	@echo '    '
	@echo '    '
	@echo '    '
	$(CXX) -c $(DEF_FLAGS) -o $@ $<
	@echo '     '

-include $(DEPS)
-include $(TESTSLIB_DEPS)
include Makefiles_Common

TestUtils/MemTracker.o: TestUtils/MemTracker.cpp TestUtils/MemTracker.hh Makefile
	@echo ' '
	@echo $(CXX_F) $(call LASTPART,$<) ' > ' $(call LASTPART,$@)
	@$(CXX) $(CXXFLAGS) -o $@ -c $<
	@echo ' '
	

Main.o: Main.cpp Makefile
	@echo ' '
	@echo $(CXX_F) $(call LASTPART,$<) ' > ' $(call LASTPART,$@)
	@$(CXX) $(CXXFLAGS) -o $@ -c $<
	@echo ' '


$(PREPROC_OUT): $(PREPROC_GET)
	@echo $(CXX_F) $(call LASTPART,$^) ' > ' $(call LASTPART,$@)
	@$(CXX) $(CXXFLAGS) -o $@ -E $^


$(SHARED_LIB_TARGET): $(SHARED_LIB_OBJS)
	@echo $(CXX_F) $(call LASTPART,$<) ' > ' $(call LASTPART,$@)
	@$(CXX) $(CXXFLAGS) -shared -o $@ $<