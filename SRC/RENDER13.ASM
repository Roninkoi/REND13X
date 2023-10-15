code SEGMENT para public 'CODE'
ASSUME cs:code

.386

VSTART equ 0a000h
W equ 320
H equ 200

;; int r_init()
public _r_init
_r_init PROC FAR
	mov ah, 0fh
	int 10h
	xor dh, dh
	mov dl, al ;; get current video mode

	xor ah, ah
	mov al, 13h ;; set mode 13h
	int 10h

	mov ah, 48h
	mov bx, 10h ;; allocate 64k
	int 21h
	mov ax, dx ;; return old video mode
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

;; void r_putpixel(int x, int y, byte c)
;; sp+4 x
;; sp+6 y
;; sp+8 c
public _r_putpixel
_r_putpixel PROC FAR
	push bp
	mov bp, sp
	;; bp+6 x
	;; bp+8 y
	;; bp+10 c
	push di
	
	mov ax, VSTART
	mov es, ax ;; video memory start

	mov dx, [bp+8] ;; y
	mov ax, W
	mul dx ;; $ax = y * W
	add ax, [bp+6] ;; $ax = x + y * W

	mov di, ax ;; pixel address

	xor ax, ax
	mov al, [bp+10] ;; color
	
	mov [es]:di, al ;; put pixel
	
	pop di
	pop bp
	ret
_r_putpixel ENDP

;; void r_scr(byte c)
;; sp+4 c
public _r_scr
_r_scr PROC FAR
	push bp
	mov bp, sp
	;; bp+6 c
	
	xor al, al
	mov ah, 6h
	xor bl, bl
	mov bh, [bp+6]
	mov cx, 0100h
	mov dx, 182ah
	int 10h

	pop bp
	ret
_r_scr ENDP

;; void r_fill(byte c)
;; sp+4 c
public _r_fill
_r_fill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 c
	push di
	
	mov ax, VSTART
	mov es, ax ;; video memory start
	
	xor di, di
	mov cx, W*H/2
	xor ax, ax
	mov al, [bp+6]
	mov ah, [bp+6]
	
	rep stosw

	pop di
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
	
	mov ax, VSTART
	mov es, ax ;; video memory start

	mov di, W
	mov ax, [bp+6]
	mul di
	mov di, ax

	mov cx, W
	mov ax, [bp+8]
	mul cx
	mov cx, ax

	xor ah, ah
	mov al, [bp+10]

	rep stosb

	pop di
	pop bp
	sti
	ret
_r_vfill ENDP

;; void r_rectfill(int x, int y, int w, int h, byte c)
;; sp+4 x
;; sp+6 y
;; sp+8 w
;; sp+10 h
;; sp+12 c
public _r_rectfill
_r_rectfill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x
	;; bp+8 y
	;; bp+10 w
	;; bp+12 h
	;; bp+14 c
	push di
	push si
	
	mov ax, VSTART
	mov es, ax ;; video memory start

	mov dx, [bp+8] ;; y
	mov ax, W
	mul dx ;; y offset

	;; start points
	mov di, [bp+6] ;; x
	;; right edge
	mov cx, [bp+10] ;; w

	mov si, cx ;; $si = w
	add di, ax ;; start address = x + y * W

	mov dx, [bp+8] ;; y
	add dx, [bp+12] ;; h
	mov ax, W
	mul dx
	mov dx, ax ;; final line address

	xor ah, ah
	mov al, [bp+14] ;; color

	mov bx, W
	sub bx, cx ;; $bx = w - W
	
	rfill:
	rep stosb ;; fill line

	mov cx, si ;; $cx = w
	add di, bx ;; y += 1

	cmp di, dx ;; end?
	jb rfill

	pop si
	pop di
	pop bp
	sti
	ret
_r_rectfill ENDP

;; void r_hlinefill(int x0, int x1, int y, byte c)
;; sp+4 x0
;; sp+6 x1
;; sp+8 y
;; sp+10 c
public _r_hlinefill
_r_hlinefill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x0
	;; bp+8 x1
	;; bp+10 y
	;; bp+12 c
	push di

	mov ax, VSTART
	mov es, ax ;; video memory start

	mov dx, [bp+10] ;; y
	mov ax, W
	mul dx ;; y offset = y * W

	mov cx, [bp+8] ;; x1
	mov di, [bp+6] ;; x0

	sub cx, di ;; x1 - x0
	inc cx ;; w = x1 - x0 + 1
	add di, ax ;; start address = x0 + y * W

	xor ah, ah
	mov al, [bp+12] ;; color

	rep stosb ;; fill line

	pop di
	pop bp
	sti
	ret
_r_hlinefill ENDP

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
	
	mov ax, VSTART
	mov es, ax ;; video memory start

	mov dx, [bp+10] ;; y
	mov ax, W
	mul dx ;; y offset

	mov cx, [bp+8] ;; x1
	mov di, [bp+6] ;; x0

	sub cx, di ;; x1 - x0
	inc cx ;; w = x1 - x0 + 1
	shr cx, 1 ;; divide by 2, discard odd
	add di, ax ;; calculate address

	mov al, [bp+12] ;; color
	mov ah, [bp+12]

	rep stosw ;; fill line
	
	pop di
	pop bp
	sti
	ret
_r_hlinefill2 ENDP

;; void r_vlinefill(int x, int y0, int y1, byte c)
;; sp+4 x
;; sp+6 y0
;; sp+8 y1
;; sp+10 c
public _r_vlinefill
_r_vlinefill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x
	;; bp+8 y0
	;; bp+10 y1
	;; bp+12 c
	push di
	
	mov ax, VSTART
	mov es, ax ;; video memory start

	mov di, [bp+8] ;; y0
	mov ax, W
	mul di
	mov di, ax
	;; starting address
	add di, [bp+6] ;; x

	mov cx, [bp+10] ;; y1
	mov ax, W
	mul cx

	mov dx, ax ;; $dx = y1 * W
	;; calculate final address
	add dx, [bp+6] ;; x

	xor ah, ah
	mov al, [bp+12] ;; color
	
	vfill:
	mov [es]:di, al ;; put pixel
	add di, W ;; y += 1
	cmp di, dx ;; end?
	jb vfill
	
	pop di
	pop bp
	sti
	ret
_r_vlinefill ENDP

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
	sub cx, bx ;; cx = dx = x1 - x0

	mov dx, [bp+12] ;; y1
	mov bx, [bp+8] ;; y0
	sub dx, bx ;; dx = dy = y1 - y0

	mov si, 1 ;; sx
	mov di, W ;; sy

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
	push cx ;; dx to stack +8
	push dx ;; dy to stack +6
	push si ;; sx to stack +4
	push di ;; sy to stack +2

	mov di, cx
	sub di, dx ;; $di = diff

	cmp cx, dx
	jg lfmaxd

	;; if dy > dx
	mov cx, dx ;; maxd = dy
	
	lfmaxd:
	push cx ;; maxd to stack

	mov bx, [bp+6] ;; $bx = x = x0
	mov cx, [bp+8] ;; $cx = y = y0
	mov ax, W
	mul cx
	mov cx, ax
	add cx, bx ;; start address x0 + y0 * W

	xor ax, ax
	mov al, [bp+14] ;; color

	mov bp, sp
	;; bp maxd
	;; bp+2 sy
	;; bp+4 sx
	;; bp+6 dy
	;; bp+8 dx
	;; bp+10 x0
	;; bp+12 y0
	;; bp+14 x1
	;; bp+16 y1
	;; bp+18 c

	xor bx, bx ;; i = $bx
	
	lfill:
	mov si, cx ;; pixel address x + y * W

	mov [es]:si, al ;; put pixel

	mov si, di
	sal si, 1 ;; $si *= 2
	mov dx, [bp+8] ;; dx
	cmp si, dx
	jg lfx

	;; if 2*d <= dx
	add di, dx
	mov dx, [bp+2] ;; sy
	add cx, dx ;; y += 1
	
	lfx:
	mov dx, [bp+6] ;; dy
	neg dx
	cmp si, dx
	jl lfy

	;; if 2*d >= -dy
	add di, dx
	mov dx, [bp+4] ;; sx
	add cx, dx ;; x += 1
	
	lfy:
	mov dx, [bp] ;; maxd
	inc bx ;; ++i
	cmp bx, dx ;; end?
	jbe lfill

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

;; void r_trifill(int x0, int dx0, int x1, int dx1, int y, int dy, byte c)
;; sp+4 x0
;; sp+6 dx0
;; sp+8 x1
;; sp+10 dx1
;; sp+12 y
;; sp+14 dy
;; sp+16 c
public _r_trifill
_r_trifill PROC FAR
	cli
	push bp
	mov bp, sp
	;; bp+6 x0
	;; bp+8 dx0
	;; bp+10 x1
	;; bp+12 dx1
	;; bp+14 y
	;; bp+16 dy
	;; bp+18 c
	push di
	push si

	mov ax, VSTART
	mov es, ax ;; video memory start

	;; final line
	mov ax, [bp+14] ;; y
	add ax, [bp+16] ;; dy
	mov cx, W
	mul cx
	push ax ;; final line start address y * W to stack

	mov cx, [bp+16] ;; dy

	mov ax, [bp+12] ;; dx1
	mov si, ax

	cmp si, 0 ;; dx1 > 0?
	jg tfs1
	neg ax ;; $ax = |dx1|
	
	tfs1:
	shl ax, 7 ;; 128 * |dx1|
	xor dx, dx
	div cx ;; |k1| = (128 * |dx1|) / dy
	cmp si, 0 ;; dx1 > 0?
	jg tfss1
	neg ax ;; k1 = -|k1|
	
	tfss1:
	push ax ;; k1 to stack

	mov si, [bp+10] ;; right point x1
	inc si
	shl si, 7
	add si, 80 ;; right bias

	mov ax, [bp+8] ;; dx0
	mov bx, ax

	cmp bx, 0 ;; dx0 > 0?
	jg tfs0
	neg ax ;; $ax = |dx0|
	
	tfs0:
	shl ax, 7 ;; 128 * |dx0|
	xor dx, dx
	div cx ;; |k0| = (128 * |dx0|) / dy
	cmp bx, 0 ;; dx0 > 0?
	jg tfss0
	neg ax ;; k0 = -|k0|
	
	tfss0:
	push ax ;; k0 to stack

	mov bx, [bp+6] ;; left point x0
	inc bx
	shl bx, 7
	sub bx, 80 ;; left bias

	mov dx, W
	mov ax, [bp+14] ;; y
	mul dx ;; y offset
	mov dx, ax

	xor ax, ax
	mov al, [bp+18] ;; color

	mov bp, sp
	;; bp k0
	;; bp+2 k1
	;; bp+4 yend
	;; bp+6 x0
	;; bp+8 dx0
	;; bp+10 x1
	;; bp+12 dx1
	;; bp+14 y
	;; bp+16 dy
	;; bp+18 c
	
	tfill:
	mov di, bx ;; x0
	shr di, 7 ;; divide to calculate coordinate

	mov cx, si ;; x1
	shr cx, 7 ;; divide to calculate coordinate

	sub cx, di
	cmp cx, 0 ;; check if x backwards
	jl tfend
	add di, dx ;; calculate final address

	rep stosb ;; fill line

	add dx, W ;; y += 1

	mov di, [bp]
	add bx, di ;; x0 += k0
	mov cx, [bp+2]
	add si, cx ;; x1 += k1

	mov cx, [bp+4] ;; yend
	cmp dx, cx
	jbe tfill ;; lines left?
	
	tfend:
	pop ax
	pop ax
	pop ax

	pop si
	pop di
	pop bp
	sti
	ret
_r_trifill ENDP

code ENDS
END
