.model small
code SEGMENT para public 'CODE'
ASSUME cs:code

.386

;; int timer()
public _timer
_timer PROC FAR
	cli

	mov al, 00000000b
	out 43h, al

	in al, 40h
	mov ah, al
	in al, 40h

	xchg al, ah

	sti

	ret
_timer ENDP

;; int timer_start()
public _timer_start
_timer_start PROC FAR
	cli

	mov al, 34h
	out 43h, al

	mov al, 0eeh
	out 40h, al
	mov al, 00h
	out 40h, al

	mov al, 00h
	out 43h, al
	
	in al, 40h
	mov ah, al
	in al, 40h

	xchg al, ah

	sti

	ret
_timer_start ENDP

;; int timer_stop()
public _timer_stop
_timer_stop PROC FAR
	cli

	mov al, 00h
	out 43h, al
	
	in al, 40h
	mov ah, al
	in al, 40h

	xchg al, ah
	mov bx, ax

	mov al, 00110110b
	out 43h, al

	mov al, 00h
	out 40h, al
	mov al, 00h
	out 40h, al

	mov ax, bx

	sti

	ret
_timer_stop ENDP

;; void timer_end()
public _timer_end
_timer_end PROC FAR
	mov al, 20h
	out 20h, al
	ret
_timer_end ENDP

code ENDS
END
