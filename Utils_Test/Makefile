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
SRC += $(call GET_SRC_R,kernel/Filesystem/FAT32,cpp)
SRC += $(call GET_SRC_R,drivers/ACPI,cpp)

ASM_SRC = ../kernel/Utility_asm.asm
ASM_OBJS = $(ASM_SRC:.asm=.o)
ASM = $(word 1,$(CXX:-g++=-as))

C_OBJS = $(SRC:.c=.o)
OBJS = $(C_OBJS:.cpp=.o) $(ASM_OBJS)

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






BOTH_FLAGS += -O0
BOTH_FLAGS += -DTESTING
BOTH_FLAGS += -DSTACK_INFO_V
BOTH_FLAGS += -I../Libraries
BOTH_FLAGS += -I../../CppUtilities
BOTH_FLAGS += #-Wfatal-errors
BOTH_FLAGS += #-fmax-errors=5
BOTH_FLAGS += -fmax-errors=1
BOTH_FLAGS += #-DINCLUDED_DEBUG_H_ENABLED
BOTH_FLAGS += #-D"kassert(x)"="assert(x)"
BOTH_FLAGS += -DDEBUG #-DDEBUG_VERIFY
BOTH_FLAGS += #-DTRACK_ALLOC
BOTH_FLAGS += -DDEBUG_TRACK_QUERY
BOTH_FLAGS += -D__USE_STD_NEW__
BOTH_FLAGS += -DIGNORE_PORT_IO
BOTH_FLAGS += -D_TRACE
BOTH_FLAGS += -fPIC
BOTH_FLAGS += -pie
BOTH_FLAGS += -D"throw(...)"=""
#BOTH_FLAGS += -fgnu-tm

BOTH_FLAGS += $(SQLITE_FLAGS)

SQLITE_FLAGS += -DSQLITE_THREADSAFE=0
SQLITE_FLAGS += -DSQLITE_OMIT_LOAD_EXTENSION
SQLITE_FLAGS += #-DSQLITE_OS_OTHER=1
SQLITE_FLAGS += -DSQLITE_TEMP_STORE=3

BASE_CXXFLAGS += -std=c++17 -I.. #-ffreestanding -DDEBUG 
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