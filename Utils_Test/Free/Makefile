rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

GET_SRC_R = $(call rwildcard,$1/,*.$2)

GET_SRC = $(wildcard ../../$1/*.cpp ../../$1/*.c)

LASTPART = $(shell basename $1)
LASTPARTS = $(foreach x,$1,$(call LASTPART,$x))

#SRC += $(call GET_SRC_R,../../Libraries,c)
SRC += $(call GET_SRC_R,../../Std,c)
SRC += $(call GET_SRC_R,../../Std,cpp)
SRC += $(wildcard ../../kernel/CppSupport/*.cpp)
SRC += $(wildcard ../Dummies/*.cpp)
SRC += Mem.cpp
SRC += $(call GET_SRC_R,../../Utils,cpp)
SRC += $(call GET_SRC_R,../../Utils,c)
SRC += ../../kernel/Error.c
SRC += ../../drivers/VGA.cpp
SRC += ../../kernel/MetaInfo.cpp
SRC += ../../kernel/Filesystem/Filesystem.cpp
SRC += ../../kernel/Memory/Memory.cpp
SRC += ../../kernel/Memory/memcpy.cpp
SRC += ../../kernel/Filesystem/API.cpp
#SRC += $(call GET_SRC_R,../../Support,cpp)
SRC += $(call GET_SRC_R,../../kernel/Filesystem,cpp)
SRC += $(call GET_SRC_R,../../kernel/ResourceHandles,cpp)
SRC += $(call GET_SRC_R,../../kernel/Filesystem/Files,cpp)
SRC += ../../kernel/Memory/Heap.cpp
SRC += ../../kernel/Memory/Heaps/DumbHeap.cpp
SRC += ../../Drivers/CMOS.cpp
SRC += ../Dummies/PortIO.c
SRC += ../Dummies/sleep.c
SRC += ../Dummies/Interrupts.c
SRC += ../../kernel/Interrupts/Interrupts_cpp.cpp
SRC += ../../kernel/Interrupts/irq_guard.cpp
SRC += $(call GET_SRC_R,Dummies,cpp)
#SRC += $(call GET_SRC_R,../../Libraries/libcxxrt,cc)




C_SRC += $(call GET_SRC_R,../../kernel,c)
C_SRC += $(call GET_SRC,../../drivers,c)
C_SRC += $(call GET_SRC_R,../../Libraries,c)

CPP_SRC += $(call GET_SRC_R,../../kernel,cpp)
CPP_SRC += $(call GET_SRC_R,../../drivers,cpp)
CPP_SRC += $(call GET_SRC_R,../../Libraries,cpp)


_CPP_OBJS = $(SRC:.cpp=.o) #$(CPP_SRC:.cpp=.o) $(C_SRC:.c=.o)
CPP_OBJS = $(_CPP_OBJS:.cc=.o)
OBJS = $(CPP_OBJS:.c=.o)

FREE_C_SRC += $(call GET_SRC_R,../../Std,c)
FREE_C_SRC += $(call GET_SRC_R,../../Libraries,c)

FREE_CPP_SRC += $(call GET_SRC_R,../../Std,cpp)
FREE_CPP_SRC += $(call GET_SRC_R,../../Libraries,cpp)
FREE_OBJS = $(OBJS) #$(FREE_C_SRC:.c=.o) $(FREE_CPP_SRC:.cpp=.o) $(C_OBJS) $(OBJS)
FREE_TESTS = SQLite_Free.o

#BOTH_FLAGS += -D"Utils"="std"
BOTH_FLAGS += -DFREE_TESTING
BOTH_FLAGS += -ffreestanding
BOTH_FLAGS += -nostdlib -D__NOSTDLIB__
BOTH_FLAGS += -D__USE_STD_NEW__
BOTH_FLAGS += -DSTACK_INFO_V
BOTH_FLAGS += -DDEBUG
BOTH_FLAGS += -I../../
BOTH_FLAGS += -I../../Libraries
BOTH_FLAGS += -D"weak_alias(name, aliasname)"="extern __typeof (name) aliasname __attribute__ ((weak, alias ( \#name )));"
BOTH_FLAGS += -D"attribute_hidden"="__attribute__((hidden))"
BOTH_FLAGS += -D"libc_hidden_proto(X)"=""
BOTH_FLAGS += -D"rtld_hidden_proto(X)"=""
BOTH_FLAGS += -D"weak_hidden_alias(name,aliasname)"="extern __typeof (name) aliasname __attribute__ ((weak, alias ( \#name )))  __attribute__((hidden));"
BOTH_FLAGS += -D"stub_warning(X)"=""
BOTH_FLAGS += -D"libc_hidden_builtin_def(x)"=""
BOTH_FLAGS += -D"libc_hidden_def(x)"=""
#BOTH_FLAGS += -Wfatal-errors
BOTH_FLAGS += -fmax-errors=3
BOTH_FLAGS += #-nostdinc
BOTH_FLAGS += #-DINCLUDED_DEBUG_H_ENABLED
#BOTH_FLAGS += #-D"kassert(x)"="assert(x)"
BOTH_FLAGS += #-I../../Std

BOTH_FLAGS += $(SQLITE_FLAGS)

SQLITE_FLAGS += -DSQLITE_THREADSAFE=0
SQLITE_FLAGS += -DSQLITE_OMIT_LOAD_EXTENSION
SQLITE_FLAGS += -DSQLITE_OS_OTHER=1
SQLITE_FLAGS += -DSQLITE_TEMP_STORE=3

#CXXFLAGS += -fpermissive
CXXFLAGS += -std=c++14
CXXFLAGS += -MMD
CXXFLAGS += -I..
CXXFLAGS += -fno-rtti
CXXFLAGS += -fno-exceptions
CXXFLAGS += $(BOTH_FLAGS)
#CXXFLAGS += -DINCLUDED_NEW_HH



CFLAGS += -MMD
CFLAGS += -I..
CFLAGS += $(BOTH_FLAGS) 


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
	@echo 'CXXFLAGS: ' $(CXXFLAGS)
	@echo ' '
	@echo ' '
	@echo ' '
	@echo ' '
	

FreeTests.out: $(FREE_OBJS) $(FREE_TESTS)
	@echo ' '
	@echo $(CXX_F) $(call LASTPARTS,$^) ' > ' $(call LASTPART,$@)
	@echo ' '
	@$(CXX) $(CXXFLAGS) $(FREEFLAGS) -o $@ $^ -lgcc -ldl


%.o: %.cpp ../ForceRebuild.cpp
	@echo ' '
	@echo $(CXX_F) $(call LASTPART,$<) ' > ' $(call LASTPART,$@)
	@$(CXX) $(CXXFLAGS) -o $@ -c $<
	@echo ' '
	
%.o: %.cc ../ForceRebuild.cpp
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
-include ../Makefiles_Common