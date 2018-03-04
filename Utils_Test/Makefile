
GET_SRC = $(wildcard ../$1/*.cpp ../$1/*.c)

SRC = $(call GET_SRC,kernel/utils) $(call GET_SRC,kernel/utils/detail) ../kernel/Memory/memcpy.c ../kernel/MetaInfo.cpp ../drivers/IDE/DMA.cpp $(call GET_SRC,drivers/IDE) ../drivers/PCI.cpp $(wildcard Dummies/*.cpp Dummies/*.c) ../drivers/VGA_Stream.cpp ../drivers/VGA.cpp

C_OBJS = $(SRC:.c=.o)
OBJS = $(C_OBJS:.cpp=.o)

TESTS_SRC = $(wildcard *.cpp)
TESTS_SRC_C = $(wildcard *.c)
TESTS = $(TESTS_SRC:.cpp=.o) $(TESTS_SRC_C:.c=.o)

BOTH_FLAGS += -DTESTING -DSTACK_INFO_V -DDEBUG #-DINCLUDED_DEBUG_H_ENABLED #-D"kassert(x)"="assert(x)"

CXXFLAGS += -std=c++14 -MMD -I.. -fno-rtti -fno-exceptions -ffreestanding -lgcc -DDEBUG -DINCLUDED_NEW_HH $(BOTH_FLAGS)

CFLAGS += -MMD -I.. -ffreestanding -lgcc $(BOTH_FLAGS)


DEPS = $(OBJS:.o=.d) $(TESTS:.o=.d)

all: Test.out .fake
	

.fake: 
	



Test.out: $(OBJS) $(TESTS)
	@echo '$$SRC = ' $(SRC)
	@echo '$$OBJS = ' $(OBJS)
	echo $(OBJS) $(TESTS)
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<



-include $(DEPS)