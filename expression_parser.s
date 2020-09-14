section .text

global parse_expression:function (parse_expression.end-parse_expression)
parse_expression:
	push ebp
	mov ebp,esp

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L16

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L16:

	sub esp,8
	call parse_assignment_expression
	add esp,8
	pop ebp
	ret
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0


global parse_assignment_expression:function (parse_assignment_expression.end-parse_assignment_expression)
parse_assignment_expression:
	push ebp
	mov ebp,esp
	sub esp,8

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L17

	sub esp,8
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,16

.L17:

	sub esp,0
	call parse_conditional_expression
	add esp,0
	mov [ebp-4],eax

	sub esp,0
	call peek
	add esp,0
	cmp eax,61
	sete al
	movzx eax,al
	test eax,eax
	jz .L18


	sub esp,4
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	sub esp,4
	call pop
	add esp,4
	push eax
	call newnode
	add esp,16
	mov [ebp-8],eax

	sub esp,0
	call parse_assignment_expression
	add esp,0
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

.L18:

	mov eax,[ebp-4]
	add esp,8
	pop ebp
	ret
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 97
	db 115
	db 115
	db 105
	db 103
	db 110
	db 109
	db 101
	db 110
	db 116
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0


global parse_conditional_expression:function (parse_conditional_expression.end-parse_conditional_expression)
parse_conditional_expression:
	push ebp
	mov ebp,esp
	sub esp,20

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L19

	sub esp,12
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L19:

	sub esp,4
	call parse_logical_or_expression
	add esp,4
	mov [ebp-4],eax

	sub esp,4
	call peek
	add esp,4
	cmp eax,63
	sete al
	movzx eax,al
	test eax,eax
	jz .L20


	sub esp,4
	call pop
	add esp,4

	mov eax,[line_number]
	mov [ebp-8],eax

	sub esp,4
	call parse_expression
	add esp,4
	mov [ebp-12],eax

	sub esp,4
	call peek
	add esp,4
	cmp eax,58
	setne al
	movzx eax,al
	test eax,eax
	jz .L22


	sub esp,8
	mov eax,[line_number]
	push eax
	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,.S1
	push eax
	call parser_error
	add esp,20

	mov eax,0
	add esp,20
	pop ebp
	ret

.L22:

	sub esp,4
	call pop
	add esp,4

	sub esp,4
	call parse_conditional_expression
	add esp,4
	mov [ebp-16],eax

	sub esp,8
	mov eax,3
	push eax
	mov eax,[ebp-8]
	push eax
	mov eax,407
	push eax
	call newnode
	add esp,20
	mov [ebp-20],eax

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-20]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-12]
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-20]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-16]
	push eax
	mov eax,2
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-20]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-20]
	add esp,20
	pop ebp
	ret

	jmp .L21

.L20:

	mov eax,[ebp-4]
	add esp,20
	pop ebp
	ret

.L21:
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 99
	db 111
	db 110
	db 100
	db 105
	db 116
	db 105
	db 111
	db 110
	db 97
	db 108
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0

.S1:
	db 69
	db 120
	db 112
	db 101
	db 99
	db 116
	db 101
	db 100
	db 32
	db 39
	db 58
	db 39
	db 32
	db 110
	db 111
	db 116
	db 32
	db 37
	db 84
	db 0


global parse_logical_or_expression:function (parse_logical_or_expression.end-parse_logical_or_expression)
parse_logical_or_expression:
	push ebp
	mov ebp,esp
	sub esp,20

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L23

	sub esp,12
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L23:

	sub esp,4
	call parse_logical_and_expression
	add esp,4
	mov [ebp-4],eax

.L24:

	sub esp,4
	call peek
	add esp,4
	cmp eax,193
	sete al
	movzx eax,al
	test eax,eax
	jz .L25


	sub esp,4
	call pop
	add esp,4

	sub esp,8
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	mov eax,401
	push eax
	call newnode
	add esp,20
	mov [ebp-8],eax

	sub esp,4
	call parse_logical_and_expression
	add esp,4
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L24

.L25:

	mov eax,[ebp-4]
	add esp,20
	pop ebp
	ret
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 108
	db 111
	db 103
	db 105
	db 99
	db 97
	db 108
	db 45
	db 111
	db 114
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0


global parse_logical_and_expression:function (parse_logical_and_expression.end-parse_logical_and_expression)
parse_logical_and_expression:
	push ebp
	mov ebp,esp
	sub esp,20

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L26

	sub esp,12
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L26:

	sub esp,4
	call parse_and_expression
	add esp,4
	mov [ebp-4],eax

.L27:

	sub esp,4
	call peek
	add esp,4
	cmp eax,192
	sete al
	movzx eax,al
	test eax,eax
	jz .L28


	sub esp,4
	call pop
	add esp,4

	sub esp,8
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	mov eax,402
	push eax
	call newnode
	add esp,20
	mov [ebp-8],eax

	sub esp,4
	call parse_and_expression
	add esp,4
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L27

.L28:

	mov eax,[ebp-4]
	add esp,20
	pop ebp
	ret
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 108
	db 111
	db 103
	db 105
	db 99
	db 97
	db 108
	db 45
	db 97
	db 110
	db 100
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0


global parse_and_expression:function (parse_and_expression.end-parse_and_expression)
parse_and_expression:
	push ebp
	mov ebp,esp
	sub esp,20

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L29

	sub esp,12
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L29:

	sub esp,4
	call parse_equality_expression
	add esp,4
	mov [ebp-4],eax

.L30:

	sub esp,4
	call peek
	add esp,4
	cmp eax,38
	sete al
	movzx eax,al
	test eax,eax
	jz .L31


	sub esp,4
	call pop
	add esp,4

	sub esp,8
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	mov eax,38
	push eax
	call newnode
	add esp,20
	mov [ebp-8],eax

	sub esp,4
	call parse_equality_expression
	add esp,4
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L30

.L31:

	mov eax,[ebp-4]
	add esp,20
	pop ebp
	ret
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 97
	db 110
	db 100
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0


global parse_equality_expression:function (parse_equality_expression.end-parse_equality_expression)
parse_equality_expression:
	push ebp
	mov ebp,esp
	sub esp,20

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L32

	sub esp,12
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L32:

	sub esp,4
	call parse_relational_expression
	add esp,4
	mov [ebp-4],eax

.L33:

	sub esp,4
	call peek
	add esp,4
	cmp eax,194
	sete al
	movzx eax,al
	test eax,eax
	jnz .L92
	sub esp,4
	call peek
	add esp,4
	cmp eax,195
	sete al
	movzx eax,al
	test eax,eax
	jz .L93
.L92:
	mov eax,1
	jmp .L94
.L93:
	mov eax,0
.L94:
	test eax,eax
	jz .L34


	sub esp,4
	call peek
	add esp,4
	cmp eax,194
	sete al
	movzx eax,al
	test eax,eax
	jz .L35

	sub esp,8
	mov eax,3
	push eax
	mov eax,[line_number]
	push eax
	mov eax,403
	push eax
	call newnode
	add esp,20
	mov [ebp-8],eax

	jmp .L36

.L35:

	sub esp,8
	mov eax,3
	push eax
	mov eax,[line_number]
	push eax
	mov eax,404
	push eax
	call newnode
	add esp,20
	mov [ebp-8],eax

.L36:

	sub esp,4
	call pop
	add esp,4

	sub esp,4
	call parse_relational_expression
	add esp,4
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L33

.L34:

	mov eax,[ebp-4]
	add esp,20
	pop ebp
	ret
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 101
	db 113
	db 117
	db 97
	db 108
	db 105
	db 116
	db 121
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0


global parse_relational_expression:function (parse_relational_expression.end-parse_relational_expression)
parse_relational_expression:
	push ebp
	mov ebp,esp
	sub esp,24

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L37

	sub esp,8
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,16

.L37:

	sub esp,0
	call parse_additive_expression
	add esp,0
	mov [ebp-4],eax

	sub esp,0
	call peek
	add esp,0
	mov [ebp-8],eax

.L38:

	mov eax,[ebp-8]
	cmp eax,60
	sete al
	movzx eax,al
	test eax,eax
	jnz .L101
	mov eax,[ebp-8]
	cmp eax,62
	sete al
	movzx eax,al
	test eax,eax
	jz .L102
.L101:
	mov eax,1
	jmp .L103
.L102:
	mov eax,0
.L103:
	test eax,eax
	jnz .L98
	mov eax,[ebp-8]
	cmp eax,196
	sete al
	movzx eax,al
	test eax,eax
	jz .L99
.L98:
	mov eax,1
	jmp .L100
.L99:
	mov eax,0
.L100:
	test eax,eax
	jnz .L95
	mov eax,[ebp-8]
	cmp eax,197
	sete al
	movzx eax,al
	test eax,eax
	jz .L96
.L95:
	mov eax,1
	jmp .L97
.L96:
	mov eax,0
.L97:
	test eax,eax
	jz .L39


	sub esp,0
	call pop
	add esp,0

	mov eax,[ebp-8]
	cmp eax,60
	je .L0
	cmp eax,62
	je .L1
	cmp eax,196
	je .L2
	cmp eax,197
	je .L3
	jmp .L4


.L0:


.L1:

	mov eax,[ebp-8]
	mov [ebp-12],eax

	jmp .L40

.L2:

	mov eax,405
	mov [ebp-12],eax

	jmp .L40

.L3:

	mov eax,406
	mov [ebp-12],eax

	jmp .L40

.L4:

	mov eax,[ebp-8]
	mov [ebp-12],eax

	jmp .L40

.L40:

	sub esp,4
	mov eax,3
	push eax
	mov eax,[line_number]
	push eax
	mov eax,[ebp-12]
	push eax
	call newnode
	add esp,16
	mov [ebp-16],eax

	sub esp,0
	call parse_additive_expression
	add esp,0
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-16]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-16]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-16]
	mov [ebp-4],eax

	sub esp,0
	call peek
	add esp,0
	mov [ebp-8],eax

	jmp .L38

.L39:

	mov eax,[ebp-4]
	add esp,24
	pop ebp
	ret
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 114
	db 101
	db 108
	db 97
	db 116
	db 105
	db 111
	db 110
	db 97
	db 108
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0


global parse_additive_expression:function (parse_additive_expression.end-parse_additive_expression)
parse_additive_expression:
	push ebp
	mov ebp,esp
	sub esp,20

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L41

	sub esp,12
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L41:

	sub esp,4
	call parse_multiplicative_expression
	add esp,4
	mov [ebp-4],eax

.L42:

	sub esp,4
	call peek
	add esp,4
	cmp eax,43
	sete al
	movzx eax,al
	test eax,eax
	jnz .L104
	sub esp,4
	call peek
	add esp,4
	cmp eax,45
	sete al
	movzx eax,al
	test eax,eax
	jz .L105
.L104:
	mov eax,1
	jmp .L106
.L105:
	mov eax,0
.L106:
	test eax,eax
	jz .L43


	sub esp,8
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	sub esp,4
	call pop
	add esp,4
	push eax
	call newnode
	add esp,20
	mov [ebp-8],eax

	sub esp,4
	call parse_multiplicative_expression
	add esp,4
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L42

.L43:

	mov eax,[ebp-4]
	add esp,20
	pop ebp
	ret
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 97
	db 100
	db 100
	db 105
	db 116
	db 105
	db 118
	db 101
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0


global parse_multiplicative_expression:function (parse_multiplicative_expression.end-parse_multiplicative_expression)
parse_multiplicative_expression:
	push ebp
	mov ebp,esp
	sub esp,20

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L44

	sub esp,12
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L44:

	sub esp,4
	call parse_cast_expression
	add esp,4
	mov [ebp-4],eax

.L45:

	sub esp,4
	call peek
	add esp,4
	cmp eax,42
	sete al
	movzx eax,al
	test eax,eax
	jnz .L107
	sub esp,4
	call peek
	add esp,4
	cmp eax,47
	sete al
	movzx eax,al
	test eax,eax
	jz .L108
.L107:
	mov eax,1
	jmp .L109
.L108:
	mov eax,0
.L109:
	test eax,eax
	jz .L46


	sub esp,8
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	sub esp,4
	call pop
	add esp,4
	push eax
	call newnode
	add esp,20
	mov [ebp-8],eax

	sub esp,4
	call parse_cast_expression
	add esp,4
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L45

.L46:

	mov eax,[ebp-4]
	add esp,20
	pop ebp
	ret
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 109
	db 117
	db 108
	db 116
	db 105
	db 112
	db 108
	db 105
	db 99
	db 97
	db 116
	db 105
	db 118
	db 101
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0


global parse_cast_expression:function (parse_cast_expression.end-parse_cast_expression)
parse_cast_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L47

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L47:

	sub esp,8
	call peek
	add esp,8
	cmp eax,40
	sete al
	movzx eax,al
	test eax,eax
	jz .L48


	sub esp,8
	call pop
	add esp,8

	sub esp,8
	call peek
	add esp,8
	cmp eax,144
	je .L5
	cmp eax,145
	je .L6
	cmp eax,146
	je .L8
	cmp eax,147
	je .L7
	cmp eax,148
	je .L9
	cmp eax,149
	je .L10
	jmp .L11


.L5:


.L6:


.L7:


.L8:


.L9:


.L10:


	sub esp,8
	call parse_type_name
	add esp,8
	mov [ebp-4],eax

	sub esp,8
	call peek
	add esp,8
	cmp eax,41
	setne al
	movzx eax,al
	test eax,eax
	jz .L51


	sub esp,12
	mov eax,[line_number]
	push eax
	sub esp,8
	call peek
	add esp,8
	push eax
	mov eax,.S1
	push eax
	call parser_error
	add esp,24

	jmp .L52

.L51:

	sub esp,8
	call pop
	add esp,8

.L52:

	sub esp,8
	call parse_cast_expression
	add esp,8
	mov [ebp-8],eax

	sub esp,12
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	mov eax,416
	push eax
	call newnode
	add esp,24
	mov [ebp-12],eax

	mov eax,[ebp-8]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-12]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-12]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-12]
	add esp,16
	pop ebp
	ret

.L11:


	sub esp,4
	mov eax,40
	push eax
	call push
	add esp,8

	sub esp,8
	call parse_unary_expression
	add esp,8
	add esp,16
	pop ebp
	ret

.L50:

	jmp .L49

.L48:

	sub esp,8
	call parse_unary_expression
	add esp,8
	add esp,16
	pop ebp
	ret

.L49:
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 99
	db 97
	db 115
	db 116
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0

.S1:
	db 69
	db 120
	db 112
	db 101
	db 99
	db 116
	db 101
	db 100
	db 32
	db 39
	db 41
	db 39
	db 32
	db 110
	db 111
	db 116
	db 32
	db 39
	db 37
	db 84
	db 39
	db 0


global parse_unary_expression:function (parse_unary_expression.end-parse_unary_expression)
parse_unary_expression:
	push ebp
	mov ebp,esp
	sub esp,20

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L53

	sub esp,12
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L53:

	sub esp,4
	call peek
	add esp,4
	mov [ebp-4],eax

	mov eax,[ebp-4]
	cmp eax,45
	sete al
	movzx eax,al
	test eax,eax
	jnz .L119
	mov eax,[ebp-4]
	cmp eax,126
	sete al
	movzx eax,al
	test eax,eax
	jz .L120
.L119:
	mov eax,1
	jmp .L121
.L120:
	mov eax,0
.L121:
	test eax,eax
	jnz .L116
	mov eax,[ebp-4]
	cmp eax,33
	sete al
	movzx eax,al
	test eax,eax
	jz .L117
.L116:
	mov eax,1
	jmp .L118
.L117:
	mov eax,0
.L118:
	test eax,eax
	jnz .L113
	mov eax,[ebp-4]
	cmp eax,38
	sete al
	movzx eax,al
	test eax,eax
	jz .L114
.L113:
	mov eax,1
	jmp .L115
.L114:
	mov eax,0
.L115:
	test eax,eax
	jnz .L110
	mov eax,[ebp-4]
	cmp eax,42
	sete al
	movzx eax,al
	test eax,eax
	jz .L111
.L110:
	mov eax,1
	jmp .L112
.L111:
	mov eax,0
.L112:
	test eax,eax
	jz .L54


	sub esp,4
	call pop
	add esp,4

	mov eax,[ebp-4]
	cmp eax,45
	je .L12
	cmp eax,38
	je .L13
	cmp eax,42
	je .L14
	jmp .L15


.L12:

	mov eax,400
	mov [ebp-8],eax

	jmp .L56

.L13:

	mov eax,410
	mov [ebp-8],eax

	jmp .L56

.L14:

	mov eax,411
	mov [ebp-8],eax

	jmp .L56

.L15:

	mov eax,[ebp-4]
	mov [ebp-8],eax

	jmp .L56

.L56:

	sub esp,8
	mov eax,1
	push eax
	mov eax,[line_number]
	push eax
	mov eax,[ebp-8]
	push eax
	call newnode
	add esp,20
	mov [ebp-12],eax

	sub esp,4
	call parse_unary_expression
	add esp,4
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-12]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-12]
	add esp,20
	pop ebp
	ret

	jmp .L55

.L54:


	mov eax,[ebp-4]
	cmp eax,180
	sete al
	movzx eax,al
	test eax,eax
	jz .L57


	sub esp,4
	call pop
	add esp,4

	sub esp,4
	call peek
	add esp,4
	cmp eax,40
	setne al
	movzx eax,al
	test eax,eax
	jz .L59


	sub esp,12
	mov eax,[line_number]
	push eax
	mov eax,.S1
	push eax
	call error
	add esp,20

	mov eax,0
	add esp,20
	pop ebp
	ret

.L59:

	sub esp,4
	call pop
	add esp,4

	sub esp,4
	call parse_type_name
	add esp,4
	mov [ebp-8],eax

	sub esp,4
	call peek
	add esp,4
	cmp eax,41
	setne al
	movzx eax,al
	test eax,eax
	jz .L60


	sub esp,8
	mov eax,[line_number]
	push eax
	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,.S2
	push eax
	call parser_error
	add esp,20

	mov eax,0
	add esp,20
	pop ebp
	ret

.L60:

	sub esp,4
	call pop
	add esp,4

	sub esp,8
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	mov eax,412
	push eax
	call newnode
	add esp,20
	mov [ebp-12],eax

	mov eax,[ebp-8]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-12]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-12]
	add esp,20
	pop ebp
	ret

	jmp .L58

.L57:

	sub esp,4
	call parse_postfix_expression
	add esp,4
	add esp,20
	pop ebp
	ret

.L58:

.L55:
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 117
	db 110
	db 97
	db 114
	db 121
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0

.S1:
	db 79
	db 110
	db 108
	db 121
	db 32
	db 39
	db 115
	db 105
	db 122
	db 101
	db 111
	db 102
	db 32
	db 40
	db 32
	db 60
	db 97
	db 98
	db 115
	db 116
	db 114
	db 97
	db 99
	db 116
	db 45
	db 116
	db 121
	db 112
	db 101
	db 62
	db 32
	db 41
	db 39
	db 32
	db 105
	db 115
	db 32
	db 99
	db 117
	db 114
	db 114
	db 101
	db 110
	db 116
	db 108
	db 121
	db 32
	db 115
	db 117
	db 112
	db 112
	db 111
	db 114
	db 116
	db 101
	db 100
	db 0

.S2:
	db 69
	db 120
	db 112
	db 101
	db 99
	db 116
	db 101
	db 100
	db 32
	db 39
	db 41
	db 39
	db 32
	db 110
	db 111
	db 116
	db 32
	db 37
	db 84
	db 32
	db 102
	db 111
	db 114
	db 32
	db 115
	db 105
	db 122
	db 101
	db 111
	db 102
	db 0


global parse_postfix_expression:function (parse_postfix_expression.end-parse_postfix_expression)
parse_postfix_expression:
	push ebp
	mov ebp,esp
	sub esp,24

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L61

	sub esp,8
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,16

.L61:

	sub esp,0
	call parse_primary_expression
	add esp,0
	mov [ebp-4],eax

	sub esp,0
	call peek
	add esp,0
	mov [ebp-12],eax

.L62:

	mov eax,1
	test eax,eax
	jz .L64


	mov eax,[ebp-12]
	cmp eax,40
	sete al
	movzx eax,al
	test eax,eax
	jz .L65


	sub esp,0
	call pop
	add esp,0

	sub esp,0
	call parse_expression_list
	add esp,0
	mov [ebp-16],eax

	sub esp,0
	call peek
	add esp,0
	cmp eax,41
	setne al
	movzx eax,al
	test eax,eax
	jz .L67

	sub esp,4
	mov eax,[line_number]
	push eax
	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,.S1
	push eax
	call parser_error
	add esp,16

.L67:

	sub esp,0
	call pop
	add esp,0

	sub esp,4
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	mov eax,408
	push eax
	call newnode
	add esp,16
	mov [ebp-8],eax

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-16]
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L66

.L65:


	mov eax,[ebp-12]
	cmp eax,91
	sete al
	movzx eax,al
	test eax,eax
	jz .L68


	sub esp,0
	call pop
	add esp,0

	sub esp,0
	call parse_expression
	add esp,0
	mov [ebp-16],eax

	sub esp,0
	call peek
	add esp,0
	cmp eax,93
	setne al
	movzx eax,al
	test eax,eax
	jz .L70

	sub esp,4
	mov eax,[line_number]
	push eax
	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,.S2
	push eax
	call parser_error
	add esp,16

.L70:

	sub esp,0
	call pop
	add esp,0

	sub esp,4
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	mov eax,413
	push eax
	call newnode
	add esp,16
	mov [ebp-8],eax

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-16]
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L69

.L68:


	mov eax,[ebp-12]
	cmp eax,46
	sete al
	movzx eax,al
	test eax,eax
	jz .L71


	sub esp,0
	call pop
	add esp,0

	sub esp,0
	call peek
	add esp,0
	cmp eax,129
	setne al
	movzx eax,al
	test eax,eax
	jz .L73

	sub esp,4
	mov eax,[line_number]
	push eax
	sub esp,8
	call peek
	add esp,8
	push eax
	mov eax,.S3
	push eax
	call parser_error
	add esp,16

.L73:

	sub esp,0
	call pop
	add esp,0

	sub esp,12
	mov eax,lex_buffer
	push eax
	call strdup
	add esp,16
	mov [ebp-16],eax

	sub esp,4
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	mov eax,414
	push eax
	call newnode
	add esp,16
	mov [ebp-8],eax

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-16]
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L72

.L71:


	mov eax,[ebp-12]
	cmp eax,198
	sete al
	movzx eax,al
	test eax,eax
	jz .L74


	sub esp,0
	call pop
	add esp,0

	sub esp,0
	call peek
	add esp,0
	cmp eax,129
	setne al
	movzx eax,al
	test eax,eax
	jz .L76

	sub esp,4
	mov eax,[line_number]
	push eax
	sub esp,8
	call peek
	add esp,8
	push eax
	mov eax,.S4
	push eax
	call parser_error
	add esp,16

.L76:

	sub esp,0
	call pop
	add esp,0

	sub esp,12
	mov eax,lex_buffer
	push eax
	call strdup
	add esp,16
	mov [ebp-16],eax

	sub esp,4
	mov eax,2
	push eax
	mov eax,[line_number]
	push eax
	mov eax,415
	push eax
	call newnode
	add esp,16
	mov [ebp-8],eax

	mov eax,[ebp-4]
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-16]
	push eax
	mov eax,1
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-8]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L75

.L74:

	jmp .L64

.L75:

.L72:

.L69:

.L66:

.L63:

	sub esp,0
	call peek
	add esp,0
	mov [ebp-12],eax

	jmp .L62

.L64:

	mov eax,[ebp-4]
	add esp,24
	pop ebp
	ret
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 112
	db 111
	db 115
	db 116
	db 102
	db 105
	db 120
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0

.S1:
	db 69
	db 120
	db 112
	db 101
	db 99
	db 116
	db 101
	db 100
	db 32
	db 39
	db 41
	db 39
	db 32
	db 110
	db 111
	db 116
	db 32
	db 37
	db 84
	db 0

.S2:
	db 69
	db 120
	db 112
	db 101
	db 99
	db 116
	db 101
	db 100
	db 32
	db 39
	db 93
	db 39
	db 32
	db 110
	db 111
	db 116
	db 32
	db 37
	db 84
	db 0

.S3:
	db 69
	db 120
	db 112
	db 101
	db 99
	db 116
	db 101
	db 100
	db 32
	db 105
	db 100
	db 101
	db 110
	db 116
	db 105
	db 102
	db 105
	db 101
	db 114
	db 32
	db 110
	db 111
	db 116
	db 32
	db 37
	db 84
	db 0

.S4:
	db 69
	db 120
	db 112
	db 101
	db 99
	db 116
	db 101
	db 100
	db 32
	db 105
	db 100
	db 101
	db 110
	db 116
	db 105
	db 102
	db 105
	db 101
	db 114
	db 32
	db 110
	db 111
	db 116
	db 32
	db 37
	db 84
	db 0


global parse_expression_list:function (parse_expression_list.end-parse_expression_list)
parse_expression_list:
	push ebp
	mov ebp,esp
	sub esp,20

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L77

	sub esp,12
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L77:

	sub esp,4
	call peek
	add esp,4
	cmp eax,41
	sete al
	movzx eax,al
	test eax,eax
	jz .L78

	mov eax,0
	add esp,20
	pop ebp
	ret

.L78:

	sub esp,4
	call parse_assignment_expression
	add esp,4
	mov [ebp-4],eax

	sub esp,4
	call peek
	add esp,4
	cmp eax,44
	sete al
	movzx eax,al
	test eax,eax
	jz .L79

	sub esp,4
	call pop
	add esp,4

	jmp .L80

.L79:


	sub esp,4
	call peek
	add esp,4
	cmp eax,41
	setne al
	movzx eax,al
	test eax,eax
	jz .L81

	sub esp,8
	mov eax,[line_number]
	push eax
	sub esp,8
	call peek
	add esp,8
	push eax
	mov eax,.S1
	push eax
	call parser_error
	add esp,20

.L81:

.L80:

	sub esp,4
	sub esp,0
	call parse_expression_list
	add esp,0
	push eax
	mov eax,[ebp-4]
	push eax
	mov eax,[line_number]
	push eax
	mov eax,409
	push eax
	call newastlist
	add esp,20
	add esp,20
	pop ebp
	ret
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 32
	db 108
	db 105
	db 115
	db 116
	db 32
	db 10
	db 0

.S1:
	db 85
	db 110
	db 101
	db 120
	db 112
	db 101
	db 99
	db 116
	db 101
	db 100
	db 32
	db 115
	db 121
	db 109
	db 98
	db 111
	db 108
	db 32
	db 39
	db 37
	db 84
	db 39
	db 32
	db 105
	db 110
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 32
	db 108
	db 105
	db 115
	db 116
	db 0


global parse_primary_expression:function (parse_primary_expression.end-parse_primary_expression)
parse_primary_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L82

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L82:

	sub esp,8
	call peek
	add esp,8
	cmp eax,128
	sete al
	movzx eax,al
	test eax,eax
	jz .L83


	sub esp,8
	call pop
	add esp,8

	sub esp,12
	mov eax,[lex_number]
	push eax
	mov eax,[line_number]
	push eax
	mov eax,305
	push eax
	call newconst
	add esp,24
	add esp,16
	pop ebp
	ret

	jmp .L84

.L83:


	sub esp,8
	call peek
	add esp,8
	cmp eax,129
	sete al
	movzx eax,al
	test eax,eax
	jz .L85


	sub esp,8
	call pop
	add esp,8

	sub esp,12
	mov eax,1
	push eax
	mov eax,[line_number]
	push eax
	mov eax,300
	push eax
	call newnode
	add esp,24
	mov [ebp-4],eax

	sub esp,4
	mov eax,lex_buffer
	push eax
	call strdup
	add esp,8
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-4]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret

	jmp .L86

.L85:


	sub esp,8
	call peek
	add esp,8
	cmp eax,40
	sete al
	movzx eax,al
	test eax,eax
	jz .L87


	sub esp,8
	call pop
	add esp,8

	sub esp,8
	call parse_expression
	add esp,8
	mov [ebp-4],eax

	sub esp,8
	call peek
	add esp,8
	cmp eax,41
	setne al
	movzx eax,al
	test eax,eax
	jz .L89


	sub esp,12
	mov eax,[line_number]
	push eax
	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,.S1
	push eax
	call parser_error
	add esp,24

	mov eax,0
	add esp,16
	pop ebp
	ret

.L89:

	sub esp,8
	call pop
	add esp,8

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret

	jmp .L88

.L87:


	sub esp,8
	call peek
	add esp,8
	cmp eax,130
	sete al
	movzx eax,al
	test eax,eax
	jz .L90


	sub esp,8
	call pop
	add esp,8

	sub esp,12
	mov eax,1
	push eax
	mov eax,[line_number]
	push eax
	mov eax,306
	push eax
	call newnode
	add esp,24
	mov [ebp-4],eax

	sub esp,4
	mov eax,lex_buffer
	push eax
	call strdup
	add esp,8
	push eax
	mov eax,0
	imul eax,4
	push eax
	mov eax,16
	add eax,[ebp-4]
	pop ecx
	add eax,ecx
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret

	jmp .L91

.L90:


	sub esp,12
	mov eax,[line_number]
	push eax
	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,.S2
	push eax
	call parser_error
	add esp,24

	mov eax,0
	add esp,16
	pop ebp
	ret

.L91:

.L88:

.L86:

.L84:
.end:

.S0:
	db 9
	db 80
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 32
	db 112
	db 114
	db 105
	db 109
	db 97
	db 114
	db 121
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 10
	db 0

.S1:
	db 69
	db 120
	db 112
	db 101
	db 99
	db 116
	db 101
	db 100
	db 32
	db 39
	db 41
	db 39
	db 32
	db 110
	db 111
	db 116
	db 32
	db 37
	db 84
	db 32
	db 97
	db 102
	db 116
	db 101
	db 114
	db 32
	db 39
	db 40
	db 39
	db 0

.S2:
	db 69
	db 120
	db 112
	db 101
	db 99
	db 116
	db 101
	db 100
	db 32
	db 112
	db 114
	db 105
	db 109
	db 97
	db 114
	db 121
	db 32
	db 101
	db 120
	db 112
	db 114
	db 101
	db 115
	db 115
	db 105
	db 111
	db 110
	db 32
	db 110
	db 111
	db 116
	db 32
	db 37
	db 84
	db 0

section .data
	common PARSER_DEBUG 4:4
	extern parse_type_name
	extern push
	extern pop
	extern peek
	extern strdup
	extern newastlist
	extern newconst
	extern newnode
	extern local_array_size
	extern free_type
	extern print_type
	extern align_type
	extern binary_type_promotion
	extern unary_type_promotion
	extern is_compatible_type
	extern is_returnable_type
	extern is_arithmatic_type
	extern check_declaration_type
	extern target_sizeof
	extern compare_type
	extern find_type_identifier
	extern insert_typelist
	extern replace_typelist
	extern append_typelist
	extern append_type
	extern new_type
	extern parser_error
	extern lexer_error
	extern error
	extern lex
	common lex_number 4:4
	common lex_buffer 128:4
	common line_number 4:4
	extern token_to_string
	extern fscanf
	extern fputc
	extern fputs
	extern puts
	extern sprintf
	extern snprintf
	extern fprintf
	extern printf
	extern ungetc
	extern getchar
	extern fgetc
	extern getc
	extern fopen
	common stderr 4:4
	common stdout 4:4
	common stdin 4:4
