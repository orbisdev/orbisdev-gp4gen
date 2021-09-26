ifndef ORBISDEV
$(error ORBISDEV, is not set)
endif

# project name (generate executable with this name)
TARGET   = gp4gen

CC       = gcc
LINKER   = $(CXX)

# change these to proper directories where each file should be
SRCDIR   = src

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(SRCDIR)/%.o)
rm       = rm -f


$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(SRCDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

all: $(TARGET)

.PHONY: clean
clean:
	@$(rm) $(OBJECTS) $(TARGET)
	@echo "Cleanup complete!"


install:
	@cp $(TARGET) $(ORBISDEV)/bin
	@echo "$(TARGET) Installed!"