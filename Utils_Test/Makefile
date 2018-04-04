

GET_SRC = $(wildcard ../$1/*.cpp ../$1/*.c)

LASTPART = $(shell basename $1)
LASTPARTS = $(foreach x,$1,$(call LASTPART,$x))

SRC = $(call GET_SRC,Utils) $(call GET_SRC,Utils/detail) ../kernel/Memory/memcpy.c ../kernel/MetaInfo.cpp ../drivers/IDE/DMA.cpp $(call GET_SRC,drivers/IDE) ../drivers/PCI.cpp $(wildcard Dummies/*.cpp Dummies/*.c) ../drivers/VGA_Stream.cpp ../drivers/VGA.cpp ../Libraries/sqlite3/sqlite3.c ../kernel/Error.c $(call GET_SRC,Support/sqlite)

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

BOTH_FLAGS += -DTESTING -DSTACK_INFO_V -I../Libraries -I../../CppUtilities -Wfatal-errors #-DINCLUDED_DEBUG_H_ENABLED #-D"kassert(x)"="assert(x)"
BOTH_FLAGS += -DDEBUG #-DDEBUG_VERIFY
BOTH_FLAGS += #-DTRACK_ALLOC

BOTH_FLAGS += $(SQLITE_FLAGS)

SQLITE_FLAGS += -DSQLITE_THREADSAFE=0
SQLITE_FLAGS += -DSQLITE_OMIT_LOAD_EXTENSION
SQLITE_FLAGS += -DSQLITE_OS_OTHER=1
SQLITE_FLAGS += -DSQLITE_TEMP_STORE=3

CXXFLAGS += -std=c++14 -MMD -I.. -ffreestanding -lgcc -DDEBUG -DINCLUDED_NEW_HH $(BOTH_FLAGS)

CXXFLAGS += -fno-rtti
CXXFLAGS += -fno-exceptions

CFLAGS += -MMD -I.. -ffreestanding -lgcc $(BOTH_FLAGS)

ASMFLAGS = -MMD -I..


DEPS = $(OBJS:.o=.d) $(TESTS:.o=.d)

PREPROC_GET = PreProcTest.cxx
PREPROC_OUT = $(PREPROC_GET:.cxx=.pp)

CXX_F = $(word 1,$(CXX))
CC_F = $(word 1,$(CC))


all: $(PREPROC_OUT) Test.out .fake
	

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


Test.out: $(OBJS) $(TESTS) $(TESTSLIB)
	@echo ' '
	@echo $(CXX_F) $(call LASTPARTS,$^) ' > ' $(call LASTPART,$@)
	@echo ' '
	@$(CXX) $(CXXFLAGS) -o $@ $^
	


%.o: %.cpp ForceRebuild.cpp
	@echo ' '
	@echo $(CXX_F) $(call LASTPART,$<) ' > ' $(call LASTPART,$@)
	@$(CXX) $(CXXFLAGS) -o $@ -c $<
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
	$(ASM) $(ASMFLAGS) -o $@ -c $<
	@echo '     '

-include $(DEPS)
-include $(TESTSLIB_DEPS)

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
	$(CXX) $(CXXFLAGS) -o $@ -E $^