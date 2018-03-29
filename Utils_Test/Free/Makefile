rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

GET_SRC_R = $(call rwildcard,$1/,*.$2)

GET_SRC = $(wildcard ../../$1/*.cpp ../../$1/*.c)

SRC = $(call GET_SRC_R,../../Libraries,c) $(wildcard ../../kernel/CppSupport/*.cpp) $(wildcard ../Dummies/*.cpp) Mem.cpp $(call GET_SRC_R,../../Utils,cpp) $(call GET_SRC_R,../../Utils,c) ../../kernel/Error.c ../../drivers/VGA.cpp ../../kernel/MetaInfo.cpp ../../kernel/Filesystem/Filesystem.cpp ../../kernel/Memory/Memory.cpp ../../kernel/Memory/memcpy.cpp ../../kernel/Filesystem/API.cpp


C_SRC = $(call GET_SRC_R,../../kernel,c) $(call GET_SRC,../../drivers,c) $(call GET_SRC_R,../../Libraries,c)

CPP_SRC = $(call GET_SRC_R,../../kernel,cpp) $(call GET_SRC_R,../../drivers,cpp) $(call GET_SRC_R,../../Libraries,cpp)


CPP_OBJS = $(SRC:.cpp=.o) #$(CPP_SRC:.cpp=.o) $(C_SRC:.c=.o)
OBJS = $(CPP_OBJS:.c=.o)

FREE_C_SRC = $(call GET_SRC_R,../../Std,c) $(call GET_SRC_R,../../Libraries,c)
FREE_CPP_SRC = $(call GET_SRC_R,../../Std,cpp) $(call GET_SRC_R,../../Libraries,cpp)
FREE_OBJS = $(FREE_C_SRC:.c=.o) $(FREE_CPP_SRC:.cpp=.o) $(C_OBJS) $(OBJS)
FREE_TESTS = SQLite_Free.o

BOTH_FLAGS += -nostdlib -DSTACK_INFO_V -DDEBUG -I../../ -I../../Libraries -I../../Std -D"weak_alias(name, aliasname)"="extern __typeof (name) aliasname __attribute__ ((weak, alias ( \#name )));" -D"attribute_hidden"="__attribute__((hidden))" -D"libc_hidden_proto(X)"="" -D"rtld_hidden_proto(X)"="" -D"weak_hidden_alias(name,aliasname)"="extern __typeof (name) aliasname __attribute__ ((weak, alias ( \#name )))  __attribute__((hidden));" -D"stub_warning(X)"="" -D"libc_hidden_builtin_def(x)"="" -D"libc_hidden_def(x)"="" -fpermissive

BOTH_FLAGS += -Wfatal-errors
BOTH_FLAGS += #-nostdinc
BOTH_FLAGS += #-DINCLUDED_DEBUG_H_ENABLED
BOTH_FLAGS += #-D"kassert(x)"="assert(x)"

BOTH_FLAGS += $(SQLITE_FLAGS)

SQLITE_FLAGS += -DSQLITE_THREADSAFE=0

CXXFLAGS += -std=c++14 -MMD -I.. -fno-rtti -fno-exceptions -ffreestanding -DDEBUG $(BOTH_FLAGS) -DINCLUDED_NEW_HH

CFLAGS += -MMD -I.. -ffreestanding -lgcc $(BOTH_FLAGS) 


FREEFLAGS += -nostdlib -I../Std -I../..


DEPS = $(FREE_OBJS:.o=.d) $(FREE_TESTS:.o=.d) $(OBJS:.o=.d)

all: .fake FreeTests.out
	
	
	

.fake:
	echo $(OBJS)
	

FreeTests.out: $(FREE_OBJS) $(FREE_TESTS)
	$(CXX) $(CXXFLAGS) $(FREEFLAGS) -o $@ $^ -lgcc


%.o: %.cpp ForceRebuild.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o: %.c ForceRebuild.cpp
	$(CC) $(CFLAGS) -o $@ -c $<



-include $(DEPS)