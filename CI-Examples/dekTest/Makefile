CPPFLAGS = -Wall -Wextra -DSEMANTIC_VERSION=\"1.1.1\"
CXXFLAGS=-g -std=c++17 -Wall -pedantic
ifeq ($(DEBUG),1)
GRAMINE_LOG_LEVEL = debug
CPPFLAGS += -g
else
GRAMINE_LOG_LEVEL = error
CPPFLAGS += -O3
endif
#LDFLAGS += -L/usr/lib/x86_64-linux-gnu/
CC := g++

.PHONY: all
all: dek dek.manifest
ifeq ($(SGX),1)
CPPFLAGS += -DSGX=1
all: dek.manifest.sgx dek.sig
endif

dek: dek.o

dek.o: dek.cpp

dek.manifest: dek.manifest.template
	gramine-manifest \
		-Dlog_level=$(GRAMINE_LOG_LEVEL) \
		$< $@

# gramine-sgx-sign generates both a .sig file and a .manifest.sgx file. This is somewhat
# hard to express properly in Make. The simple solution would be to use
# "Rules with Grouped Targets" (`&:`), however make on Ubuntu <= 20.04 doesn't support it.
#
# Simply using a normal rule with "two targets" is equivalent to creating separate rules
# for each of the targets, and when using `make -j`, this might cause two instances
# of gramine-sgx-sign to get launched simultaneously, potentially breaking the build.
#
# As a workaround, we use a dummy intermediate target, and mark both files as depending on it, to
# get the dependency graph we want. We mark this dummy target as .INTERMEDIATE, which means
# that make will consider the source tree up-to-date even if the sgx_sign file doesn't exist,
# as long as the other dependencies check out. This is in contrast to .PHONY, which would
# be rebuilt on every invocation of make.
dek.sig dek.manifest.sgx: sgx_sign
	@:

.INTERMEDIATE: sgx_sign
sgx_sign: dek.manifest dek
	gramine-sgx-sign \
		--manifest $< \
		--output $<.sgx

ifeq ($(SGX),)
GRAMINE = gramine-direct
else
GRAMINE = gramine-sgx
endif

.PHONY: check
check: all
	$(GRAMINE) dek > OUTPUT
	echo "Hello, world" | diff OUTPUT -
	@echo "[ Success ]"

.PHONY: clean
clean:
	$(RM) *.token *.sig *.manifest.sgx *.manifest dek.o dek OUTPUT

.PHONY: cleankey
cleankey:
	find key/ -type f -not -name 'stub' -delete
	find data/ -type f -not -name 'stub' -delete
#	$(RM) -v stub key/* data/*

.PHONY: distclean
distclean: clean
