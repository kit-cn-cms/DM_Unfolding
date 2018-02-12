# TOPDIR := $(PWD)/
# SRCDIR := $(TOPDIR)\src
# BINDIR := $(TOPDIR)\bin

# SOURCES := $(shell find $(SRCDIR) -name "*.cpp" ) 
# OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

# CC := g++
# CFLAGS   := -Wall -std=c++17   `root-config --cflags` -I `root-config --incdir` `root-config --libs` 



# ALLFILES := $(SOURCES) // add headers and the like if required

# .PHONY: all

# all: Unfold

# Unfold: $(OBJECTS)
# 	$(CC) $(CFLAGS) $^ -o $(BINDIR)/$@


# clean:
# 	rm -f $(OBJECTS) $(EXE)`



# Compiler
CC = g++
OPTS =  -Wall -std=c++17  `root-config --cflags` -I `root-config --incdir` `root-config --glibs`   

# Project name
PROJECT = Unfold

# Directories
SRCDIR = src
TOPDIR := $(PWD)/
BINDIR := $(TOPDIR)\bin
OBJDIR = $(TOPDIR)\obj

# Libraries
LIBS = 

# Files and folders
SRCS    = $(shell find $(SRCDIR) -name '*.cpp')
SRCDIRS = $(shell find . -name '*.cpp' | dirname {} | sort | uniq | sed 's/\/$(SRCDIR)//g' )
OBJS    = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Targets
Unfold: buildrepo $(OBJS)
	$(CC)  $(OPTS) $(OBJS) $(LIBS) -o $(BINDIR)/$@


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(OPTS) -c $< -o   $@
	
clean:
	rm $(PROJECT) $(OBJDIR) -Rf
	
buildrepo:
	@$(call make-repo)

# Create obj directory structure
define make-repo
	mkdir -p $(OBJDIR)
	for dir in $(SRCDIRS); \
	do \
		mkdir -p $(OBJDIR)/$$dir; \
	done
endef