.model small
code SEGMENT para public 'CODE'
ASSUME cs:code

.386

;; int get_code()
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

	sti

	mov ax, cx
	ret
_get_keycode ENDP

code ENDS
END
