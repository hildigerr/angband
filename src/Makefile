

SRCS = \
  main.c misc1.c misc2.c store1.c arrays.c io.c birth.c desc.c\
  generate.c sets.c dungeon.c  command.ccreature.c death.c eat.c help.c magic.c\
  potions.c prayer.c save.c staffs.c wands.c scrolls.c spells.c cave.c\
  wizard.c store2.c signals.c moria1.c moria2.c monsters.c\
  treasure.c variable.c recall.c unix.c \
  player.c tables.c describe_mon.c rods.c random.c util.c z-util.c z-virt.c z-form.c

OBJS = \
  main.o misc1.o misc2.o store1.o arrays.o io.o birth.o desc.o\
  generate.o sets.o dungeon.o  command.o creature.o death.o eat.o help.o magic.o\
  potions.o prayer.o save.o staffs.o wands.o scrolls.o spells.o cave.o\
  wizard.o store2.o signals.o moria1.o moria2.o monsters.o\
  treasure.o variable.o recall.o unix.o \
  player.o tables.o describe_mon.o rods.o util.o random.o z-util.o z-virt.o z-form.o




####
# Please take a look at config.h and h-config.h for *critical* configuration steps needed.


####
# If you use gcc, you may find that adding '-traditional-cpp' will fix
# defining the LIBDIR in config.h.  '-Dconst' may also help if your compiler
# complains about conflicting definitions

CC = cc

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


default: angband

.c.o:
	$(CC) $(CFLAGS) -c $*.c

#
# Build the "Angband" program and move it up one directory
#

angband: $(OBJS)
	$(CC) $(CFLAGS)  -o angband $(OBJS) $(LIBS)
	mv angband ..


depend:
	/usr/local/bin/X11/makedepend $(INCLUDES) -p$(OBJDIR)/ $(SRCS)

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
