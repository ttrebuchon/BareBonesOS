CC = i686-elf-gcc
CXX = i686-elf-g++
ASM = i686-elf-as


BOTH_FLAGS = -std=c++14 -fPIC -ffreestanding -nostdlib -fno-exceptions -fno-rtti -I../../

CXXFLAGS = -fPIC -ffreestanding -nostdlib

ELF_OUT = out.txt
DUMP_OUT = dump_out.txt

TARGET = libDynamic.so

all: $(TARGET) $(ELF_OUT) $(DUMP_OUT)


$(TARGET): Foo.o
	$(CXX) $^ $(BOTH_FLAGS) -shared -o $@ -nostdlib -export-dynamic -Wl,-soname,$@,-shared
	cp $(TARGET) ../Dummy_Img/
	

$(ELF_OUT): $(TARGET)
	readelf -a -g -n -t -e --dyn-syms -W $< > $@

$(DUMP_OUT): $(TARGET)
	objdump -a -f -h -x -D -S -g -t -T -r -R -w --special-syms $< > $@

Foo.o: Foo.cpp
	$(CXX) $^ $(BOTH_FLAGS) -c -shared -export-dynamic -o $@
	$(CXX) $^ $(BOTH_FLAGS) -S -shared -export-dynamic -o Foo.asm_out.s

clean:
	-rm -f $(TARGET)
	-rm -f *.o



