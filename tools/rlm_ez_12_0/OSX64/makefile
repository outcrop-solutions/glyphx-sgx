#
#	RLM-ez Binary Kit makefile - Unix systems.
#
#
EXECS = ezlicense ezclient 

SHAREDEXT = jnilib

VER = 1203

SO = librlmez$(VER).$(SHAREDEXT)

JAR = rlmez$(VER).jar

all: $(EXECS) so

CFLAGS = -g -Dx64_m1 -m64 -fPIC

ARFLAGS = crs

ARXFLAGS = x

CC = cc

LD = $(CC)

LDFLAGS = -m64

LIBRARY = rlmez.a

NOSSL = rlmez_nossl.a

ALTLIBRARY = librlmez.a

XTRALIBRLMID = 

XTRALIB =  $(XTRALIBRLMID)

OBJS = ezclient.o

KEYOBJS = rlm_privkey.o rlm_pubkey.o 

SHAREDFLAG = -dynamiclib -m64 -framework JavaVM

SHAREDXTRALIB = -lgcc_s.1

JAVALD = cc

$(LIBRARY):	$(KEYOBJS)
	ar $(ARFLAGS) $(LIBRARY) $(KEYOBJS)
	@rm -f $(ALTLIBRARY)
	@ln -s $(LIBRARY) $(ALTLIBRARY)

$(NOSSL):	$(LIBRARY)
	mkdir _t
	cd _t; ar x ../$(LIBRARY); mkdir _t2; cp rlm*.o _t2; cd _t2; ar $(ARFLAGS) $(NOSSL) *.o; mv $(NOSSL) ../..
	ranlib $(NOSSL)
	rm -rf _t

rlm_privkey.c:
	@echo ""
	@echo "******************************************************"
	@echo "Your public/private key pair does not exist here."
	@echo "We will generate them for you now..."
	@echo ""
	@echo "If you have previously generated these key files,"
	@echo "place them here now and re-run this make"
	@echo "******************************************************"
	@echo ""
	./rlmgenkeys
	chmod -w rlm_pubkey.c rlm_privkey.c

rlm_pubkey.o:	rlm_pubkey.c
	$(CC) $(CFLAGS) -c rlm_pubkey.c

rlm_privkey.o:	rlm_privkey.c
	$(CC) $(CFLAGS) -c rlm_privkey.c

ezclient:	ezclient.o $(LIBRARY)
	$(LD) $(LDFLAGS) -o ezclient ezclient.o $(LIBRARY) $(XTRALIB)
	
ezlicense:	$(LIBRARY)
	$(LD) $(LDFLAGS) -o ezlicense ezlicense.o $(LIBRARY) $(XTRALIB)
	
shared:	$(SO)

so:	$(SO)

$(SO):	$(LIBRARY)
	rm -rf tmp
	mkdir tmp; \
	cd tmp; \
	mkdir r; \
	mkdir o;\
	cp ../$(LIBRARY) .; \
	ar x $(LIBRARY); \
	mv rlm_*.o r; \
	mv *.o o; \
	rm -f r/rlm_privkey.o; \
	rm -f r/rlm_genlic.o; \
	rm -f r/rlm_act_doit.o; \
	rm -f r/rlmgenkeys.o; \
	rm -f r/rlm_mklic.o; \
	rm -f r/rlmsign.o; \
	rm -f r/rlm_sign_license.o; \
	rm -f r/rlm_sign_lf.o; \
	rm -f r/rlmverify.o; \
	ar cr o.a o/*.o; \
	cd .. 
	$(JAVALD) -o $(SO) $(SHAREDFLAG) $(LDFLAGS) tmp/r/*.o tmp/o.a -lc $(XTRALIBRLMID) $(SHAREDXTRALIB)
	rm -rf tmp
	
$(OBJS):	rlmez.h

clean:;	rm -f $(OBJS) $(KEYOBJS) $(EXECS) $(ISV) $(SO) core $(ISV_XFER_OBJ)
