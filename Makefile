CC := gcc
CFLAGS := -g -Wall
LIBS := -lallegro
SOURCES := $(shell find src/ -type f -name "*.c")
OBJECTS := $(SOURCES:.c=.o)
TARGET := game
 
all: $(SOURCES) $(TARGET)
 
$(TARGET): $(OBJECTS)
	@echo " Linking..."; $(CC) $^ -o $@ $(LIBS)
 
%.o: %.c
	@echo " CC $<"; $(CC) $(CFLAGS) -c -o $@ $<
 
clean:
	@echo " Cleaning..."; $(RM) src/*.o $(TARGET)
 
.PHONY: all clean
