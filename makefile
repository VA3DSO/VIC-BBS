# Makefile for VIC-BBS

CC      = cl65
TARGET  = vic20
CFG     = bbs.cfg
CFLAGS  = -t $(TARGET) --config $(CFG) -Cl -O -DEXP8K

# Output binaries
BBS     = bbs.prg
GAMES   = games.prg
EDITOR  = editor.prg
FILES   = files.prg
SYSOP   = sysop.prg
#RSA     = rsa.prg
#RSAB    = rsab.prg

# Default target
all: $(BBS) $(GAMES) $(EDITOR) $(FILES) $(SYSOP)
	@echo "All BBS programs built."
	c1541 bbs.dhd < inst.txt

$(BBS): bbs.c common.c uutils.c futils.c
	$(CC) $(CFLAGS) -o $@ bbs.c common.c uutils.c futils.c

$(GAMES): games.c common.c
	$(CC) $(CFLAGS) -o $@ games.c common.c

$(EDITOR): editor.c common.c futils.c
	$(CC) $(CFLAGS) -o $@ editor.c common.c futils.c

$(FILES): files.c common.c futils.c
	$(CC) $(CFLAGS) -o $@ files.c common.c futils.c

$(SYSOP): sysop.c common.c uutils.c futils.c
	$(CC) $(CFLAGS) -o $@ sysop.c common.c uutils.c futils.c

# Uncomment and adapt when RSA is ready
#$(RSA): rsa.c sm_common.c
#	$(CC) $(CFLAGS) -o $@ rsa.c sm_common.c
#
#$(RSAB): rsab.c
#	$(CC) $(CFLAGS) -o $@ rsab.c

clean:
	rm -f *.o *.prg
	@echo "Clean complete."