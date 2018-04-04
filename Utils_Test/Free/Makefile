rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

GET_SRC_R = $(call rwildcard,$1/,*.$2)

GET_SRC = $(wildcard ../../$1/*.cpp ../../$1/*.c)

LASTPART = $(shell basename $1)
LASTPARTS = $(foreach x,$1,$(call LASTPART,$x))

SRC = $(call GET_SRC_R,../../Libraries,c) $(wildcard ../../kernel/CppSupport/*.cpp) $(wildcard ../Dummies/*.cpp) Mem.cpp $(call GET_SRC_R,../../Utils,cpp) $(call GET_SRC_R,../../Utils,c) ../../kernel/Error.c ../../drivers/VGA.cpp ../../kernel/MetaInfo.cpp ../../kernel/Filesystem/Filesystem.cpp ../../kernel/Memory/Memory.cpp ../../kernel/Memory/memcpy.cpp ../../kernel/Filesystem/API.cpp $(call GET_SRC_R,../../Support,cpp) $(call GET_SRC_R,../../kernel/Filesystem,cpp) $(call GET_SRC_R,../../kernel/ResourceHandles,cpp)


C_SRC = $(call GET_SRC_R,../../kernel,c) $(call GET_SRC,../../drivers,c) $(call GET_SRC_R,../../Libraries,c)

CPP_SRC = $(call GET_SRC_R,../../kernel,cpp) $(call GET_SRC_R,../../drivers,cpp) $(call GET_SRC_R,../../Libraries,cpp)


CPP_OBJS = $(SRC:.cpp=.o) #$(CPP_SRC:.cpp=.o) $(C_SRC:.c=.o)
OBJS = $(CPP_OBJS:.c=.o)

FREE_C_SRC = $(call GET_SRC_R,../../Std,c) $(call GET_SRC_R,../../Libraries,c)
FREE_CPP_SRC = $(call GET_SRC_R,../../Std,cpp) $(call GET_SRC_R,../../Libraries,cpp)
FREE_OBJS = $(FREE_C_SRC:.c=.o) $(FREE_CPP_SRC:.cpp=.o) $(C_OBJS) $(OBJS)
FREE_TESTS = SQLite_Free.o

BOTH_FLAGS += -DFREE_TESTING -ffreestanding -DSTACK_INFO_V -DDEBUG -I../../ -I../../Libraries  -D"weak_alias(name, aliasname)"="extern __typeof (name) aliasname __attribute__ ((weak, alias ( \#name )));" -D"attribute_hidden"="__attribute__((hidden))" -D"libc_hidden_proto(X)"="" -D"rtld_hidden_proto(X)"="" -D"weak_hidden_alias(name,aliasname)"="extern __typeof (name) aliasname __attribute__ ((weak, alias ( \#name )))  __attribute__((hidden));" -D"stub_warning(X)"="" -D"libc_hidden_builtin_def(x)"="" -D"libc_hidden_def(x)"="" 

BOTH_FLAGS += -ffreestanding
BOTH_FLAGS += -nostdlib
BOTH_FLAGS += -Wfatal-errors
BOTH_FLAGS += #-nostdinc
BOTH_FLAGS += #-DINCLUDED_DEBUG_H_ENABLED
BOTH_FLAGS += #-D"kassert(x)"="assert(x)"
BOTH_FLAGS += #-I../../Std

BOTH_FLAGS += $(SQLITE_FLAGS)

SQLITE_FLAGS += -DSQLITE_THREADSAFE=0
SQLITE_FLAGS += -DSQLITE_OMIT_LOAD_EXTENSION
SQLITE_FLAGS += -DSQLITE_OS_OTHER=1
SQLITE_FLAGS += -DSQLITE_TEMP_STORE=3

CXXFLAGS += -fpermissive -std=c++14 -MMD -I.. -fno-rtti -fno-exceptions -DDEBUG $(BOTH_FLAGS) -DINCLUDED_NEW_HH

CFLAGS += -MMD -I.. $(BOTH_FLAGS) 


FREEFLAGS += -I../..


DEPS = $(FREE_OBJS:.o=.d) $(FREE_TESTS:.o=.d) $(OBJS:.o=.d)


CXX_F = $(word 1,$(CXX))
CC_F = $(word 1,$(CC))



all: .fake FreeTests.out
	
	
	

.fake:
	@echo ' '
	@echo ' '
	@echo 'OBJS: ' $(call LASTPARTS,$(FREE_OBJS)) $(call LASTPARTS,$(FREE_TESTS))
	@echo ' '
	@echo ' '
	@echo ' '
	@echo ' '
	

FreeTests.out: $(FREE_OBJS) $(FREE_TESTS)
	@echo ' '
	@echo $(CXX_F) $(call LASTPARTS,$^) ' > ' $(call LASTPART,$@)
	@echo ' '
	@$(CXX) $(CXXFLAGS) $(FREEFLAGS) -o $@ $^ -lgcc


%.o: %.cpp ../ForceRebuild.cpp
	@echo ' '
	@echo $(CXX_F) $(call LASTPART,$<) ' > ' $(call LASTPART,$@)
	@$(CXX) $(CXXFLAGS) -o $@ -c $<
	@echo ' '

%.o: %.c ../ForceRebuild.cpp
	@echo ' '
	@echo $(CC_F) $(call LASTPART,$<) ' > ' $(call LASTPART,$@)
	@$(CC) $(CFLAGS) -o $@ -c $<
	@echo ' '



-include $(DEPS)