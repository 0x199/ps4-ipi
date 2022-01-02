SHELL := /bin/bash

# Package metadata.
TITLE       := Internal PKG Installer
VERSION     := 1.01
TITLE_ID    := BREW00050
CONTENT_ID  := IV0000-BREW00050_00-IPI0000000000000

# Libraries linked into the ELF.
LIBS        := -lc -lkernel -lc++ -lSceVideoOut -lSceBgft -lSceAppInstUtil -lSceSysmodule -lSceSystemService

# Additional compile flags.
EXTRAFLAGS  := -DGRAPHICS_USES_FONT -I../ps4-zip/src

# Asset and module directories.
ASSETS 		:= $(wildcard pkg/assets/**/*)
LIBMODULES  := $(wildcard pkg/sce_module/*)

# You likely won't need to touch anything below this point.

# Root vars
TOOLCHAIN   := $(OO_PS4_TOOLCHAIN)
# Need to make a symbolic link to point to Internal PKG Installer. 
# Make does not like spaces in directories.
PROJDIR     := Internal_PKG_Installer
COMMONDIR   := $(TOOLCHAIN)/samples/_common
INTDIR      := $(PROJDIR)/x64/Debug

# Define objects to build
CFILES      := $(wildcard $(PROJDIR)/*.c)
CPPFILES    := $(wildcard $(PROJDIR)/*.cpp)
COMMONFILES := $(wildcard $(COMMONDIR)/*.cpp)
OBJS        := $(patsubst $(PROJDIR)/%.c, $(INTDIR)/%.o, $(CFILES)) $(patsubst $(PROJDIR)/%.cpp, $(INTDIR)/%.o, $(CPPFILES)) 

# Define final C/C++ flags
CFLAGS      := -cc1 -triple x86_64-pc-freebsd-elf -munwind-tables -fuse-init-array -debug-info-kind=limited -debugger-tuning=sce -emit-obj $(EXTRAFLAGS) -isysroot $(TOOLCHAIN) -isystem $(TOOLCHAIN)/include
CXXFLAGS    := $(CFLAGS) -isystem $(TOOLCHAIN)/include/c++/v1 -I$(TOOLCHAIN)/src/lib
LDFLAGS     := -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x --eh-frame-hdr -L$(TOOLCHAIN)/lib -L../ps4-zip/ $(LIBS) $(TOOLCHAIN)/lib/crt1.o

# Check for linux vs macOS and account for clang/ld path
UNAME_S     := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
		CC      := clang
		CCX     := clang++
		LD      := ld.lld
		CDIR    := linux
endif
ifeq ($(UNAME_S),Darwin)
		CC      := /usr/local/opt/llvm/bin/clang
		CCX     := /usr/local/opt/llvm/bin/clang++
		LD      := /usr/local/opt/llvm/bin/ld.lld
		CDIR    := macos
endif

all: clean $(CONTENT_ID).pkg revert

pkg/eboot.bin: $(INTDIR) $(OBJS)
	$(LD) $(INTDIR)/*.o -o $(INTDIR)/$(PROJDIR).elf $(LDFLAGS)
	$(TOOLCHAIN)/bin/$(CDIR)/create-fself -eboot=pkg/eboot.bin -in=$(INTDIR)/$(PROJDIR).elf -out=$(INTDIR)/$(PROJDIR).oelf --paid 0x3800000000000010

$(CONTENT_ID).pkg: pkg/pkg.gp4
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core pkg_build $< .

pkg/pkg.gp4: pkg/eboot.bin pkg/sce_sys/about/right.sprx pkg/sce_sys/param.sfo pkg/sce_sys/icon0.png $(LIBMODULES) $(ASSETS)
	cd pkg && $(TOOLCHAIN)/bin/$(CDIR)/create-gp4 -out $(shell echo "$@" | sed -e 's/pkg\///g') --content-id=$(CONTENT_ID) --files "$(shell echo "$^" | sed -e 's/pkg\///g')"

pkg/sce_sys/param.sfo: Makefile
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_new $@
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ APP_TYPE --type Integer --maxsize 4 --value 1 
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ APP_VER --type Utf8 --maxsize 8 --value '$(VERSION)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ ATTRIBUTE --type Integer --maxsize 4 --value 0  
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ CATEGORY --type Utf8 --maxsize 4 --value 'gd'  
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ CONTENT_ID --type Utf8 --maxsize 48 --value '$(CONTENT_ID)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ DOWNLOAD_DATA_SIZE --type Integer --maxsize 4 --value 0 
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ SYSTEM_VER --type Integer --maxsize 4 --value 0  
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ TITLE --type Utf8 --maxsize 128 --value '$(TITLE)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ TITLE_ID --type Utf8 --maxsize 12 --value '$(TITLE_ID)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ VERSION --type Utf8 --maxsize 8 --value '$(VERSION)'

$(INTDIR)/%.o: $(PROJDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(INTDIR)/%.o: $(PROJDIR)/%.cpp
	$(CCX) $(CXXFLAGS) -o $@ $<

$(INTDIR)/%.o: $(COMMONDIR)/%.c
	$(CCX) $(CXXFLAGS) -o $@ $<

$(INTDIR)/%.o: $(COMMONDIR)/%.cpp
	$(CCX) $(CXXFLAGS) -o $@ $<


# Created intermediate directories after renaming
# No idea why intermediate directories are inside this directory

# clean and revert is a workaround to make not liking spaces in the directory name
clean:
	file "Internal PKG Installer" | grep -q "directory" && mv -u "Internal PKG Installer" Internal_PKG_Installer || echo "Not found" 
	mkdir -p $(INTDIR)
	rm -f $(CONTENT_ID).pkg pkg/pkg.gp4 pkg/sce_sys/param.sfo pkg/eboot.bin \
		$(INTDIR)/$(PROJDIR).elf $(INTDIR)/$(PROJDIR).oelf $(OBJS)

revert:
	file "Internal_PKG_Installer" | grep -q "directory" && mv -u Internal_PKG_Installer "Internal PKG Installer"
