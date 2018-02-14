
GET_SRC = $(wildcard ../$1/*.cpp ../$1/*.c)

SRC = $(call GET_SRC,kernel/utils) $(call GET_SRC,kernel/utils/detail) ../kernel/Memory/memcpy.c #$(call GET_SRC,drivers/IDE)

C_OBJS = $(SRC:.c=.o)
OBJS = $(C_OBJS:.cpp=.o)

TESTS_SRC = $(wildcard *.cpp)
TESTS_SRC_C = $(wildcard *.c)
TESTS = $(TESTS_SRC:.cpp=.o) $(TESTS_SRC_C:.c=.o)

CXXFLAGS += -std=c++14 -MMD -I.. -fno-rtti -fno-exceptions -ffreestanding -lgcc -DDEBUG -DINCLUDED_NEW_HH

CFLAGS += -MMD -I.. -ffreestanding -lgcc


DEPS = $(OBJS:.o=.d) $(TESTS:.o=.d)

all: Test.out .fake
	

.fake: 
	



Test.out: $(OBJS) $(TESTS)
	@echo '$$SRC = ' $(SRC)
	echo $(OBJS) $(TESTS)
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<



-include $(DEPS)