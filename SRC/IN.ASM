.model small
code SEGMENT para public 'CODE'
ASSUME cs:code

.386

;; unsigned get_keycode()
public _get_keycode
_get_keycode PROC FAR
	cli

	xor cx, cx

	in al, 060h ;; read code
	mov cl, al
	in al, 061h ;; status
	mov bl, al
	or al, 080h
	out 061h, al
	mov al, bl
	out 061h, al

	mov al, 020h ;; reset
	out 020h, al

	mov ax, cx

	sti
	ret
_get_keycode ENDP

;; unsigned get_mousepos()
public _get_mousepos
_get_mousepos PROC FAR
	cli

	mov ax, 03h
	int 33h

	;; cx = mouse x (0-639)
	;; dx = mouse y (0-199)
	mov ax, 320
	mul dx
	;; 640 -> 320, divide x by 2
	shr cx, 1
	add ax, cx

	sti
	ret
_get_mousepos ENDP

;; int get_mouseclick()
public _get_mouseclick
_get_mouseclick PROC FAR
	cli

	mov ax, 03h
	int 33h

	;; bx = mouse buttons pressed
	;; 01 = left down
	;; 10 = right down
	mov ax, bx

	sti
	ret
_get_mouseclick ENDP

;; void set_mousepos(int x, int y)
;; bp+4 x
;; bp+6 y
public _set_mousepos
_set_mousepos PROC FAR
	push bp
	;; bp+6 x
	;; bp+8 y
	mov bp, sp

	mov ax, 04h
	mov cx, [bp+6] ;; set mouse x
	shl cx, 1
	mov dx, [bp+8] ;; set mouse y
	int 33h

	pop bp
	ret
_set_mousepos ENDP

;; void set_mouse(void (interrupt *handler) ())
;; bp+4 handler near
;; bp+6 handler far
public _set_mouse
_set_mouse PROC FAR
	push bp
	;; bp+6 handler near
	;; bp+8 handler far
	mov bp, sp

	mov es, [bp+8]
	mov dx, [bp+6]

	mov cx, 1fh
	mov ax, 0ch
	int 33h

	pop bp
	ret
_set_mouse ENDP

;; int reset_mouse()
public _reset_mouse
_reset_mouse PROC FAR
	mov ax, 00h
	int 33h

	ret
_reset_mouse ENDP

code ENDS
END
