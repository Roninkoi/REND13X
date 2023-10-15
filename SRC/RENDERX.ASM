code SEGMENT para public 'CODE'
ASSUME cs:code

.386

VSTART equ 0a000h
W equ 320
H equ 240

SCI equ 03c4h
SCD equ 03c5h
CRTI equ 03d4h
MISCOUT equ 03c2h

MAP_MASK equ 02h
ALL_PLANES equ 0f00h
PIX_PLANE equ 0100h

CRTHI equ 0ch
CRTLO equ 0dh

code ENDS
END
	
