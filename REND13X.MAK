#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCCDOS  = Bcc +BccDos.cfg 
TLINK   = TLink
TLIB    = TLib
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGSDOS =  -LC:\BC45\LIB
IDE_BFLAGS = 
LLATDOS_DcbREND13Xbrend13xdexe =  -c -LC:\TC\LIB -Tde
RLATDOS_DcbREND13Xbrend13xdexe = 
BLATDOS_DcbREND13Xbrend13xdexe = 
CNIEAT_DcbREND13Xbrend13xdexe = -IC:\TC\INCLUDE -D
LNIEAT_DcbREND13Xbrend13xdexe = -x
LEAT_DcbREND13Xbrend13xdexe = $(LLATDOS_DcbREND13Xbrend13xdexe)
REAT_DcbREND13Xbrend13xdexe = $(RLATDOS_DcbREND13Xbrend13xdexe)
BEAT_DcbREND13Xbrend13xdexe = $(BLATDOS_DcbREND13Xbrend13xdexe)

#
# Dependency List
#
Dep_rend13x = \
   D:\REND13X\rend13x.exe

rend13x : BccDos.cfg $(Dep_rend13x)
  echo MakeNode 

Dep_DcbREND13Xbrend13xdexe = \
   in.obj\
   rx.obj\
   draw.obj\
   main.obj\
   render.obj\
   renderx.obj\
   render13.obj\
   vec.obj\
   input.obj

D:\REND13X\rend13x.exe : $(Dep_DcbREND13Xbrend13xdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_DcbREND13Xbrend13xdexe) $(LNIEAT_DcbREND13Xbrend13xdexe) +
C:\TC\LIB\c0l.obj+
in.obj+
rx.obj+
draw.obj+
main.obj+
render.obj+
renderx.obj+
render13.obj+
vec.obj+
input.obj
$<,$*
C:\TC\LIB\fp87.lib+
C:\TC\LIB\mathl.lib+
C:\TC\LIB\cl.lib

|

in.obj :  src\in.asm
  $(TASM) @&&|
 /ml D:\REND13X\src\in.asm ,in.obj
|

rx.obj :  src\rx.asm
  $(TASM) @&&|
 /ml D:\REND13X\src\rx.asm ,rx.obj
|

draw.obj :  src\draw.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_DcbREND13Xbrend13xdexe) $(CNIEAT_DcbREND13Xbrend13xdexe) -o$@ src\draw.c
|

main.obj :  src\main.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_DcbREND13Xbrend13xdexe) $(CNIEAT_DcbREND13Xbrend13xdexe) -o$@ src\main.c
|

render.obj :  src\render.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_DcbREND13Xbrend13xdexe) $(CNIEAT_DcbREND13Xbrend13xdexe) -o$@ src\render.c
|

renderx.obj :  src\renderx.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_DcbREND13Xbrend13xdexe) $(CNIEAT_DcbREND13Xbrend13xdexe) -o$@ src\renderx.c
|

render13.obj :  src\render13.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_DcbREND13Xbrend13xdexe) $(CNIEAT_DcbREND13Xbrend13xdexe) -o$@ src\render13.c
|

vec.obj :  src\vec.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_DcbREND13Xbrend13xdexe) $(CNIEAT_DcbREND13Xbrend13xdexe) -o$@ src\vec.c
|

input.obj :  src\input.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_DcbREND13Xbrend13xdexe) $(CNIEAT_DcbREND13Xbrend13xdexe) -o$@ src\input.c
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=rend13x.csm
-R-
-f287
-i32
-O
-ml
-3
| $@


