
# Libraries to link against
LIBS=-ltidy -lboost_filesystem
LIBS+=$(shell pkg-config --libs libcurl libxml-2.0)

# Local include paths
INCLUDE=-I.

# Compilation flags
CFLAGS=-Wall
## example:
CFLAGS+=$(shell pkg-config --cflags libcurl libxml-2.0)

# Target binaries
PROGS=ekhem

# Source files' extension
EXT=cc

# Compiler
GCC=g++

######################################
# You shouldn't need to modify these #
######################################

CC=${GCC} ${INCLUDE} ${CFLAGS} 
LINK=${GCC}
GENDEP=${GCC} -MM ${INCLUDE} ${CFLAGS}

OBJS:=$(patsubst %.${EXT},%.o, $(wildcard *.${EXT}))
DEPS:=$(patsubst %.${EXT},.%.d, $(wildcard *.${EXT}))

.PHONY: all
all: ${PROGS}
	@ echo Build complete

.%.d: %.${EXT}
	@ echo DEP $<
	@ ${GENDEP} $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

${PROGS}: ${DEPS} ${OBJS}
	@ echo LINK $@
	@ ${LINK} ${OBJS} ${LIBS} ${LFLAGS} -o $@
	@ strip $@

${OBJS}: %.o: %.${EXT}
	@ echo CC $<
	@ ${CC} -c $<

REALDEPS:=$(wildcard .*.d)
ifneq ($(strip $(REALDEPS)),)
include ${REALDEPS}
endif

.PHONY: clean
clean:
	@ echo CLEAN ${PROGS}
	@ rm -f ${PROGS}

	@ echo CLEAN ${OBJS}
	@ rm -f ${OBJS}

	@ echo CLEAN ${DEPS}
	@ rm -f ${DEPS}

