# Microsoft Visual C++ generated build script - Do not modify

PROJ = REND13X
DEBUG = 0
PROGTYPE = 6
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\REND13X\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = DRAW.C      
FIRSTCPP =             
RC = rc
CFLAGS_D_DEXE = /nologo /W3 /FR /G2 /Zi /D_DEBUG /Od /AM /D_DOS /Fd"REND13.PDB"
CFLAGS_R_DEXE = /nologo /W3 /FR /G2 /DNDEBUG /Gs /Ox /AM /D_DOS
LFLAGS_D_DEXE = /NOLOGO /NOI /STACK:5120 /ONERROR:NOEXE /CO 
LFLAGS_R_DEXE = /NOLOGO /NOD /NOI /STACK:5120 /ONERROR:NOEXE 
LIBS_D_DEXE = oldnames mlibce 
LIBS_R_DEXE = oldnames mlibce 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = IN.OBJ RX.OBJ 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_DEXE)
LFLAGS = $(LFLAGS_D_DEXE)
LIBS = $(LIBS_D_DEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_DEXE)
LFLAGS = $(LFLAGS_R_DEXE)
LIBS = $(LIBS_R_DEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = DRAW.SBR \
		INPUT.SBR \
		MAIN.SBR \
		RENDER.SBR \
		RENDER13.SBR \
		RENDERX.SBR \
		VEC.SBR


IN_DEP = 

RX_DEP = 

DRAW_DEP = d:\rend13x\src\render.h \
	d:\rend13x\src\vec.h


INPUT_DEP = d:\rend13x\src\render.h \
	d:\rend13x\src\vec.h


MAIN_DEP = d:\rend13x\src\render.h \
	d:\rend13x\src\vec.h


RENDER_DEP = d:\rend13x\src\render.h \
	d:\rend13x\src\vec.h


RENDER13_DEP = d:\rend13x\src\render.h \
	d:\rend13x\src\vec.h


RENDERX_DEP = d:\rend13x\src\render.h \
	d:\rend13x\src\vec.h


VEC_DEP = d:\rend13x\src\vec.h


all:	$(PROJ).EXE $(PROJ).BSC

DRAW.OBJ:	SRC\DRAW.C $(DRAW_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c SRC\DRAW.C

INPUT.OBJ:	SRC\INPUT.C $(INPUT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC\INPUT.C

MAIN.OBJ:	SRC\MAIN.C $(MAIN_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC\MAIN.C

RENDER.OBJ:	SRC\RENDER.C $(RENDER_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC\RENDER.C

RENDER13.OBJ:	SRC\RENDER13.C $(RENDER13_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC\RENDER13.C

RENDERX.OBJ:	SRC\RENDERX.C $(RENDERX_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC\RENDERX.C

VEC.OBJ:	SRC\VEC.C $(VEC_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC\VEC.C

$(PROJ).EXE::	DRAW.OBJ INPUT.OBJ MAIN.OBJ RENDER.OBJ RENDER13.OBJ RENDERX.OBJ VEC.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
DRAW.OBJ +
INPUT.OBJ +
MAIN.OBJ +
RENDER.OBJ +
RENDER13.OBJ +
RENDERX.OBJ +
VEC.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
d:\rend13x\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
