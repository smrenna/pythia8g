# Makefile is a part of the PYTHIA event generator.
# Copyright (C) 2014 Torbjorn Sjostrand.
# PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
# Please respect the MCnet Guidelines, see GUIDELINES for details.
# Author: Philip Ilten, August 2014.
#
# This is is the Makefile used to build PYTHIA on POSIX systems. Example usage 
# is:
#     make -j2
# For help using the make command please consult the local system documentation,
# i.e. "man make" or "make --help".

################################################################################
# VARIABLES: Definition of the relevant variables from the configuration script
# and the distribution structure.
################################################################################

# Include the configuration and set the local directory structure.
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
  -include Makefile.inc
endif
LOCAL_BIN=bin
LOCAL_DOCS=AUTHORS COPYING GUIDELINES examples/Makefile.inc README
LOCAL_INCLUDE=include
LOCAL_LIB=lib
LOCAL_SHARE=share/Pythia8
LOCAL_SRC=src
LOCAL_TMP=tmp
LOCAL_MKDIRS:=$(shell mkdir -p $(LOCAL_TMP) $(LOCAL_LIB))

# PYTHIA.
OBJECTS=$(patsubst $(LOCAL_SRC)/%.cc,$(LOCAL_TMP)/%.o, \
	$(wildcard $(LOCAL_SRC)/*.cc))
TARGETS=$(LOCAL_LIB)/libpythia8.a
ifeq ($(ENABLE_SHARED),true)
  TARGETS+=$(LOCAL_LIB)/libpythia8$(LIB_SUFFIX)
endif

# LHAPDF5.
TARGETS+=$(LOCAL_LIB)/libpythia8lhapdf5.a
ifeq ($(ENABLE_SHARED),true)
  TARGETS+=$(LOCAL_LIB)/libpythia8lhapdf5$(LIB_SUFFIX)
endif

################################################################################
# RULES: Definition of the rules used to build PYTHIA.
################################################################################

# Rules without physical targets (secondary expansion for documentation).
.SECONDEXPANSION:
.PHONY: all install clean distclean

# All targets.
all: $(TARGETS) $(addprefix $(LOCAL_SHARE)/, $(LOCAL_DOCS))

# The documentation.
$(addprefix $(LOCAL_SHARE)/, $(LOCAL_DOCS)): $$(notdir $$@)
	cp $^ $@

# The Makefile configuration.
Makefile.inc:
	./configure

# Auto-generated (with -MD flag) dependencies.
-include $(LOCAL_TMP)/*.d

# PYTHIA.
$(LOCAL_TMP)/%.o: $(LOCAL_SRC)/%.cc
ifeq ($(GZIP_USE),true)
	$(CXX) -MD $(CXX_COMMON) -I$(LOCAL_INCLUDE) -I$(BOOST_INCLUDE) \
	-DGZIPSUPPORT -c -o $@ $^
else
	$(CXX) -MD $(CXX_COMMON) -I$(LOCAL_INCLUDE) -c -o $@ $^
endif
$(LOCAL_LIB)/libpythia8.a: $(OBJECTS)
	ar cru $@ $^
$(LOCAL_LIB)/libpythia8$(LIB_SUFFIX): $(OBJECTS)
	$(CXX) $(CXX_COMMON) $(CXX_SHARED) -o $@ $^ $(CXX_SONAME),$(notdir $@)

# LHAPDF5.
$(LOCAL_TMP)/LHAPDF5.o: $(LOCAL_INCLUDE)/Pythia8Plugins/LHAPDF5.h
	$(CXX) -MD $(CXX_COMMON) -I$(LOCAL_INCLUDE) -x c++ -c -o $@ $^
$(LOCAL_LIB)/libpythia8lhapdf5.a: $(LOCAL_TMP)/LHAPDF5.o
	ar cru $@ $^
$(LOCAL_LIB)/libpythia8lhapdf5$(LIB_SUFFIX): $(LOCAL_TMP)/LHAPDF5.o
	$(CXX) $(CXX_COMMON) $(CXX_SHARED) -o $@ $^ $(CXX_SONAME),$(notdir $@)

# Install (rsync is used for finer control).
install: all
	mkdir -p $(PREFIX_BIN) $(PREFIX_INCLUDE) $(PREFIX_LIB) $(PREFIX_SHARE)
	rsync -a pythia8-config.in $(LOCAL_BIN)/
	rsync -a $(LOCAL_BIN)/* $(PREFIX_BIN) --exclude .svn
	rsync -a $(LOCAL_INCLUDE)/* $(PREFIX_INCLUDE) --exclude .svn
	rsync -a $(LOCAL_LIB)/* $(PREFIX_LIB) --exclude .svn
	rsync -a $(LOCAL_SHARE)/* $(PREFIX_SHARE) --exclude .svn

# Clean.
clean:
	rm -rf $(LOCAL_TMP) $(LOCAL_LIB)
	rm -f $(LOCAL_SHARE)/examples/*Dct.*
	rm -f $(LOCAL_SHARE)/examples/main[0-9][0-9]
	rm -f $(LOCAL_SHARE)/examples/out[0-9][0-9]
	rm -f $(LOCAL_SHARE)/examples/weakbosons.lhe 

# Clean all temporary and generated files.
distclean: clean
	find . -type f -name Makefile.inc -print0 | xargs -0 rm -f
	find . -type f -name "*~" -print0 | xargs -0 rm -f
	find . -type f -name "#*" -print0 | xargs -0 rm -f
	rm -f $(LOCAL_SHARE)/AUTHORS
	rm -f $(LOCAL_SHARE)/COPYING
	rm -f $(LOCAL_SHARE)/GUIDELINES
	rm -f $(LOCAL_SHARE)/README