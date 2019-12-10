makemore:=$(lastword $(MAKEFILE_LIST))
MAKEFLAGS+=--no-print-directory
ifeq ($(inside_makemore),)
inside_makemore:=yes
##
# debug tools
##
V=0
ifeq ($(V),1)
quiet=
Q=
else
quiet=quiet_
Q=@
endif
echo-cmd = $(if $($(quiet)cmd_$(1)), echo '  $($(quiet)cmd_$(1))';)
cmd = $(echo-cmd) $(cmd_$(1))

##
# file extention definition
bin-ext=
slib-ext=a
dlib-ext=so
makefile-ext=mk

##
# make file with targets definition
##
bin-y:=
sbin-y:=
lib-y:=
slib-y:=
modules-y:=
modules-y:=
data-y:=
hostbin-y:=

srcdir?=$(dir $(realpath $(firstword $(MAKEFILE_LIST))))
file?=$(notdir $(firstword $(MAKEFILE_LIST)))

#ifneq ($(findstring -arch,$(CFLAGS)),)
#ARCH=$(shell echo $(CFLAGS) 2>&1 | $(AWK) 'BEGIN {FS="[- ]"} {print $$2}')
#buildpath=$(join $(srcdir),$(ARCH))
#endif
ifneq ($(BUILDDIR),)
  builddir=$(BUILDDIR:%/=%)/
  buildpath:=$(if $(wildcard $(addprefix /.,$(builddir))),$(builddir),$(join $(srcdir),$(builddir)))
else
  builddir=$(srcdir)
endif

# CONFIG could define LD CC or/and CFLAGS
# CONFIG must be included before "Commands for build and link"
VERSIONFILE?=version
DEFCONFIG?=$(srcdir)defconfig

CONFIG?=.config
ifneq ($(wildcard $(builddir)$(CONFIG)),)
include $(builddir)$(CONFIG)
CONFIGFILE=$(buidldir)$(CONFIG)
$(eval NOCONFIGS:=$(shell awk '/^# .* is not set/{print $$2}' $(builddir)$(CONFIG)))
$(foreach config,$(NOCONFIGS),$(eval $(config)=n) )
endif

CONFIGURE_STATUS:=.config.cache
ifneq ($(wildcard $(builddir)$(CONFIGURE_STATUS)),)
include $(builddir)$(CONFIGURE_STATUS)
CONFIGFILE=$(buidldir)$(CONFIGURE_STATUS)
endif

ifneq ($(file),)
  include $(file)
endif

ifneq ($(buildpath),)
  obj=$(addprefix $(buildpath),$(cwdir))
else
  ifneq ($(CROSS_COMPILE),)
	buildpath:=$(builddir)$(CROSS_COMPILE:%-=%)/
    obj:=$(addprefix $(buildpath),$(cwdir))
  else
    obj=
  endif
endif
hostobj:=$(builddir)host/$(cwdir)

PATH:=$(value PATH):$(hostobj)
TMPDIR:=/tmp
TESTFILE:=makemore_test
##
# default Macros for installation
##
# not set variable if not into the build step
AWK?=awk
GREP?=grep
RM?=rm -f
INSTALL?=install
INSTALL_PROGRAM?=$(INSTALL) -D
INSTALL_DATA?=$(INSTALL) -m 644 -D
PKGCONFIG?=pkg-config
YACC?=bison
MOC?=moc$(QT:%=-%)
UIC?=uic$(QT:%=-%)

TOOLCHAIN?=
CROSS_COMPILE?=
CC?=gcc
CFLAGS?=
CXX?=g++
CXXFLAGS?=
LD?=gcc
LDFLAGS?=
AR?=ar
RANLIB?=ranlib
HOSTCC?=$(CC)
HOSTCXX?=$(CXX)
HOSTLD?=$(LD)
HOSTAR?=$(AR)
HOSTRANLIB?=$(RANLIB)
HOSTCFLAGS?=$(CFLAGS)
HOSTLDFLAGS?=$(LDFLAGS)

export PATH:=$(PATH):$(TOOLCHAIN):$(TOOLCHAIN)/bin
# if cc is a link on gcc, prefer to use directly gcc for ld
ifeq ($(CC),cc)
 TARGETCC:=gcc
else
 TARGETCC:=$(CC)
endif
TARGETLD:=$(LD)
TARGETAS:=$(AS)
TARGETCXX:=$(CXX)
TARGETAR:=$(AR)
TARGETRANLIB:=$(RANLIB)

CCVERSION:=$(shell $(TARGETCC) -v 2>&1)
ifneq ($(dir $(TARGETCC)),./)
	TARGETPREFIX=
else
	ifneq ($(CROSS_COMPILE),)
		ifeq ($(findstring $(CROSS_COMPILE),$(TARGETCC)),)
			TARGETPREFIX=$(CROSS_COMPILE:%-=%)-
		endif
	else
		TARGETPREFIX=
	endif
endif
TARGETCC:=$(TARGETPREFIX)$(TARGETCC)
TARGETLD:=$(TARGETPREFIX)$(LD)
TARGETAS:=$(TARGETPREFIX)$(AS)
TARGETCXX:=$(TARGETPREFIX)$(CXX)
TARGETAR:=$(TARGETPREFIX)$(AR)
TARGETRANLIB:=$(TARGETPREFIX)$(RANLIB)

ARCH?=$(shell LANG=C $(TARGETCC) -v 2>&1 | $(GREP) Target | $(AWK) 'BEGIN {FS="[- ]"} {print $$2}')
libsuffix=$(findstring 64,$(ARCH))

ifneq ($(PREFIX),)
prefix:=$(PREFIX)
endif
prefix?=/usr/local
prefix:=$(prefix:"%"=%)
exec_prefix?=$(prefix)
program_prefix?=
library_prefix?=lib
bindir?=$(exec_prefix)/bin
bindir:=$(bindir:"%"=%)
sbindir?=$(exec_prefix)/sbin
sbindir:=$(sbindir:"%"=%)
libdir?=$(word 1,$(wildcard $(exec_prefix)/lib$(libsuffix) $(exec_prefix)/lib))
libdir:=$(if $(libdir), $(libdir),$(exec_prefix)/lib)
libdir:=$(libdir:"%"=%)
sysconfdir?=$(prefix)/etc
sysconfdir:=$(sysconfdir:"%"=%)
includedir?=$(prefix)/include
includedir:=$(includedir:"%"=%)
datadir?=$(prefix)/share/$(package:"%"=%)
datadir:=$(datadir:"%"=%)
pkgdatadir?=$(datadir:"%"=%)
pkglibdir?=$(libdir)/$(package:"%"=%)
pkglibdir:=$(pkglibdir:"%"=%)

ifneq ($(SYSROOT),)
sysroot:=$(patsubst "%",%,$(SYSROOT:%/=%)/)
TARGETPATHPREFIX=$(sysroot)
SYSROOT_CFLAGS+=--sysroot=$(sysroot)
SYSROOT_CFLAGS+=-isysroot $(sysroot)
SYSROOT_LDFLAGS+=--sysroot=$(sysroot)
else
sysroot:=
TARGETPATHPREFIX= =
endif

#CFLAGS+=$(foreach macro,$(DIRECTORIES_LIST),-D$(macro)=\"$($(macro))\")
LIBRARY+=
LDFLAGS+=

ifneq ($(strip $(includedir)),)
SYSROOT_CFLAGS+=-I$(TARGETPATHPREFIX)$(strip $(includedir))
ifneq ($(DESTDIR),)
SYSROOT_CFLAGS+=-I$(DESTDIR)$(strip $(includedir))
endif
endif
ifneq ($(strip $(libdir)),)
RPATHFLAGS+=-Wl,-rpath,$(strip $(libdir))
SYSROOT_LDFLAGS+=-L$(TARGETPATHPREFIX)$(strip $(libdir))
ifneq ($(DESTDIR),)
SYSROOT_LDFLAGS+=-L$(DESTDIR)$(strip $(libdir))
endif
endif
ifneq ($(strip $(pkglibdir)),)
RPATHFLAGS+=-Wl,-rpath,$(strip $(pkglibdir))
SYSROOT_LDFLAGS+=-L$(TARGETPATHPREFIX)$(strip $(pkglibdir))
ifneq ($(DESTDIR),)
SYSROOT_LDFLAGS+=-L$(DESTDIR)$(strip $(pkglibdir))
endif
endif

INTERN_CFLAGS+=-I.
INTERN_CXXFLAGS+=-I.
INTERN_LDFLAGS+=-L.
ifneq ($(obj),)
INTERN_LDFLAGS+=-L$(obj)
endif
ifneq ($(hostobj),)
INTERN_LDFLAGS+=-L$(hostobj)
endif
ifneq ($(src),)
INTERN_CFLAGS+=-I$(src)
INTERN_CXXFLAGS+=-I$(src)
endif
INTERN_CFLAGS+=-include $(builddir)$(VERSIONFILE:%=%.h)
ifneq ($(wildcard $(builddir)config.h),)
INTERN_CFLAGS+=-include $(builddir)config.h
endif

export package version prefix bindir sbindir libdir includedir datadir pkglibdir srcdir builddir sysconfdir

##
# objects recipes generation
##
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y), $(eval $(t)_SOURCES+=$(patsubst %.hpp,%.moc.cpp,$($(t)_QTHEADERS) $($(t)_QTHEADERS-y))))
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y), $(if $(findstring .cpp, $(notdir $($(t)_SOURCES))), $(eval $(t)_LIBRARY+=stdc++)))

$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y), $(eval $(t)-objs+=$(patsubst %.s,%.o,$(patsubst %.S,%.o,$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$($(t)_SOURCES) $($(t)_SOURCES-y)))))))
target-objs:=$(foreach t, $(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y), $(if $($(t)-objs), $(addprefix $(obj),$($(t)-objs)), $(obj)$(t).o))

$(foreach t,$(hostbin-y), $(eval $(t)-objs:=$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$($(t)_SOURCES) $($(t)_SOURCES-y)))))
$(foreach t,$(hostslib-y), $(eval $(t)-objs:=$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$($(t)_SOURCES) $($(t)_SOURCES-y)))))
target-hostobjs:=$(foreach t, $(hostbin-y) $(hostslib-y), $(if $($(t)-objs), $(addprefix $(hostobj)/,$($(t)-objs)), $(hostobj)/$(t).o))

$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$(foreach s, $($(t)_SOURCES) $($(t)_SOURCES-y),$(eval $(t)_LIBS+=$($(s:%.c=%)_LIBS)) ))
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$(foreach s, $($(t)_SOURCES) $($(t)_SOURCES-y),$(eval $(t)_LIBS+=$($(s:%.cpp=%)_LIBS)) ))

$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$(foreach s, $($(t)_SOURCES) $($(t)_SOURCES-y),$(eval $(t)_LIBRARY+=$($(s:%.c=%)_LIBRARY)) ))
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$(foreach s, $($(t)_SOURCES) $($(t)_SOURCES-y),$(eval $(t)_LIBRARY+=$($(s:%.cpp=%)_LIBRARY)) ))

$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y) $(hostbin-y),$(eval $(t)_CFLAGS+=$($(t)_CFLAGS-y)))
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y) $(hostbin-y),$(eval $(t)_CXXFLAGS+=$($(t)_CXXFLAGS-y)))
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y) $(hostbin-y),$(eval $(t)_LDFLAGS+=$($(t)_LDFLAGS-y)))
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y) $(hostbin-y),$(eval $(t)_LIBS+=$($(t)_LIBS-y)))
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y) $(hostbin-y),$(eval $(t)_LIBRARY+=$($(t)_LIBRARY-y)))

$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y) $(hostbin-y),$(eval $(t)_CFLAGS+=$(INTERN_CFLAGS)))
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y) $(hostbin-y),$(eval $(t)_LDFLAGS+=$(INTERN_LDFLAGS)))

$(foreach t,$(slib-y) $(lib-y),$(eval include-y+=$($(t)_HEADERS)))

# LIBRARY contains libraries name to check
# The name may terminate with {<version>} informations like LIBRARY+=usb{1.0}
# Here the commands remove the informations and store the name into LIBS
# After LIBS contains all libraries name to link
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$(foreach l, $($(t)_LIBRARY),$(eval $(t)_LIBS+=$(firstword $(subst {, ,$(subst },,$(l)))) ) ))
$(foreach l, $(LIBRARY),$(eval LIBS+=$(firstword $(subst {, ,$(subst },,$(l)))) ) )

$(foreach l, $(LIBS),$(eval CFLAGS+=$(shell $(PKGCONFIG) --cflags lib$(l) 2> /dev/null) ) )
$(foreach l, $(LIBS),$(eval LDFLAGS+=$(shell $(PKGCONFIG) --libs-only-L lib$(l) 2> /dev/null) ) )
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$(foreach l, $($(t)_LIBS),$(eval $(t)_CFLAGS+=$(shell $(PKGCONFIG) --cflags lib$(l) 2> /dev/null))))
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$(foreach l, $($(t)_LIBS),$(eval $(t)_LDFLAGS+=$(shell $(PKGCONFIG) --libs-only-L lib$(l) 2> /dev/null) ) ))

# set the CFLAGS of each source file
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$(foreach s, $($(t)_SOURCES) $($(t)_SOURCES-y),$(eval $(s:%.c=%)_CFLAGS+=$($(t)_CFLAGS)) ))
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$(foreach s, $($(t)_SOURCES) $($(t)_SOURCES-y),$(eval $(s:%.cpp=%)_CFLAGS+=$($(t)_CFLAGS)) ))

$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$(foreach s, $($(t)_SOURCES) $($(t)_SOURCES-y),$(eval $(t)_LDFLAGS+=$($(s:%.c=%)_LDFLAGS)) ))
$(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$(foreach s, $($(t)_SOURCES) $($(t)_SOURCES-y),$(eval $(t)_LDFLAGS+=$($(s:%.cpp=%)_LDFLAGS)) ))

# The Dynamic_Loader library (libdl) allows to load external libraries.
# If this libraries has to link to the binary functions,
# this binary has to export the symbol with -rdynamic flag
$(foreach t,$(bin-y) $(sbin-y),$(if $(findstring dl, $($(t)_LIBS) $(LIBS)),$(eval $(t)_LDFLAGS+=-rdynamic)))

##
# targets recipes generation
##
ifeq (STATIC,y)
lib-static-target:=$(addprefix $(obj),$(addsuffix $(slib-ext:%=.%),$(addprefix $(library_prefix),$(slib-y) $(lib-y))))
else
lib-static-target:=$(addprefix $(obj),$(addsuffix $(slib-ext:%=.%),$(addprefix $(library_prefix),$(slib-y))))
lib-dynamic-target:=$(addprefix $(obj),$(addsuffix $(dlib-ext:%=.%),$(addprefix $(library_prefix),$(lib-y))))
endif
modules-target:=$(addprefix $(obj),$(addsuffix $(dlib-ext:%=.%),$(modules-y)))
bin-target:=$(addprefix $(obj),$(addprefix $(program_prefix),$(addsuffix $(bin-ext:%=.%),$(bin-y) $(sbin-y))))
hostslib-target:=$(addprefix $(hostobj),$(addsuffix $(slib-ext:%=.%),$(addprefix lib,$(hostslib-y))))
hostbin-target:=$(addprefix $(hostobj),$(addsuffix $(bin-ext:%=.%),$(hostbin-y)))

#create subproject
$(foreach t,$(subdir-y),$(eval $(t)_CONFIGURE+=$($(t)_CONFIGURE-y)))
$(foreach t,$(subdir-y),$(if $($(t)_CONFIGURE), $(eval subdir-project+=$(t))))
subdir-y:=$(filter-out $(subdir-project),$(subdir-y))

#dispatch from subdir-y to directory paths and makefile paths
subdir-dir:=$(foreach dir,$(subdir-y),$(filter-out %$(makefile-ext:%=.%), $(filter-out %Makefile, $(dir))))
subdir-files:=$(foreach dir,$(subdir-y),$(filter %$(makefile-ext:%=.%),$(dir)) $(filter %Makefile, $(dir)))

#target each Makefile in directories
subdir-target:=$(wildcard $(addsuffix /Makefile,$(subdir-dir:%/.=%)))
subdir-target+=$(wildcard $(subdir-files))

#download-target+=$(foreach dl,$(download-y),$(DL)/$(dl)/$($(dl)_SOURCE))
$(foreach dl,$(download-y),$(if $(findstring git,$($(dl)_SITE_METHOD)),$(eval gitclone-target+=$(dl)),$(eval download-target+=$(dl))))

objdir:=$(sort $(dir $(target-objs)))

hostobjdir:=$(sort $(dir $(target-hostobjs)))

targets:=
targets+=$(lib-dynamic-target)
targets+=$(modules-target)
targets+=$(lib-static-target)
targets+=$(bin-target)

ifneq ($(CROSS_COMPILE),)
DESTDIR?=$(sysroot:"%"=%)
endif
##
# install recipes generation
##
sysconf-install:=$(addprefix $(DESTDIR)$(sysconfdir:%/=%)/,$(sysconf-y))
data-install:=$(addprefix $(DESTDIR)$(datadir:%/=%)/,$(data-y))
include-install:=$(addprefix $(DESTDIR)$(includedir:%/=%)/,$(include-y))
lib-static-install:=$(addprefix $(DESTDIR)$(libdir:%/=%)/,$(addsuffix $(slib-ext:%=.%),$(addprefix lib,$(slib-y))))
lib-dynamic-install:=$(addprefix $(DESTDIR)$(libdir:%/=%)/,$(addsuffix $(version:%=.%),$(addsuffix $(dlib-ext:%=.%),$(addprefix lib,$(lib-y)))))
modules-install:=$(addprefix $(DESTDIR)$(pkglibdir:%/=%)/,$(addsuffix $(dlib-ext:%=.%),$(modules-y)))
bin-install:=$(addprefix $(DESTDIR)$(bindir:%/=%)/,$(addprefix $(program_prefix),$(addsuffix $(bin-ext:%=.%),$(bin-y))))
sbin-install:=$(addprefix $(DESTDIR)$(sbindir:%/=%)/,$(addprefix $(program_prefix),$(addsuffix $(bin-ext:%=.%),$(sbin-y))))

DEVINSTALL?=y
install:=
dev-install-y:=
dev-install-$(DEVINSTALL)+=$(lib-static-install)
install+=$(lib-dynamic-install)
install+=$(modules-install)
install+=$(data-install)
install+=$(sysconf-install)
dev-install-$(DEVINSTALL)+=$(include-install)
install+=$(bin-install)
install+=$(sbin-install)

##
# main entries
##
action:=_build
build:=$(action) -f $(srcdir)$(makemore) file
.DEFAULT_GOAL:=_entry
.PHONY:_entry _build _install _clean _distclean _check _hostbuild
_entry: _configbuild _versionbuild default_action

_info:
	@:

_hostbuild: action:=_hostbuild
_hostbuild: build:=$(action) -f $(srcdir)$(makemore) file
_hostbuild: _info $(subdir-target) $(hostobjdir) $(hostslib-target) $(hostbin-target)
	@:

_configbuild: $(obj) $(if $(wildcard $(CONFIGFILE)),$(join $(builddir),config.h))
_versionbuild: $(if $(package) $(version), $(join $(builddir),$(VERSIONFILE:%=%.h)))

_build: _info $(download-target) $(gitclone-target) $(objdir) $(subdir-project) $(subdir-target) $(data-y) $(targets)
	@:

_install: action:=_install
_install: build:=$(action) -f $(srcdir)$(makemore) file
_install: _info $(install) $(dev-install-y) $(subdir-target)
	@:

_clean: action:=_clean
_clean: build:=$(action) -f $(srcdir)$(makemore) file
_clean: $(subdir-target) _clean_objs
	$(Q)$(call cmd,clean,$(wildcard $(targets)))
	$(Q)$(call cmd,clean,$(wildcard $(hostslib-target) $(hostbin-target)))

_clean_objs:
	$(Q)$(call cmd,clean,$(wildcard $(target-objs)) $(wildcard $(target-hostobjs)))

_distclean: action:=_distclean
_distclean: build:=$(action) -f $(srcdir)$(makemore) file
_distclean: $(subdir-target) _clean
	$(Q)$(call cmd,clean_dir,$(filter-out $(src),$(obj)))

_check: action:=_check
_check: build:=$(action) -s -f $(srcdir)$(makemore) file
_check: $(subdir-target) $(LIBRARY) $(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$($(t)_LIBRARY))

PHONY:clean distclean install check default_action pc all
clean: action:=_clean
clean: build:=$(action) -f $(srcdir)$(makemore) file
clean: default_action

distclean: action:=_distclean
distclean: build:=$(action) -f $(srcdir)$(makemore) file
distclean: default_action
distclean:
	$(Q)$(call cmd,clean_dir,$(wildcard $(buildpath:%=%/)host))
	$(Q)$(call cmd,clean_dir,$(wildcard $(gitclone-target)))
	$(Q)$(call cmd,clean,$(wildcard $(download-target)))
	$(Q)$(call cmd,clean,$(wildcard $(builddir)$(CONFIG)))
	$(Q)$(call cmd,clean,$(wildcard $(join $(builddir),$(CONFIG:.%=%.h))))
	$(Q)$(call cmd,clean,$(wildcard $(join $(builddir),$(VERSIONFILE:%=%.h))))

install: action:=_install
install: build:=$(action) -f $(srcdir)$(makemore) file
install: _configbuild _versionbuild default_action

check: action:=_check
check: build:=$(action) -s -f $(srcdir)$(makemore) file
check: $(.DEFAULT_GOAL)

hosttools: action:=_hostbuild
hosttools: build:=$(action) -f $(srcdir)$(makemore) file
hosttools: default_action

default_action: _info
	$(Q)$(MAKE) $(build)=$(file)
	@:

pc: $(builddir)$(package:%=%.pc)

all: _configbuild _versionbuild default_action

PHONY: menuconfig gconfig xconfig config oldconfig
menuconfig gconfig xconfig config:
	$(EDITOR) $(builddir)$(CONFIG)


defconfig: $(builddir)$(CONFIG).old cleanconfig FORCE
	@echo "  "DEFCONFIG $*
	@$(if $(DEFCONFIG),$(GREP) -v "^#" $(DEFCONFIG) > $(builddir)$(CONFIG))

%_defconfig: $(builddir)$(CONFIG).old cleanconfig FORCE
	@echo "  "DEFCONFIG $*
	$(if $(firstword $(wildcard $@ $(srcdir)/configs/$@ $(srcdir)/$@)),,$(error $*_defconfig not found))
	$(eval DEFCONFIG:=$(firstword $(wildcard $@ $(srcdir)/configs/$@ $(srcdir)/$@)))
	@$(if $(DEFCONFIG),$(GREP) -v "^#" $(DEFCONFIG) > $(builddir)$(CONFIG))

CONFIGS:=$(shell cat $(DEFCONFIG) | sed 's/\"/\\\"/g' | grep -v '^\#' | awk -F= 't$$1 != t {print $$1}' )
oldconfig: $(DEFCONFIG) FORCE
	@$(eval CONFIGS=$(foreach config,$(CONFIGS),$(if $($(config)),,$(config))))
	@$(if $(CONFIGS),cat $(DEFCONFIG) | grep $(addprefix -e ,$(CONFIGS)), echo "") >> $(builddir)$(CONFIG)

cleanconfig: $(if $(wildcard $(builddir)$(CONFIG)),distclean)

$(builddir)$(CONFIG).old: $(wildcard $(builddir)$(CONFIG))
	@$(if $<,mv $< $@)

$(builddir)$(CONFIG):
	$(warning "Configure the project first")
	$(warning "  make <...>_defconfig")
	$(warning "  make defconfig")
	$(warning "  make config")
	$(error  )

$(builddir)config.h: $(CONFIGFILE) $(builddir)$(CONFIG)
	@echo "  "CONFIG $*
	@echo '#ifndef __CONFIG_H__' > $@
	@echo '#define __CONFIG_H__' >> $@
	@echo '' >> $@
	@$(GREP) -v "^#" $< | $(AWK) -F= 't$$1 != t {if ($$2 != "n") print "#define "toupper($$1)" "$$2}' >> $@
	@echo '' >> $@
	@$(if $(pkglibdir), sed -i -e "/\\<PKGLIBDIR\\>/d" $@; echo '#define PKGLIBDIR "'$(pkglibdir)'"' >> $@)
	@$(if $(datadir), sed -i -e "/\\<DATADIR\\>/d" $@; echo '#define DATADIR "'$(datadir)'"' >> $@)
	@$(if $(pkgdatadir), sed -i -e "/\\<PKG_DATADIR\\>/d" $@; echo '#define PKG_DATADIR "'$(pkgdatadir)'"' >> $@)
	@$(if $(sysconfdir), sed -i -e "/\\<SYSCONFDIR\\>/d" $@; echo '#define SYSCONFDIR "'$(sysconfdir)'"' >> $@)
	@echo '#endif' >> $@

$(builddir)$(VERSIONFILE:%=%.h): $(builddir)$(CONFIG)
	@echo "  "VERSION $*
	@echo '#ifndef __VERSION_H__' > $@
	@echo '#define __VERSION_H__' >> $@
	@echo '' >> $@
	@$(if $(version), echo '#define VERSION "'$(version)'"' >> $@)
	@$(if $(package), echo '#define PACKAGE "'$(package)'"' >> $@)
	@$(if $(version), echo '#define PACKAGE_VERSION "'$(version)'"' >> $@)
	@$(if $(package), echo '#define PACKAGE_NAME "'$(package)'"' >> $@)
	@$(if $(package), echo '#define PACKAGE_TARNAME "'$(subst " ","_",$(package))'"' >> $@)
	@$(if $(package), echo '#define PACKAGE_STRING "'$(package) $(version)'"' >> $@)
	@echo '#endif' >> $@

##
# Commands for clean
##
quiet_cmd_clean=$(if $(2),CLEAN  $(notdir $(2)))
 cmd_clean=$(if $(2),$(RM) $(2))
quiet_cmd_clean_dir=$(if $(2),CLEAN $(notdir $(2)))
 cmd_clean_dir=$(if $(2),$(RM) -r $(2))
##
# Commands for build and link
##
RPATH=$(wildcard $(addsuffix /.,$(wildcard $(CURDIR:%/=%)/* $(obj)*)))
quiet_cmd_yacc_y=YACC $*
 cmd_yacc_y=$(YACC) -o $@ $<
quiet_cmd_as_o_s=AS $*
 cmd_as_o_s=$(TARGETAS) $(ASFLAGS) $($*_CFLAGS) $(SYSROOT_CFLAGS) -c -o $@ $<
quiet_cmd_cc_o_c=CC $*
 cmd_cc_o_c=$(TARGETCC) $(CFLAGS) $($*_CFLAGS) $(SYSROOT_CFLAGS) -c -o $@ $<
quiet_cmd_cc_o_cpp=CXX $*
 cmd_cc_o_cpp=$(TARGETCXX) $(CXXFLAGS) $(CFLAGS) $($*_CXXFLAGS) $($*_CFLAGS) $(SYSROOT_CFLAGS) -c -o $@ $<
quiet_cmd_moc_hpp=QTMOC $*
 cmd_moc_hpp=$(MOC) $(INCLUDES) $($*_MOCFLAGS) $($*_MOCFLAGS-y) -o $@ $<
quiet_cmd_uic_hpp=QTUIC $*
 cmd_uic_hpp=$(UIC) $< > $@
quiet_cmd_ld_bin=LD $*
 cmd_ld_bin=$(TARGETCC) -L. $($*_LDFLAGS) $(LDFLAGS) $(SYSROOT_LDFLAGS) $(RPATHFLAGS) -o $@ $^ -Wl,--start-group $(LIBS:%=-l%) $($*_LIBS:%=-l%) -Wl,--end-group -lc
quiet_cmd_ld_slib=LD $*
 cmd_ld_slib=$(RM) $@ && \
	$(TARGETAR) -cvq $@ $^ > /dev/null && \
	$(TARGETRANLIB) $@
quiet_cmd_ld_dlib=LD $*
 cmd_ld_dlib=$(TARGETCC) $($*_LDFLAGS) $(LDFLAGS) $(SYSROOT_LDFLAGS) $(RPATHFLAGS) -Bdynamic -shared -Wl,-soname,$(strip $(notdir $@)) -o $@ $^ $(addprefix -L,$(RPATH)) $(LIBS:%=-l%) $($*_LIBS:%=-l%) -lc

quiet_cmd_hostcc_o_c=HOSTCC $*
 cmd_hostcc_o_c=$(HOSTCC) $(HOSTCFLAGS) $($*_CFLAGS) -c -o $@ $<
quiet_hostcmd_cc_o_cpp=HOSTCXX $*
 cmd_hostcc_o_cpp=$(HOSTCXX) $(HOSTCXXFLAGS) $($*_CFLAGS) -c -o $@ $<
quiet_cmd_hostld_bin=HOSTLD $*
 cmd_hostld_bin=$(HOSTCC) -o $@ $^ $($*_LDFLAGS) $(HOSTLDFLAGS) -L. $(LIBS:%=-l%) $($*_LIBS:%=-l%)
quiet_cmd_hostld_slib=HOSTLD $*
 cmd_hostld_slib=$(RM) $@ && \
	$(HOSTAR) -cvq $@ $^ > /dev/null && \
	$(HOSTRANLIB) $@

quiet_cmd_check_lib=CHECK $*
define cmd_check_lib
	$(RM) $(TMPDIR)/$(TESTFILE:%=%.c) $(TMPDIR)/$(TESTFILE)
	echo "int main(){}" > $(TMPDIR)/$(TESTFILE:%=%.c)
	$(TARGETCC) -c -o $(TMPDIR)/$(TESTFILE:%=%.o) $(TMPDIR)/$(TESTFILE:%=%.c) $(INTERN_CFLAGS) $(CFLAGS) > /dev/null 2>&1
	$(TARGETLD) -o $(TMPDIR)/$(TESTFILE) $(TMPDIR)/$(TESTFILE:%=%.o) $(INTERN_LDFLAGS) $(LDFLAGS) $(addprefix -l, $2) > /dev/null 2>&1
endef

checkoption:=--exact-version
prepare_check=$(if $(filter %-, $2),$(eval checkoption:=--atleast-version),$(if $(filter -%, $2),$(eval checkoption:=--max-version)))
cmd_check2_lib=$(if $(findstring $(3:%-=%), $3),$(if $(findstring $(3:-%=%), $3),,$(eval checkoption:=--atleast-version),$(eval checkoption:=--max-version))) \
	$(PKGCONFIG) --print-errors $(checkoption) $(subst -,,$3) lib$2

##
# build rules
##
.SECONDEXPANSION:
$(hostobjdir) $(objdir) $(buildpath):
	$(Q)mkdir -p $@

$(obj)%.tab.c:%.y
	@$(call cmd,yacc_y)

$(obj)%.o:%.s
	@$(call cmd,as_o_s)

$(obj)%.o:%.c
	@$(call cmd,cc_o_c)

$(obj)%.o:%.cpp
	@$(call cmd,cc_o_cpp)

$(obj)%.moc.cpp:$(obj)%.ui.hpp
$(obj)%.moc.cpp:%.hpp
	@$(call cmd,moc_hpp)

$(obj)%.ui.hpp:%.ui
	@$(call cmd,uic_hpp)

$(hostobj)%.o:%.c
	@$(call cmd,hostcc_o_c)

$(hostobj)%.o:%.cpp
	@$(call cmd,hostcc_o_cpp)

$(lib-static-target): $(obj)lib%$(slib-ext:%=.%): $$(if $$(%-objs), $$(addprefix $(obj),$$(%-objs)), $(obj)%.o)
	@$(call cmd,ld_slib)

$(lib-dynamic-target): CFLAGS+=-fPIC
$(lib-dynamic-target): $(obj)lib%$(dlib-ext:%=.%): $$(if $$(%-objs), $$(addprefix $(obj),$$(%-objs)), $(obj)%.o)
	@$(call cmd,ld_dlib)

$(modules-target): CFLAGS+=-fPIC
$(modules-target): $(obj)%$(dlib-ext:%=.%): $$(if $$(%-objs), $$(addprefix $(obj),$$(%-objs)), $(obj)%.o)
	@$(call cmd,ld_dlib)

#$(bin-target): $(obj)/%$(bin-ext:%=.%): $$(if $$(%_SOURCES), $$(addprefix $(src)/,$$(%_SOURCES)), $(src)/%.c)
$(bin-target): $(obj)%$(bin-ext:%=.%): $$(if $$(%-objs), $$(addprefix $(obj),$$(%-objs)), $(obj)%.o)
	@$(call cmd,ld_bin)

$(hostbin-target): $(hostobj)%$(bin-ext:%=.%): $$(if $$(%-objs), $$(addprefix $(hostobj),$$(%-objs)), $(hostobj)%.o)
	@$(call cmd,hostld_bin)

$(hostslib-target): $(hostobj)lib%$(slib-ext:%=.%): $$(if $$(%-objs), $$(addprefix $(hostobj),$$(%-objs)), $(hostobj)%.o)
	@$(call cmd,hostld_slib)

.PHONY: $(subdir-project) $(subdir-target) FORCE
$(subdir-project): %: FORCE
	$(Q)echo "  "PROJECT $*
	$(Q)cd $* && $($*_CONFIGURE)
	$(Q)$(MAKE) -C $*
	$(Q)$(MAKE) -C $* install

$(subdir-target): %: FORCE
	$(Q)echo "  "SUBDIR $*
	$(Q)$(MAKE) -C $(dir $*) cwdir=$(cwdir)$(dir $*) builddir=$(builddir) $(build)=$(notdir $*)

$(LIBRARY) $(sort $(foreach t,$(slib-y) $(lib-y) $(bin-y) $(sbin-y) $(modules-y),$($(t)_LIBRARY))): %:
	@$(RM) $(TMPDIR)/$(TESTFILE:%=%.c) $(TMPDIR)/$(TESTFILE)
	@echo "int main(){}" > $(TMPDIR)/$(TESTFILE:%=%.c)
	@$(call cmd,check_lib,$(firstword $(subst {, ,$(subst },,$@))))
	@$(call prepare_check,$(lastword $(subst {, ,$(subst },,$@))))
	@$(if $(findstring $(words $(subst {, ,$(subst },,$@))),2),$(call cmd,check2_lib,$(firstword $(subst {, ,$(subst },,$@))),$(lastword $(subst {, ,$(subst },,$@)))))

##
# Commands for install
##
quiet_cmd_install_data=INSTALL $*
define cmd_install_data
	$(INSTALL_DATA) $< $@
endef
quiet_cmd_install_bin=INSTALL $*
define cmd_install_bin
	$(INSTALL_PROGRAM) $< $@
endef
quiet_cmd_install_link=INSTALL $*
define cmd_install_link
	$(LN) -t $(dirname $@) $(basename $<) $(basename $@)
endef

##
# install rules
##
$(foreach dir, includedir datadir sysconfdir libdir bindir sbindir ,$(DESTDIR)$($(dir))/):
	$(Q)mkdir -p $@

$(include-install): $(DESTDIR)$(includedir:%/=%)/%: %
	@$(call cmd,install_data)
	@$(foreach a,$($*_ALIAS) $($*_ALIAS-y), $(shell cd $(DESTDIR)$(includedir) && rm -f $(a) && ln -s $(includedir:%/=%)/$* $(a)))

$(sysconf-install): $(DESTDIR)$(sysconfdir:%/=%)/%: %
	@$(call cmd,install_data)
	@$(foreach a,$($*_ALIAS) $($*_ALIAS-y), $(shell cd $(DESTDIR)$(sysconfdir) && rm -f $(a) && ln -s $(sysconfdir:%/=%)/$* $(a)))

$(data-install): $(DESTDIR)$(datadir:%/=%)/%: %
	@$(call cmd,install_data)
	@$(foreach a,$($*_ALIAS) $($*_ALIAS-y), $(shell cd $(DESTDIR)$(datadir) && rm -f $(a) && ln -s $(datadir:%/=%)/$* $(a)))

$(lib-static-install): $(DESTDIR)$(libdir:%/=%)/lib%$(slib-ext:%=.%): $(obj)lib%$(slib-ext:%=.%)
	@$(call cmd,install_bin)
	@$(foreach a,$($*_ALIAS) $($*_ALIAS-y), $(shell cd $(DESTDIR)$(libdir) && rm -f $(a) && ln -s (libdir:%/=%)/lib$*$(slib-ext:%=.%) $(a)))

$(lib-dynamic-install): $(DESTDIR)$(libdir:%/=%)/lib%$(dlib-ext:%=.%)$(version:%=.%): $(DESTDIR)$(libdir)/
$(lib-dynamic-install): $(DESTDIR)$(libdir:%/=%)/lib%$(dlib-ext:%=.%)$(version:%=.%): $(obj)lib%$(dlib-ext:%=.%)
	@$(call cmd,install_bin)
	@$(if $(version),$(shell cd $(DESTDIR)$(libdir) && rm -f lib$*$(dlib-ext:%=.%) && ln -s lib$*$(dlib-ext:%=.%)$(version:%=.%) lib$*$(dlib-ext:%=.%)))
	@$(foreach a,$($*_ALIAS) $($*_ALIAS-y), $(shell cd $(DESTDIR)$(libdir) && rm -f $(a) && ln -s $(libdir:%/=%)/lib$*$(dlib-ext:%=.%) $(a)))

$(modules-install): $(DESTDIR)$(pkglibdir:%/=%)/%$(dlib-ext:%=.%): $(obj)%$(dlib-ext:%=.%)
	@$(call cmd,install_bin)
	@$(foreach a,$($*_ALIAS) $($*_ALIAS-y), $(shell cd $(DESTDIR)$(pkglibdir) && rm -f $(a) && ln -s $(pkglibdir:%/=%)/$*$(dlib-ext:%=.%) $(a)))

$(bin-install): $(DESTDIR)$(bindir:%/=%)/%$(bin-ext:%=.%): $(obj)%$(bin-ext:%=.%)
	@$(call cmd,install_bin)
	@$(foreach a,$($*_ALIAS) $($*_ALIAS-y), $(shell cd $(DESTDIR)$(bindir) && rm -f $(a) && ln -s $(bindir:%/=%)/$*$(bin-ext:%=.%) $(a)))

$(sbin-install): $(DESTDIR)$(sbindir:%/=%)/%$(bin-ext:%=.%): $(obj)%$(bin-ext:%=.%)
	@$(call cmd,install_bin)
	@$(foreach a,$($*_ALIAS) $($*_ALIAS-y), $(shell cd $(DESTDIR)$(sbindir) && rm -f $(a) && ln -s $(sbindir:%/=%)/$*$(bin-ext:%=.%) $(a)))

##
# Commands for download
##
DL?=$(srcdir)/.dl

quiet_cmd_download=DOWNLOAD $*
define cmd_download
	wget -q -O $(OUTPUT) $(URL)
endef

quiet_cmd_gitclone=CLONE $*
define cmd_gitclone
	$(if $(wildcard $(OUTPUT)),,git clone --depth 1 $(URL) $(VERSION) $(OUTPUT))
endef

$(DL)/:
	mkdir -p $@

$(download-target): %: $(DL)/
	$(eval URL=$($*_SITE)/$($*_SOURCE))
	$(eval DL=$(realpath $(DL)))
	$(eval OUTPUT=$(DL)/$($*_SOURCE))
	@$(call cmd,download)
	@$(if $(findstring .zip, $($*_SOURCE)),unzip -o -d $(cwdir)/$* $(OUTPUT))
	@$(if $(findstring .tar.gz, $($*_SOURCE)),tar -xzf $(OUTPUT) -C $(cwdir)/$*)

$(gitclone-target): %:
	$(eval URL=$($*_SITE))
	$(eval OUTPUT=$(cwdir)/$*)
	$(eval VERSION=$(if $($*_VERSION),-b $($*_VERSION)))
	@$(call cmd,gitclone)

#if inside makemore
endif
