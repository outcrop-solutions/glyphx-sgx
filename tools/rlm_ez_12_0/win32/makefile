#
#	RLM-ez Binary Kit makefile - Windows.
#
#

PLATFORM = x86_w3

EXECS = ezlicense.exe ezclient.exe

VER = 1203

DLL = rlmez$(VER).dll

DLLLIB = rlmez$(VER).lib

all: $(EXECS) $(DLL)

CC = cl

CFLAGS = /nologo

LD = link

LDFLAGS = /nologo /nodefaultlib /nxcompat

LIBRARY = rlmez.lib

RLMID1LIB = rlmid1.lib

!if "$(PLATFORM)" == "x86_w4" || "$(PLATFORM)" == "x64_w4"
XXTRALIB = libvcruntimed.lib libucrtd.lib
!else
XXTRALIB =
!endif

XTRALIB = $(RLMID1LIB) ws2_32.lib Advapi32.lib Gdi32.lib \
			User32.lib netapi32.lib kernel32.lib oldnames.lib \
			shell32.lib libcmt.lib $(XXTRALIB)

!if "$(PLATFORM)" == "x64_w2" || "$(PLATFORM)" == "x64_w3" || "$(PLATFORM)" == "x64_w4"
MACHINE = x64
!else
MACHINE = x86
!endif

OBJS = ezclient.obj

KEYOBJS = rlm_privkey.obj rlm_pubkey.obj

$(LIBRARY):	$(KEYOBJS)
	nmake /nologo $(KEYOBJS)
	lib /nologo $(LIBRARY) $(KEYOBJS)
	lib /nologo rlmez_md.lib $(KEYOBJS)
	lib /nologo rlmez_mdd.lib $(KEYOBJS)
	lib /nologo rlmez_mtd.lib $(KEYOBJS)

rlm_privkey.c:
	@echo ""
	@echo "******************************************************"
	@echo "Your public/private key pair does not exist in ../src"
	@echo "We will generate them for you now..."
	@echo ""
	@echo "If you have previously generated these key files,"
	@echo "place them in ../src now and re-run this make"
	@echo "******************************************************"
	@echo ""
	rlmgenkeys
	nmake /nologo rlm_pubkey.obj
	nmake /nologo rlm_privkey.obj

rlm_pubkey.o:	rlm_pubkey.c
	$(CC) $(CFLAGS) /c rlm_pubkey.c

rlm_privkey.o:	rlm_privkey.c
	$(CC) $(CFLAGS) /c rlm_privkey.c


ezclient.exe:	ezclient.obj $(LIBRARY)
	$(LD) $(LDFLAGS) /out:ezclient.exe ezclient.obj $(LIBRARY) $(XTRALIB)
	
ezlicense.exe:	$(LIBRARY)
	$(LD) $(LDFLAGS) /out:ezlicense.exe ezlicense.obj $(LIBRARY) $(XTRALIB)
	
$(DLL):	$(LIBRARY)
	copy rlmez.lib rlmeztemp.lib
	lib /nologo /remove:rlm_privkey.obj rlmeztemp.lib
	lib /nologo /remove:rlm_sign_license.obj rlmeztemp.lib
	link /nologo /ignore:4001 /ignore:4068 /machine:$(MACHINE) /dll \
		/out:$(DLL) /def:rlmez.def rlmeztemp.lib \
		/nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib $(XTRALIB)
	del rlmeztemp.lib
	
$(OBJS):	rlmez.h

clean:;
	if exist ezclient.obj del ezclient.obj
	if exist rlm_pubkey.obj del rlm_pubkey.obj
	if exist rlm_privkey.obj del rlm_privkey.obj
	if exist ezlicense.exe del ezlicense.exe
	if exist ezclient.exe del ezclient.exe
	if exist $(DLL) del $(DLL)
