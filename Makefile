#############################
#				GLOBAL SETTINGS			#
#############################
LIBRARY_NAME = cenv
PREFIX = /usr/local
INCLUDE_DIR = $(PREFIX)/include
HEADER = cenv.h
UNAME := $(shell uname)


#############################
#		COMPATIBILITY SETTINGS	#
#############################
ifeq ($(UNAME), Linux)
	INSTALL_CMD = cp
	RM_CMD = rm -f
	MKDIR_CMD = mkdir -p
endif

ifeq ($(UNAME), Darwin)
	INSTALL_CMD = cp
	RM_CMD = rm -f
	MKDIR_CMD = mkdir -p
endif

ifeq ($(OS), Windows_NT)
	INSTALL_CMD = copy
	RM_CMD = del /f /q
	MKDIR_CMD = mkdir
	INCLUDE_DIR = C:\Program Files\cenv\include
endif


#############################
#						RULES						#
#############################
install:
	@echo "Installing header file..."
	$(MKDIR_CMD) $(INCLUDE_DIR)
	$(INSTALL_CMD) $(HEADER) $(INCLUDE_DIR)

uninstall:
	@echo "Uninstalling header file..."
	$(RM_CMD) $(INCLUDE_DIR)\$(HEADER)

clean:
	@echo "Nothing to clean."

all: install