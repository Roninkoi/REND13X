.model small
.data
public _crtparam
_crtparam label word
	dw 00d06h ;; vert tot
	dw 03e07h ;; overflow
	dw 04109h ;; cell height
	dw 0ea10h ;; vsync start
	dw 0ac11h ;; vsync end
	dw 0df12h ;; display vert
	dw 00014h ;; no dword
	dw 0e715h ;; vblank start
	dw 00616h ;; vblank end
	dw 0e317h ;; byte mode

public _pgoffs
_pgoffs dw 0 ;; page offset

code SEGMENT para public 'CODE'
ASSUME cs:code

.386

VSTART equ 0a000h
W equ 320
H equ 240

ALPHA equ 5

SCI equ 003c4h
SCD equ 003c5h
GCI equ 003ceh
CRTI equ 003d4h
MISCOUT equ 003c2h

MAP_MASK equ 00002h
READ_MAP equ 00004h
ALL_PLANES equ 00f00h
PIX_PLANE equ 00100h

CRTHI equ 0000ch
CRTLO equ 0000dh

SH equ 15
MASK_LO equ 7fffh
MASK_HI equ 0ffff8000h
MASK_ALL equ 0ffffffffh

;; int r_init13()
public _r_init13
_r_init13 PROC FAR
	mov ah, 0fh
	int 10h
	xor dh, dh
	mov dl, al ;; get current video mode

	xor ah, ah
	mov al, 13h ;; set mode 13h
	int 10h
	
	mov ax, dx ;; return old video mode
	ret
_r_init13 ENDP

;;int r_init()
public _r_init
_r_init PROC FAR
	call _r_init13
	push ax
	push si
	push di
	
	mov dx, SCI ;; sequence controller
	mov ax, 0604h
	out dx, ax ;; no chain 4
	mov ax, 0100h
	out dx, ax ;; sync reset
	mov dx, MISCOUT ;; misc output
	mov al, 0e3h
	out dx, al ;; 25 MHz clock, 60 Hz scan
	mov dx, SCI
	mov ax, 0300h
	out dx, ax ;; restart sequencer

	mov dx, CRTI ;; CRT controller
	mov al, 11h ;; vsync end reg contains write
	out dx, al

	inc dx ;; CRT controller data register
	in al, dx ;; get vsync end reg
	and al, 7fh ;; no write protect
	out dx, al

	mov dx, CRTI
	cld
	mov si, offset _crtparam
	mov cx, 10 ;; length of crtparam
	
crtp: ;; set parameters from _crtparam
	lodsw
	out dx, ax
	loop crtp
	
	pop di
	pop si
	pop ax
	ret
_r_init ENDP

;; void r_exit(int vmode)
;; sp+4 vmode
public _r_exit
_r_exit PROC FAR
	push bp
	;; bp+6 vmode
	mov bp, sp
	
	mov dl, [bp+6] ;; vmode
	xor ah, ah
	xor bx, bx
	mov al, dl ;; switch to original video mode
	int 10h

	pop bp
	ret
_r_exit ENDP

;; void r_waitretrace()
public _r_waitretrace
_r_waitretrace PROC FAR
	cli
twaits: ;; trace start
	mov dx, 3dah
	in al, dx
	and al, 8
	cmp al, 0
	jg twaits
	
twaite: ;; trace end
	mov dx, 3dah
	in al, dx
	and al, 8
	cmp al, 0
	jle twaite
	
	sti
	ret
_r_waitretrace ENDP

;; int r_page(int pg)
;; sp+4 pg
public _r_page
_r_page PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 pg

	mov ax, W*H/4
	mov cx, [bp+6]
	mul cx
	mov _pgoffs, ax ;; pgoffs = W*H/4 * pg

	mov dx, CRTI

	mov cx, _pgoffs
	and cx, 0ff00h
	or cx, CRTHI
	mov ax, cx ;; hi = CRTHI | (pgo & 0xff00);

	out dx, ax

	mov cx, _pgoffs
	shl cx, 8
	or cx, CRTLO
	mov ax, cx ;; lo = CRTLO | (pgo << 8);

	out dx, ax
	
	pop bp
	sti
	ret
_r_page ENDP

;; int r_flip(int pg)
;; sp+4 pg
public _r_flip
_r_flip PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 pg
ftwaits:
	mov dx, 3dah ;; trace start
	in al, dx
	and al, 8
	cmp al, 0
	jg ftwaits

	mov dx, CRTI

	mov cx, _pgoffs
	and cx, 0ff00h
	or cx, CRTHI
	mov ax, cx ;; hi = CRTHI | (pgo & 0xff00);

	out dx, ax

	mov cx, _pgoffs
	shl cx, 8
	or cx, CRTLO
	mov ax, cx ;; lo = CRTLO | (pgo << 8);

	out dx, ax
	
ftwaite:
	mov dx, 3dah ;; trace end
	in al, dx
	and al, 8
	cmp al, 0
	jle ftwaite

	mov dx, 0 ;; pg = 0
	mov _pgoffs, 0 ;; pgoffs = 0

	mov ax, [bp+6] ;; pg
	cmp ax, 0
	jg pge ;; pg > 0?

	mov dx, 1 ;; pg = 1
	mov ax, W*H/4
	mov _pgoffs, ax ;; pgoffs = W*H/4
	
pge:
	mov ax, dx ;; return pg

	pop bp
	sti
	ret
_r_flip ENDP

;; void r_putpixel(int x, int y, byte c)
;; sp+4 x
;; sp+6 y
;; sp+8 c
public _r_putpixel
_r_putpixel PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x
	;; bp+8 y
	;; bp+10 c
	push di

	mov ax, W/4
	mov cx, [bp+8] ;; y
	mul cx
	mov di, [bp+6] ;; x
	shr di, 2
	add di, ax ;; calculate address x>>2 + y * W/4
	add di, _pgoffs ;; page offset
	
	mov ax, VSTART ;; video memory start
	mov es, ax

	mov cx, [bp+6] ;; x
	and cx, 3 ;; plane of pixel
	mov ax, PIX_PLANE ;; select pixel plane
	add ax, MAP_MASK
	shl ah, cl
	mov dx, SCI
	out dx, ax

	mov al, [bp+10] ;; color
	mov es:[di], al ;; put pixel

	pop di
	pop bp
	sti
	ret
_r_putpixel ENDP

;; void r_fill(byte c)
;; sp+4 c
public _r_fill
_r_fill PROC FAR
	cli
	push bp
	mov bp, sp
	;; sp+6 c
	
	mov dx, SCI
	mov ax, ALL_PLANES ;; select all planes
	add ax, MAP_MASK
	out dx, ax

	mov ax, VSTART ;; video memory start
	mov es, ax

	mov di, _pgoffs ;; page offset
	
	mov ah, [bp+6] ;; color
	mov al, [bp+6]
	
	mov cx, W*H/8 ;; size
	
	rep stosw ;; fill

	pop bp
	sti
	ret
_r_fill ENDP

;; void r_vfill(int y0, int h, byte c)
;; sp+4 y0
;; sp+6 h
;; sp+8 c
public _r_vfill
_r_vfill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 y0
	;; bp+8 h
	;; bp+10 c
	push di
	
	mov dx, SCI
	mov ax, ALL_PLANES ;; select all planes
	add ax, MAP_MASK
	out dx, ax

	mov ax, VSTART ;; video memory start
	mov es, ax

	mov di, W/4
	mov ax, [bp+6]
	mul di
	mov di, ax

	add di, _pgoffs ;; page offset
	
	mov cx, W/8
	mov ax, [bp+8]
	mul cx
	mov cx, ax

	mov ah, [bp+10] ;; color
	mov al, [bp+10]
	
	rep stosw ;; fill

	pop di
	pop bp
	sti
	ret
_r_vfill ENDP

;; void r_planefill(int x, int y, int p, byte c)
;; sp+4 x
;; sp+6 y
;; sp+8 p
;; sp+10 c
public _r_planefill
_r_planefill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x
	;; bp+8 y
	;; bp+10 p
	;; bp+12 c
	push di

	mov dx, PIX_PLANE
	mov ax, [bp+10] ;; select planes to draw 0-16
	mul dx
	add ax, MAP_MASK
	mov dx, SCI
	out dx, ax

	mov ax, VSTART ;; video memory start
	mov es, ax

	mov dx, [bp+8] ;; y
	mov ax, W/4
	mul dx ;; y offset

	mov di, [bp+6] ;; x
	shr di, 2 ;; x coordinate of plane

	add di, ax ;; calculate address
	add di, _pgoffs ;; add page offset

	xor ax, ax
	mov al, [bp+12] ;; color

	mov es:[di], al ;; draw planes

	pop di
	pop bp
	sti
	ret
_r_planefill ENDP

;; void r_hlinefill1(int x0, int x1, int y, byte c)
;; sp+4 x0
;; sp+6 x1
;; sp+8 y
;; sp+10 c
public _r_hlinefill1
_r_hlinefill1 PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x0
	;; bp+8 x1
	;; bp+10 y
	;; bp+12 c
	push di

	mov ax, ALL_PLANES ;; select all planes
	add ax, MAP_MASK
	mov dx, SCI
	out dx, ax

	mov ax, VSTART ;; video memory start
	mov es, ax

	mov dx, [bp+10] ;; y
	mov ax, W/4
	mul dx ;; y offset

	mov cx, [bp+8] ;; x1
	shr cx, 2
	mov di, [bp+6] ;; x0
	shr di, 2 ;; plane coordinates

	inc di ;; remove ends
	dec cx

	sub cx, di
	inc cx ;; n = x1 - x0 + 1
	add di, ax ;; calculate address x0>>2 + y * W/4
	add di, _pgoffs ;; add page offset

	xor ax, ax
	mov al, [bp+12] ;; color

	rep stosb ;; fill line

	pop di
	pop bp
	sti
	ret
_r_hlinefill1 ENDP

;; void r_hlinefill2(int x0, int x1, int y, byte c)
;; sp+4 x0
;; sp+6 x1
;; sp+8 y
;; sp+10 c
public _r_hlinefill2
_r_hlinefill2 PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x0
	;; bp+8 x1
	;; bp+10 y
	;; bp+12 c
	push di

	mov ax, ALL_PLANES ;; select all planes
	add ax, MAP_MASK
	mov dx, SCI
	out dx, ax

	mov ax, VSTART ;; video memory start
	mov es, ax

	mov dx, [bp+10] ;; y
	mov ax, W/4
	mul dx ;; y offset

	mov cx, [bp+8] ;; x1
	shr cx, 2
	mov di, [bp+6] ;; x0
	shr di, 2 ;; plane coordinates

	inc di ;; remove ends
	dec cx

	sub cx, di
	inc cx ;; n = x1 - x0 + 1
	shr cx, 1 ;; words
	add di, ax ;; calculate address x0>>2 + y * W/4
	add di, _pgoffs ;; add page offset

	mov al, [bp+12] ;; color
	mov ah, [bp+12]

	rep stosw ;; fill line

	pop di
	pop bp
	sti
	ret
_r_hlinefill2 ENDP

;; void r_hplanefill(int x0, int x1, int y, int p, byte c)
;; sp+4 x0
;; sp+6 x1
;; sp+8 y
;; sp+10 p
;; sp+12 c
public _r_hplanefill
_r_hplanefill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x0
	;; bp+8 x1
	;; bp+10 y
	;; bp+12 p
	;; bp+14 c
	push di

	mov dx, PIX_PLANE
	mov ax, [bp+12] ;; select planes to draw 0-16
	mul dx
	add ax, MAP_MASK
	mov dx, SCI
	out dx, ax

	mov ax, VSTART ;; video memory start
	mov es, ax

	mov dx, [bp+10] ;; y
	mov ax, W/4
	mul dx ;; y offset

	mov cx, [bp+8] ;; x1
	shr cx, 2
	mov di, [bp+6] ;; x0
	shr di, 2 ;; plane coordinates

	sub cx, di
	inc cx ;; n = x1 - x0 + 1
	add di, ax ;; calculate address
	add di, _pgoffs ;; add page offset

	xor ax, ax
	mov al, [bp+14] ;; color

	rep stosb ;; fill line

	pop di
	pop bp
	sti
	ret
_r_hplanefill ENDP

;; void r_vplanefill(int x, int y0, int y1, int p, byte c)
;; sp+4 x
;; sp+6 y0
;; sp+8 y1
;; sp+10 p
;; sp+12 c
public _r_vplanefill
_r_vplanefill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x
	;; bp+8 y0
	;; bp+10 y1
	;; bp+12 p
	;; bp+14 c
	push di
	push si
	
	mov dx, PIX_PLANE
	mov ax, [bp+12] ;; select planes to draw 0-16
	mul dx
	add ax, MAP_MASK
	mov dx, SCI
	out dx, ax

	mov ax, VSTART ;; video memory start
	mov es, ax

	mov si, W/4
	mov ax, [bp+10] ;; y1
	mul si
	mov bx, ax ;; end point
	add bx, _pgoffs ;; add page offset for cmp

	mov ax, [bp+8] ;; y0
	mul si
	mov cx, ax ;; start point

	mov di, [bp+6] ;; x
	shr di, 2
	add di, cx ;; calculate address x>>2 + y0 * W/4
	add di, _pgoffs ;; page offset

	xor ax, ax
	mov al, [bp+14] ;; color
	
vfill:
	mov es:[di], al ;; put pixel
	add di, si ;; y += 1

	cmp di, bx ;; end?
	jb vfill

	pop si
	pop di
	pop bp
	sti
	ret
_r_vplanefill ENDP

;; void r_linefill(int x0, int y0, int x1, int y1, byte c)
;; sp+4 x0
;; sp+6 y0
;; sp+8 x1
;; sp+10 y1
;; sp+12 c
public _r_linefill
_r_linefill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x0
	;; bp+8 y0
	;; bp+10 x1
	;; bp+12 y1
	;; bp+14 c
	push di
	push si

	mov ax, VSTART
	mov es, ax ;; video memory start

	mov cx, [bp+10] ;; x1
	mov bx, [bp+6] ;; x0
	sub cx, bx ;; $cx = dx = x1 - x0

	mov dx, [bp+12] ;; y1
	mov bx, [bp+8] ;; y0
	sub dx, bx ;; $dx = dy = y1 - y0

	mov si, 1 ;; sx
	mov di, W/4 ;; sy

	cmp cx, 0
	jge lfdxs

	;; if dx < 0
	neg si
	neg cx
	
lfdxs:
	cmp dx, 0
	jge lfdys

	;; if dy < 0
	neg di
	neg dx
	
lfdys:
	push cx ;; dx to stack +10
	push dx ;; dy to stack +8
	push si ;; sx to stack +6
	push di ;; sy to stack +4

	mov di, cx
	sub di, dx ;; $di = diff

	cmp cx, dx
	jg lfmaxd

	;; if dy > dx
	mov cx, dx
	
lfmaxd:
	push cx ;; maxd to stack +2

	mov cx, [bp+8] ;; start y
	mov ax, W/4
	mul cx
	add ax, _pgoffs ;; add page offset
	mov bx, ax ;; $bx = y = y0
	mov dx, [bp+6] ;; $dx = x = x0

	xor ax, ax
	mov al, [bp+14] ;; color
	push ax ;; color to stack

	mov bp, sp
	;; bp c
	;; bp+2 maxd
	;; bp+4 sy
	;; bp+6 sx
	;; bp+8 dy
	;; bp+10 dx

	xor sp, sp ;; $sp = i
	
lfill:
	mov cx, dx
	and cx, 3 ;; plane of pixel
	mov ax, PIX_PLANE
	add ax, MAP_MASK
	shl ah, cl
	mov cx, dx
	mov dx, SCI
	out dx, ax ;; select plane
	mov dx, cx

	mov si, dx
	sar si, 2
	add si, bx ;; calculate address x + y * W/4

	mov ax, [bp]
	mov es:[si], al ;; write pixel

	mov si, di
	sal si, 1 ;; $si *= 2
	mov cx, [bp+10] ;; dx
	cmp si, cx
	jg lfx

	;; if 2*d <= dx
	add di, cx ;; diff += dx
	mov cx, [bp+4] ;; sy
	add bx, cx ;; y += sy
	
lfx:
	mov cx, [bp+8] ;; dy
	neg cx
	cmp si, cx
	jl lfy

	;; if 2*d >= -dy
	add di, cx ;; diff += dy
	mov cx, [bp+6] ;; sx
	add dx, cx ;; x += sx
	
lfy:
	mov cx, [bp+2] ;; maxd
	inc sp ;; ++i
	cmp sp, cx ;; end?
	jbe lfill

	mov sp, bp

	pop ax
	pop ax
	pop ax
	pop ax
	pop ax
	pop ax

	pop si
	pop di
	pop bp
	sti
	ret
_r_linefill ENDP

;; void r_triplanefill(int x0, int dx0, int x1, int dx1,
;;                     int y, int dy, int p, byte c)
;; sp+4 x0
;; sp+6 dx0
;; sp+8 x1
;; sp+10 dx1
;; sp+12 y
;; sp+14 dy
;; sp+16 p
;; sp+18 c
public _r_triplanefill
_r_triplanefill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x0
	;; bp+8 dx0
	;; bp+10 x1
	;; bp+12 dx1
	;; bp+14 y
	;; bp+16 dy
	;; bp+18 p
	;; bp+20 c
	push di
	push si

	mov dx, PIX_PLANE
	mov ax, [bp+18] ;; select planes to draw 0-16
	mul dx
	add ax, MAP_MASK
	mov dx, SCI
	out dx, ax

	mov ax, VSTART
	mov es, ax ;; video memory start

	mov ax, [bp+14] ;; y
	add ax, [bp+16] ;; dy
	mov cx, W/4
	mul cx ;; final line
	add ax, _pgoffs ;; page offset for cmp
	push ax ;; end to stack

	mov ax, [bp+16] ;; dy
	movsx ecx, ax

	mov ax, [bp+12]
	movsx ebx, ax ;; dx1
	mov eax, ebx

	cmp ebx, 0 ;; dx1 > 0?
	jg tfs1
	neg eax ;; $ax = |dx1|
	
tfs1:
	shl eax, SH
	xor edx, edx
	div ecx ;; |k1| = (2^SH * |dx1|) / dy

	cmp ebx, 0 ;; dx1 > 0?
	jg tfss1
	neg eax ;; k1 = -|k1|
	
tfss1:
	push eax ;; k1 to stack

	mov ax, [bp+8] ;; dx0
	movsx ebx, ax
	mov eax, ebx
	
	cmp ebx, 0 ;; dx0 > 0?
	jg tfs0
	neg eax ;; $ax = |dx0|
	
tfs0:
	shl eax, SH
	xor edx, edx
	div ecx ;; |k0| = (2^SH * |dx0|) / dy

	cmp ebx, 0 ;; dx0 > 0?
	jg tfss0
	neg eax ;; k0 = -|k0|
	
tfss0:
	push eax ;; k0 to stack

	mov dx, W/4
	mov ax, [bp+14] ;; y
	mul dx ;; y offset
	add ax, _pgoffs ;; add page offset
	mov si, ax ;; $si = y

	mov ax, [bp+6] ;; left point x0
	inc ax
	movsx ebx, ax
	shl ebx, SH
	mov eax, 1
	shl eax, SH-1
	sub ebx, eax ;; left bias

	mov ax, [bp+10] ;; right point x1
	inc ax
	movsx edx, ax
	shl edx, SH
	mov eax, 1
	shl eax, SH-1
	add edx, eax ;; right bias

	xor ax, ax
	mov al, [bp+20] ;; color

	mov bp, sp
	;; bp k0
	;; bp+4 k1
	;; bp+8 yend
	
tfill:
	mov ecx, ebx ;; x0
	shr ecx, SH+2 ;; divide to calculate coordinates
	mov di, cx

	mov ecx, edx ;; x1
	shr ecx, SH+2

	sub cx, di
	;; cmp cx, 0 ;; check if x backwards
	;; jl tfend
	add di, si ;; calculate final address

	rep stosb ;; fill line

	add si, W/4 ;; y += 1

	mov ecx, [bp]
	add ebx, ecx ;; x0 += k0
	mov ecx, [bp+4]
	add edx, ecx ;; x1 += k1

	mov cx, [bp+8] ;; yend
	cmp si, cx
	jbe tfill ;; lines left?
	
tfend:
	pop eax
	pop eax
	pop ax

	pop si
	pop di
	pop bp
	sti
	ret
_r_triplanefill ENDP

;; void r_spritefill2(int x, int y, int w, int h, int tstart)
;; sp+4 x
;; sp+6 y
;; sp+8 w
;; sp+10 h
;; sp+12 tstart
public _r_spritefill2
_r_spritefill2 PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x
	;; bp+8 y
	;; bp+10 w
	;; bp+12 h
	;; bp+14 tstart
	push di
	push si
	push ds
	
	mov dx, GCI ;; read and write all 4 planes at once
	mov ax, 08h
	out dx, ax
	mov dx, PIX_PLANE
	mov ax, 0fh
	mul dx
	add ax, MAP_MASK
	mov dx, SCI
	out dx, ax
	mov ax, VSTART ;; video memory start
	mov es, ax

	mov si, W/4
	mov ax, [bp+8] ;; y1
	add ax, [bp+12] ;; h
	mul si
	mov bx, ax ;; end point
	add bx, _pgoffs ;; add page offset for cmp

	mov ax, [bp+8] ;; y0
	mul si
	mov cx, ax ;; start point

	mov di, [bp+6] ;; x
	shr di, 2
	add di, cx ;; calculate address x>>2 + y0 * W/4
	add di, _pgoffs ;; page offset

	mov ax, VSTART
	mov ds, ax
	mov si, [bp+14] ;; texture memory start
	
	mov ax, [bp+10] ;; w
	shr ax, 2
	mov dx, W/4
	sub dx, ax
sfill2:
	mov cx, ax
	rep movsb ;; fill textured line
	add di, dx ;; y += 1
	add si, dx

	cmp di, bx ;; end?
	jb sfill2

	mov dx, GCI
	inc dx
	mov ax, 0ffh
	out dx, ax
	
	pop ds
	pop si
	pop di
	pop bp
	sti
	ret
_r_spritefill2 ENDP

;; void r_spriteplanefill(int x, int y, int w, int h, int p,
;;                        int tw, int th, int tp, int tstart)
;; sp+4 x
;; sp+6 y
;; sp+8 w
;; sp+10 h
;; sp+12 p
;; bp+14 tw
;; bp+16 th
;; sp+18 tp
;; sp+20 tstart
public _r_spriteplanefill
_r_spriteplanefill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x
	;; bp+8 y
	;; bp+10 w
	;; bp+12 h
	;; bp+14 p
	;; bp+16 tw
	;; bp+18 th
	;; bp+20 tp
	;; bp+22 tstart
	push di
	push si
	push ds
	
	mov dx, PIX_PLANE
	mov ax, [bp+14] ;; select planes to draw 0-16
	mul dx
	add ax, MAP_MASK
	mov dx, SCI
	out dx, ax

	mov ax, VSTART ;; video memory start
	mov es, ax

	mov si, W/4
	mov ax, [bp+8] ;; y
	mul si
	mov bx, ax
	mov cx, [bp+10] ;; w
	add cx, 3
	mov ax, [bp+20] ;; tp
	and ax, 3
	sub cx, ax
	shr cx, 2 ;; (w + 3 - tp) >> 2
	mov ax, [bp+20] ;; tp
	and ax, 3
	add ax, [bp+6] ;; x
	shr ax, 2
	add ax, cx
	add ax, bx
	add ax, _pgoffs ;; add page offset for cmp
	push ax ;; xend to stack

	mov ax, [bp+8] ;; y
	mul si
	mov cx, ax ;; start point

	mov di, [bp+20] ;; tp
	and di, 3
	add di, [bp+6] ;; x
	shr di, 2
	add di, cx ;; calculate address x>>2 + y0 * W/4
	add di, _pgoffs ;; page offset

	mov ax, VSTART
	mov ds, ax
	xor esi, esi
	mov si, [bp+22] ;; texture memory start

	xor eax, eax
	mov ax, [bp+18] ;; th
	shl eax, SH+1
	xor ecx, ecx
	mov cx, [bp+12] ;; h
	xor edx, edx
	div ecx ;; th/h
	inc eax
	shr eax, 1
	push eax ;; th/h to stack
	
	mov ax, [bp+12] ;; h
	mov cx, W/4
	mul cx
	mov cx, ax
	dec cx
	push cx ;; dx to stack

	xor eax, eax
	mov ax, [bp+18] ;; th
	mov cx, W/4
	mul cx
	shl eax, SH
	mov ebx, eax
	
	xor eax, eax
	mov ax, [bp+16] ;; tw
	shl eax, SH+1
	xor ecx, ecx
	mov cx, [bp+10] ;; w
	xor edx, edx
	div ecx ;; tw/w
	inc eax
	shr eax, 1
	sub ebx, eax
	push ebx ;; dtx to stack
	mov ebx, eax
	
	mov cx, W/4
	dec cx
	push cx ;; dy to stack

	xor eax, eax
	mov ax, W/4
	shl eax, SH
	push eax ;; dty to stack

	xor eax, eax
	mov ax, [bp+20] ;; tp
	xor ecx, ecx
	mov cx, ax
	and ax, 3
	shr ecx, 2
	shl ecx, SH-2
	mul ebx ;; * tw/w
	shr eax, 2
	mov edx, eax
	add edx, ecx
	shl esi, SH
	add edx, esi ;; tstart

	mov bp, sp
	;; bp dty
	;; bp+4 dy
	;; bp+6 dtx
	;; bp+10 dx
	;; bp+12 th/h
	;; bp+16 xend
	;; bp+36 h

	push edx
	mov ecx, [bp+12] ;; th/h
	mov edx, 1
	shl edx, SH
	cmp ecx, edx
	jbe sfillyf

	shr ecx, SH
	inc ecx
	mov eax, [bp] ;; dty
	mul ecx
	mov [bp], eax
	mov eax, [bp+12] ;; th/h
	xor edx, edx
	div ecx
	mov [bp+12], eax
sfillyf:
 	pop edx

	xor ebx, ebx
	add ebx, [bp+12] ;; th/h
	shr ebx, 1
sfillx:
	mov ecx, edx
	shr ecx, SH-2
	push edx
	mov dx, GCI
	mov ax, READ_MAP
	out dx, ax
	inc dx
	mov ax, cx ;; select read plane
	and ax, 3
	out dx, ax
	pop edx
	
	mov cx, [bp+36] ;; h
sfilly:
	mov esi, edx
	shr esi, SH

	mov al, ds:[si]
	cmp al, ALPHA ;; alpha test
	je sfilla
	mov es:[di], al ;; write textured pixel
	;; movsb ;; (if alpha not needed)
	
sfilla:
	inc di
	
	add di, [bp+4] ;; dy
	add ebx, [bp+12] ;; th/h
	mov eax, ebx
	
	and eax, MASK_HI
	sub ebx, eax
	
	neg eax ;; if 1, add dty
	and eax, [bp] ;; dty
	add edx, eax
	
	dec cx
	cmp cx, 0
	ja sfilly

	sub di, [bp+10] ;; dx
	sub edx, [bp+6] ;; dtx

	mov cx, [bp+16] ;; xend
	cmp di, cx ;; end?
	jb sfillx

	pop eax
	pop ax
	pop eax
	pop ax
	pop eax
	pop ax

	pop ds
	pop si
	pop di
	pop bp
	sti
	ret
_r_spriteplanefill ENDP

code ENDS
END

