# File: Makefile

##
## Note that you may have to make modifications below according
## to your machine, available libraries, compilation options,
## etc...
##


SRCS = \
  main.c main-gcu.c main-ncu.c main-cur.c \
  signals.c util.c io.c arrays.c save.c death.c \
  generate.c birth.c creature.c effects.c magic.c dungeon.c command.c \
  misc1.c misc2.c misc3.c misc4.c moria1.c moria2.c moria3.c moria4.c \
  spells1.c spells2.c cave.c desc.c tables.c variable.c \
  store.c recall.c wizard.c \
  sets.c eat.c help.c monsters.c player.c \
  potions.c staffs.c wands.c scrolls.c rods.c \
  random.c z-util.c z-virt.c z-form.c

OBJS = \
  main.o main-gcu.o main-ncu.o main-cur.o \
  signals.o util.o io.o arrays.o save.o death.o \
  generate.o birth.o creature.o effects.o magic.o dungeon.o command.o \
  misc1.o misc2.o misc3.o misc4.o moria1.o moria2.o moria3.o moria4.o \
  spells1.o spells2.o cave.o desc.o tables.o variable.o \
  store.o recall.o wizard.o \
  sets.o eat.o help.o monsters.o player.o \
  potions.o staffs.o wands.o scrolls.o rods.o \
  random.o z-util.o z-virt.o z-form.o




###
### Following are some "system" definitions
###
### See also "config.h" and "h-config.h".
###


#
# This is my compiler of choice, it seems to work most everywhere
#
CC = gcc

####
# Compiler flags
####
# '-O' means optimize.  '-g' is for debugging.
#
# If you are using gcc, you may combine both '-O' and '-g'.  Also:
# '-pipe' uses pipes rather than temporary files (which is faster).
# '-pg' enables profiling (for coders only).  '-W<something>' provides
# additional warnings.
####
# Remove one of the '#' from the list of compiler flag choices below:
####

# For normal compilers (ie, 'cc' on anything but a NeXT).
# Once you are sure it works, you can and should use '-O' instead.
# CFLAGS = -Wall -O1

# For HPUX, the compiler flags for 'cc' should be:
#CFLAGS = -Ae -Dunix -g

####
# For gcc (or NeXT) users only.
 
# Normal gcc users:
# For SunOS 5.X (Solaris 2.X), add '-DSOLARIS'
CFLAGS = -g -O1 -pipe

# Full optimization (but only when you know it works right!):
#CFLAGS = -O4 -pipe -fomit-frame-pointer

# Developers:
#CFLAGS = -g -O1 -pipe -Wall -Wcast-align -Wpointer-arith -Wwrite-strings

# Anal developers:     [ :-) ]
#CFLAGS = -g -O1 -pipe -Wall -Wcast-align -Wpointer-arith -Wwrite-strings \
 -Wshadow -Wcast-align -Waggregate-return 

# Profiling:
#CFLAGS = -pg -g -O1 -pipe-Wall -Wcast-align -Wpointer-arith -Wwrite-strings

####
# Libraries to link from
####
# some people might be able to (or even might have to) remove '-lrpcsvc'

LIBS = -lcurses -ltermcap -lrpcsvc

####
# For Linux systems, this line should look like:
# LIBS = -lncurses -ltermcap -lbsd

####
# For NCR System 3000, you'll probably need this:
# LIBS=-lcurses -ltermcap -lmalloc -L/usr/ucblib -lucb

####
# For SunOS 5.X (Solaris 2.X), this line should look like:
# LIBS = -ltermlib -ltermcap -lrpcsvc -lnsl -lucb

# For SunOS 5.X (Solaris 2.X), this line should look like:
# ....unless you don't have the BSD/SunOS 4.x compatibility kit:
# LIBS = -ltermlib -ltermcap -lrpcsvc -lnsl

# end of user configurable stuff
#######



#
# Build the "Angband" program and move it up one directory
#

angband: $(OBJS)
	$(CC) $(CFLAGS)  -o angband $(OBJS) $(LIBS)
	mv angband ..

#
# Generate dependancies automatically (not required)
#

depend:
	makedepend -D__MAKEDEPEND__ $(SRCS)

####
# verify that the monsters.c and describe_mon.c files correspond

check:
	@/bin/rm -f temp1 temp2
	@egrep '^{' monsters.c | awk -F\" '{print $$2}' > temp1
	@egrep '^{' describe_mon.c | awk -F\" '{print $$2}' > temp2
	@echo Monster name problems present:
	@diff -c1 temp1 temp2

query:
	@echo "Monsters.c:"
	egrep '^{"' monsters.c | wc -l
	@echo "Describe_Mon.c:"
	egrep '^{"' describe_mon.c | wc -l


# DO NOT DELETE THIS LINE -- make depend depends on it.

