CC = i686-elf-gcc
CXX = i686-elf-g++
ASM = i686-elf-as


BOTH_FLAGS = -std=c++14 -fPIC -ffreestanding -nostdlib -fno-exceptions -fno-rtti

CXXFLAGS = -fPIC -ffreestanding -nostdlib

TARGET = libDynamic.so

all: $(TARGET)


$(TARGET): Foo.o
	$(CXX) $^ $(BOTH_FLAGS) -shared -o $@ -nostdlib -export-dynamic -Wl,-soname,$@,-shared
	cp $(TARGET) ../Dummy_Img/

Foo.o: Foo.cpp
	$(CXX) $^ $(BOTH_FLAGS) -c -shared -export-dynamic -o $@
	$(CXX) $^ $(BOTH_FLAGS) -S -shared -export-dynamic -o Foo.asm_out.s

clean:
	-rm -f $(TARGET)
	-rm -f *.o



