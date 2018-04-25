

GET_SRC = $(wildcard ../../$1/*.cpp ../../$1/*.c)

SRC = $(call GET_SRC,Utils) $(call GET_SRC,Utils/detail) ../../kernel/Memory/memcpy.c ../../kernel/MetaInfo.cpp ../../drivers/PCI.cpp $(wildcard ../Dummies/*.cpp ../Dummies/*.c) ../../drivers/VGA_Stream.cpp ../../drivers/VGA.cpp ../../Libraries/sqlite3/sqlite3.c ../../kernel/Error.c  $(call GET_SRC,Support/sqlite)

ASM_SRC = ../../kernel/Utility_asm.asm
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

BOTH_FLAGS += -DTESTING -DSTACK_INFO_V -I../../Libraries -I../../../CppUtilities -Wfatal-errors #-DINCLUDED_DEBUG_H_ENABLED #-D"kassert(x)"="assert(x)"
BOTH_FLAGS += -DDEBUG #-DDEBUG_VERIFY
BOTH_FLAGS += #-DTRACK_ALLOC

CXXFLAGS += -std=c++14 -MMD -I../.. -fno-rtti -fno-exceptions -ffreestanding -lgcc -DDEBUG -DINCLUDED_NEW_HH $(BOTH_FLAGS)

CFLAGS += -MMD -I../.. -ffreestanding -lgcc $(BOTH_FLAGS)

ASMFLAGS = -MMD -I../..


DEPS = $(OBJS:.o=.d) $(TESTS:.o=.d)

PREPROC_GET = PreProcTest.cxx
PREPROC_OUT = $(PREPROC_GET:.cxx=.pp)


all: Test.out .fake
	

.fake: 
	


Test.out: $(OBJS) $(TESTS) $(TESTSLIB)
	@echo '$$SRC = ' $(SRC)
	@echo '  '
	@echo '$$OBJS = ' $(OBJS)
	@echo '  '
	@echo '$$TESTSLIB = ' $(TESTSLIB)
	@echo '  '
	@echo '$$DEPS = ' $(DEPS)
	@echo '  '
	echo $(OBJS) $(TESTS)
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.asm
	@echo '    '
	@echo '    '
	@echo '    '
	$(ASM) $(ASMFLAGS) -o $@ -c $<
	@echo '     '

-include $(DEPS)
-include $(TESTSLIB_DEPS)

TestUtils/MemTracker.o: TestUtils/MemTracker.cpp TestUtils/MemTracker.hh Makefile
	$(CXX) $(CXXFLAGS) -o $@ -c $<

Main.o: Main.cpp Makefile
	$(CXX) $(CXXFLAGS) -o $@ -c $<