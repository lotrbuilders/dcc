section .text

global is_terminal:function (is_terminal.end-is_terminal)
is_terminal:
	push ebp
	mov ebp,esp

	mov eax,[ebp+8]
	cmp eax,4
	je .L0
	cmp eax,6
	je .L1
	cmp eax,8
	je .L2
	jmp .L3


.L0:


.L1:


.L2:

	mov eax,1
	pop ebp
	ret

.L3:

	mov eax,0
	pop ebp
	ret

.L4:

	mov eax,0
	pop ebp
	ret
.end:

