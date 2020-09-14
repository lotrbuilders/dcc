section .text

global peek:function (peek.end-peek)
peek:
	push ebp
	mov ebp,esp

	mov eax,[tk]
	cmp eax,0
	sete al
	movzx eax,al
	test eax,eax
	jz .L0

	sub esp,8
	call lex
	add esp,8
	mov [tk],eax

.L0:

	mov eax,[PEEK_DEBUG]
	test eax,eax
	jz .L1

	sub esp,12
	mov eax,[tk]
	push eax
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,24

.L1:

	mov eax,[tk]
	pop ebp
	ret
.end:

.S0:
	db 9
	db 112
	db 101
	db 101
	db 107
	db 32
	db 105
	db 110
	db 116
	db 111
	db 32
	db 116
	db 107
	db 58
	db 32
	db 37
	db 100
	db 10
	db 0


global pop:function (pop.end-pop)
pop:
	push ebp
	mov ebp,esp
	sub esp,4

	mov eax,[tk]
	cmp eax,0
	sete al
	movzx eax,al
	test eax,eax
	jz .L2


	sub esp,4
	call lex
	add esp,4
	mov [ebp-4],eax

	mov eax,[PEEK_DEBUG]
	test eax,eax
	jz .L4

	sub esp,8
	mov eax,[ebp-4]
	push eax
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L4:

	mov eax,[ebp-4]
	add esp,4
	pop ebp
	ret

	jmp .L3

.L2:


	mov eax,[tk]
	mov [ebp-4],eax

	mov eax,0
	mov [tk],eax

	mov eax,[PEEK_DEBUG]
	test eax,eax
	jz .L5

	sub esp,8
	mov eax,[ebp-4]
	push eax
	mov eax,.S1
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L5:

	mov eax,[ebp-4]
	add esp,4
	pop ebp
	ret

.L3:
.end:

.S0:
	db 9
	db 112
	db 111
	db 112
	db 32
	db 116
	db 107
	db 58
	db 32
	db 37
	db 100
	db 10
	db 0

.S1:
	db 9
	db 112
	db 111
	db 112
	db 32
	db 116
	db 107
	db 58
	db 32
	db 37
	db 100
	db 10
	db 0


global expect:function (expect.end-expect)
expect:
	push ebp
	mov ebp,esp
	sub esp,4

	sub esp,4
	call pop
	add esp,4
	push eax
	mov eax,[ebp+8]
	pop ecx
	cmp eax,ecx
	setne al
	movzx eax,al
	test eax,eax
	jz .L6

	sub esp,0
	mov eax,.S0
	push eax
	call error
	add esp,4

.L6:

	mov eax,[ebp+8]
	add esp,4
	pop ebp
	ret
.end:

.S0:
	db 117
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
	db 116
	db 111
	db 107
	db 101
	db 110
	db 0


global parse:function (parse.end-parse)
parse:
	push ebp
	mov ebp,esp
	sub esp,4

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L7

	sub esp,12
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L7:

	sub esp,4
	call parse_translation_unit
	add esp,4
	add esp,4
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 105
	db 110
	db 103
	db 10
	db 0


global parse_translation_unit:function (parse_translation_unit.end-parse_translation_unit)
parse_translation_unit:
	push ebp
	mov ebp,esp
	sub esp,12

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L8

	sub esp,4
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,12

.L8:

	sub esp,8
	mov eax,3
	push eax
	call newnode
	add esp,12
	mov [ebp-4],eax

	mov eax,[ebp-4]
	mov [ebp-8],eax

.L9:

	sub esp,12
	call peek
	add esp,12
	cmp eax,[SYM_EOF]
	setne al
	movzx eax,al
	test eax,eax
	jz .L10


	mov eax,[SYM_TRANSLATION_UNIT]
	push eax
	mov eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	sub esp,12
	call parse_global
	add esp,12
	push eax
	mov eax,4
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	sub esp,8
	mov eax,3
	push eax
	call newnode
	add esp,12
	push eax
	mov eax,8
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,8
	add eax,[ebp-8]
	mov eax,[eax]
	mov [ebp-8],eax

	jmp .L9

.L10:

	mov eax,[ebp-4]
	add esp,12
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 95
	db 116
	db 114
	db 97
	db 110
	db 115
	db 108
	db 97
	db 116
	db 105
	db 111
	db 110
	db 95
	db 117
	db 110
	db 105
	db 116
	db 10
	db 0


global parse_global:function (parse_global.end-parse_global)
parse_global:
	push ebp
	mov ebp,esp
	sub esp,28

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L11

	sub esp,4
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,12

.L11:

	mov eax,0
	mov [ebp-24],eax

	sub esp,8
	mov eax,5
	push eax
	call newnode
	add esp,12
	mov [ebp-16],eax

	sub esp,12
	call parse_decleration
	add esp,12
	mov [ebp-8],eax

	mov eax,[ebp-8]
	cmp eax,0
	sete al
	movzx eax,al
	test eax,eax
	jz .L12

	sub esp,8
	mov eax,.S1
	push eax
	call error
	add esp,12

.L12:

	mov eax,8
	add eax,[ebp-8]
	mov eax,[eax]
	mov [ebp-4],eax

	mov eax,4
	add eax,[ebp-8]
	mov eax,[eax]
	mov [ebp-12],eax

	sub esp,12
	call peek
	add esp,12
	cmp eax,59
	sete al
	movzx eax,al
	test eax,eax
	jz .L13


	sub esp,12
	call pop
	add esp,12

	mov eax,[SYM_GLOBAL_DECL]
	push eax
	mov eax,[ebp-16]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,4
	add eax,[ebp-16]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-12]
	push eax
	mov eax,16
	add eax,[ebp-16]
	pop ecx
	mov [eax],ecx

	jmp .L14

.L13:


	sub esp,12
	call peek
	add esp,12
	cmp eax,61
	sete al
	movzx eax,al
	test eax,eax
	jz .L15


	sub esp,12
	call pop
	add esp,12

	mov eax,[SYM_GLOBAL_DEF]
	push eax
	mov eax,[ebp-16]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,4
	add eax,[ebp-16]
	pop ecx
	mov [eax],ecx

	sub esp,12
	call parse_expression
	add esp,12
	push eax
	mov eax,8
	add eax,[ebp-16]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-12]
	push eax
	mov eax,16
	add eax,[ebp-16]
	pop ecx
	mov [eax],ecx

	sub esp,8
	mov eax,59
	push eax
	call expect
	add esp,12

	jmp .L16

.L15:


	sub esp,8
	mov eax,40
	push eax
	call expect
	add esp,12

	sub esp,12
	call parse_decleration_list
	add esp,12
	mov [ebp-20],eax

	sub esp,8
	mov eax,41
	push eax
	call expect
	add esp,12

	sub esp,12
	call peek
	add esp,12
	cmp eax,123
	sete al
	movzx eax,al
	test eax,eax
	jz .L17


	mov eax,[SYM_FUNC_DEF]
	push eax
	mov eax,[ebp-16]
	pop ecx
	mov [eax],ecx

	sub esp,8
	mov eax,123
	push eax
	call expect
	add esp,12

	sub esp,12
	call parse_statement_list
	add esp,12
	mov [ebp-24],eax

	sub esp,8
	mov eax,125
	push eax
	call expect
	add esp,12

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L19

	sub esp,0
	mov eax,[ebp-4]
	push eax
	mov eax,.S2
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,12

.L19:

	jmp .L18

.L17:


	sub esp,8
	mov eax,59
	push eax
	call expect
	add esp,12

	mov eax,[SYM_FUNC_DECL]
	push eax
	mov eax,[ebp-16]
	pop ecx
	mov [eax],ecx

.L18:

	mov eax,[ebp-4]
	push eax
	mov eax,4
	add eax,[ebp-16]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-24]
	push eax
	mov eax,8
	add eax,[ebp-16]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-20]
	push eax
	mov eax,12
	add eax,[ebp-16]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-12]
	push eax
	mov eax,16
	add eax,[ebp-16]
	pop ecx
	mov [eax],ecx

.L16:

.L14:

	mov eax,[ebp-16]
	add esp,28
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 103
	db 108
	db 111
	db 98
	db 97
	db 108
	db 10
	db 0

.S1:
	db 66
	db 97
	db 100
	db 32
	db 100
	db 101
	db 99
	db 108
	db 101
	db 114
	db 97
	db 116
	db 105
	db 111
	db 110
	db 0

.S2:
	db 102
	db 117
	db 110
	db 99
	db 116
	db 105
	db 111
	db 110
	db 32
	db 37
	db 115
	db 10
	db 0


global parse_decleration_list:function (parse_decleration_list.end-parse_decleration_list)
parse_decleration_list:
	push ebp
	mov ebp,esp
	sub esp,8

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L20

	sub esp,8
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,16

.L20:

	sub esp,0
	call peek
	add esp,0
	cmp eax,41
	sete al
	movzx eax,al
	test eax,eax
	jz .L21

	mov eax,0
	add esp,8
	pop ebp
	ret

	jmp .L22

.L21:


	sub esp,12
	mov eax,3
	push eax
	call newnode
	add esp,16
	mov [ebp-4],eax

	mov eax,[SYM_DECL_LIST]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,0
	call parse_decleration
	add esp,0
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,0
	call peek
	add esp,0
	cmp eax,44
	sete al
	movzx eax,al
	test eax,eax
	jz .L23


	sub esp,0
	call pop
	add esp,0

	sub esp,0
	call parse_decleration_list
	add esp,0
	push eax
	mov eax,8
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	jmp .L24

.L23:

	mov eax,0
	push eax
	mov eax,8
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

.L24:

	mov eax,[ebp-4]
	add esp,8
	pop ebp
	ret

.L22:
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 100
	db 101
	db 99
	db 108
	db 101
	db 114
	db 97
	db 116
	db 105
	db 111
	db 110
	db 45
	db 108
	db 105
	db 115
	db 116
	db 10
	db 0


global parse_statement_list:function (parse_statement_list.end-parse_statement_list)
parse_statement_list:
	push ebp
	mov ebp,esp
	sub esp,12

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L25

	sub esp,4
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,12

.L25:

	sub esp,12
	call peek
	add esp,12
	cmp eax,125
	setne al
	movzx eax,al
	test eax,eax
	jz .L26


	sub esp,12
	call parse_statement
	add esp,12
	mov [ebp-8],eax

	sub esp,8
	mov eax,3
	push eax
	call newnode
	add esp,12
	mov [ebp-4],eax

	mov eax,[SYM_STATEMENT_LIST]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,12
	call parse_statement_list
	add esp,12
	push eax
	mov eax,8
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	add esp,12
	pop ebp
	ret

	jmp .L27

.L26:

	mov eax,0
	add esp,12
	pop ebp
	ret

.L27:
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 115
	db 116
	db 97
	db 116
	db 101
	db 109
	db 101
	db 110
	db 116
	db 45
	db 108
	db 105
	db 115
	db 116
	db 10
	db 0


global parse_statement:function (parse_statement.end-parse_statement)
parse_statement:
	push ebp
	mov ebp,esp
	sub esp,12

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L28

	sub esp,4
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,12

.L28:

	sub esp,12
	call peek
	add esp,12
	mov [ebp-8],eax

	mov eax,[ebp-8]
	cmp eax,[SYM_RETURN]
	sete al
	movzx eax,al
	test eax,eax
	jz .L29

	sub esp,12
	call parse_jump_statement
	add esp,12
	mov [ebp-4],eax

	jmp .L30

.L29:


	mov eax,[ebp-8]
	cmp eax,[SYM_INT]
	sete al
	movzx eax,al
	test eax,eax
	jnz .L119
	mov eax,[ebp-8]
	cmp eax,[SYM_CHAR]
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
	jz .L31

	sub esp,12
	call parse_local_decleration
	add esp,12
	mov [ebp-4],eax

	jmp .L32

.L31:


	mov eax,[ebp-8]
	cmp eax,[SYM_IF]
	sete al
	movzx eax,al
	test eax,eax
	jz .L33

	sub esp,12
	call parse_selection_statement
	add esp,12
	mov [ebp-4],eax

	jmp .L34

.L33:


	mov eax,[ebp-8]
	cmp eax,[SYM_FOR]
	sete al
	movzx eax,al
	test eax,eax
	jnz .L122
	mov eax,[ebp-8]
	cmp eax,[SYM_WHILE]
	sete al
	movzx eax,al
	test eax,eax
	jz .L123
.L122:
	mov eax,1
	jmp .L124
.L123:
	mov eax,0
.L124:
	test eax,eax
	jz .L35

	sub esp,12
	call parse_iteration_statement
	add esp,12
	mov [ebp-4],eax

	jmp .L36

.L35:


	mov eax,[ebp-8]
	cmp eax,123
	sete al
	movzx eax,al
	test eax,eax
	jz .L37


	sub esp,8
	mov eax,2
	push eax
	call newnode
	add esp,12
	mov [ebp-4],eax

	mov eax,[SYM_COMPOUND_STATEMENT]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,12
	call pop
	add esp,12

	sub esp,12
	call parse_statement_list
	add esp,12
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,8
	mov eax,125
	push eax
	call expect
	add esp,12

	jmp .L38

.L37:


	sub esp,12
	call parse_expression
	add esp,12
	mov [ebp-4],eax

	sub esp,8
	mov eax,59
	push eax
	call expect
	add esp,12

.L38:

.L36:

.L34:

.L32:

.L30:

	mov eax,[ebp-4]
	add esp,12
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 115
	db 116
	db 97
	db 116
	db 101
	db 109
	db 101
	db 110
	db 116
	db 10
	db 0


global parse_local_decleration:function (parse_local_decleration.end-parse_local_decleration)
parse_local_decleration:
	push ebp
	mov ebp,esp
	sub esp,8

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L39

	sub esp,8
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,16

.L39:

	sub esp,12
	mov eax,3
	push eax
	call newnode
	add esp,16
	mov [ebp-4],eax

	mov eax,[SYM_LOCAL_DECLARATION]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,0
	call parse_decleration
	add esp,0
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,0
	call peek
	add esp,0
	cmp eax,61
	sete al
	movzx eax,al
	test eax,eax
	jz .L40


	sub esp,0
	call pop
	add esp,0

	sub esp,0
	call parse_expression
	add esp,0
	push eax
	mov eax,8
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

.L40:

	sub esp,12
	mov eax,59
	push eax
	call expect
	add esp,16

	mov eax,[ebp-4]
	add esp,8
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 108
	db 111
	db 99
	db 97
	db 108
	db 32
	db 100
	db 101
	db 99
	db 108
	db 101
	db 114
	db 97
	db 116
	db 105
	db 111
	db 110
	db 10
	db 0


global parse_decleration:function (parse_decleration.end-parse_decleration)
parse_decleration:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L41

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L41:

	mov eax,0
	mov [ebp-4],eax

	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_INT]
	sete al
	movzx eax,al
	test eax,eax
	jnz .L125
	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_CHAR]
	sete al
	movzx eax,al
	test eax,eax
	jz .L126
.L125:
	mov eax,1
	jmp .L127
.L126:
	mov eax,0
.L127:
	test eax,eax
	jz .L42


	sub esp,8
	call pop
	add esp,8
	mov [ebp-12],eax

	sub esp,8
	call parse_declerator
	add esp,8
	mov [ebp-8],eax

	sub esp,0
	mov eax,[ebp-12]
	push eax
	mov eax,[ebp-8]
	push eax
	call declarator_type
	add esp,8
	mov [ebp-12],eax

	sub esp,4
	mov eax,[ebp-8]
	push eax
	call declarator_name
	add esp,8
	mov [ebp-16],eax

	sub esp,4
	mov eax,4
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	mov eax,[SYM_DECLARATION]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-12]
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-16]
	push eax
	mov eax,8
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	push eax
	mov eax,12
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

.L42:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 100
	db 101
	db 99
	db 108
	db 101
	db 114
	db 97
	db 116
	db 105
	db 111
	db 110
	db 10
	db 0


global parse_declerator:function (parse_declerator.end-parse_declerator)
parse_declerator:
	push ebp
	mov ebp,esp
	sub esp,20

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L43

	sub esp,12
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,20

.L43:

	mov eax,0
	mov [ebp-4],eax

	mov eax,0
	mov [ebp-8],eax

.L44:

	sub esp,4
	call peek
	add esp,4
	cmp eax,42
	sete al
	movzx eax,al
	test eax,eax
	jz .L45


	sub esp,4
	call pop
	add esp,4

	mov eax,1
	add eax,[ebp-8]
	mov [ebp-8],eax

	jmp .L44

.L45:

	sub esp,0
	mov eax,3
	push eax
	call newnode
	add esp,4
	mov [ebp-4],eax

	mov eax,[SYM_DECLARATOR]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,0
	mov eax,[SYM_ID]
	push eax
	call expect
	add esp,4

	sub esp,0
	mov eax,[lexed_string]
	push eax
	call i_strdup
	add esp,4
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	push eax
	mov eax,8
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	add esp,20
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 100
	db 101
	db 99
	db 108
	db 101
	db 114
	db 97
	db 116
	db 111
	db 114
	db 10
	db 0


global parse_jump_statement:function (parse_jump_statement.end-parse_jump_statement)
parse_jump_statement:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L46

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L46:

	mov eax,0
	mov [ebp-4],eax

	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_RETURN]
	sete al
	movzx eax,al
	test eax,eax
	jz .L47


	sub esp,8
	call pop
	add esp,8

	sub esp,8
	call parse_expression
	add esp,8
	mov [ebp-8],eax

	mov eax,[ebp-8]
	cmp eax,0
	setne al
	movzx eax,al
	test eax,eax
	jz .L48


	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L50

	sub esp,0
	mov eax,.S1
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L50:

	sub esp,4
	mov eax,2
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	sub esp,0
	mov eax,.S2
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

	mov eax,[SYM_RETURN]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	jmp .L49

.L48:


	sub esp,4
	mov eax,.S3
	push eax
	call error
	add esp,8

.L49:

	sub esp,4
	mov eax,59
	push eax
	call expect
	add esp,8

.L47:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 106
	db 117
	db 109
	db 112
	db 45
	db 115
	db 116
	db 97
	db 116
	db 101
	db 109
	db 101
	db 110
	db 116
	db 10
	db 0

.S1:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 114
	db 101
	db 116
	db 117
	db 114
	db 110
	db 32
	db 115
	db 116
	db 97
	db 116
	db 101
	db 109
	db 101
	db 110
	db 116
	db 10
	db 0

.S2:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 114
	db 101
	db 116
	db 117
	db 114
	db 110
	db 32
	db 115
	db 116
	db 97
	db 116
	db 101
	db 109
	db 101
	db 110
	db 116
	db 32
	db 50
	db 10
	db 0

.S3:
	db 101
	db 120
	db 112
	db 101
	db 99
	db 116
	db 101
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
	db 32
	db 97
	db 102
	db 116
	db 101
	db 114
	db 32
	db 114
	db 101
	db 116
	db 117
	db 114
	db 110
	db 0


global parse_selection_statement:function (parse_selection_statement.end-parse_selection_statement)
parse_selection_statement:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L51

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L51:

	mov eax,0
	mov [ebp-4],eax

	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_IF]
	sete al
	movzx eax,al
	test eax,eax
	jz .L52


	sub esp,8
	call pop
	add esp,8

	sub esp,4
	mov eax,40
	push eax
	call expect
	add esp,8

	sub esp,8
	call parse_expression
	add esp,8
	mov [ebp-8],eax

	sub esp,4
	mov eax,41
	push eax
	call expect
	add esp,8

	sub esp,8
	call parse_statement
	add esp,8
	mov [ebp-12],eax

	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_ELSE]
	sete al
	movzx eax,al
	test eax,eax
	jz .L53


	sub esp,8
	call pop
	add esp,8

	sub esp,8
	call parse_statement
	add esp,8
	mov [ebp-16],eax

	jmp .L54

.L53:

	mov eax,0
	mov [ebp-16],eax

.L54:

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L55

	sub esp,0
	mov eax,.S1
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L55:

	sub esp,4
	mov eax,4
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	mov eax,[SYM_IF]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-12]
	push eax
	mov eax,8
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-16]
	push eax
	mov eax,12
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

.L52:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 115
	db 101
	db 108
	db 101
	db 99
	db 116
	db 105
	db 111
	db 110
	db 45
	db 115
	db 116
	db 97
	db 116
	db 101
	db 109
	db 101
	db 110
	db 116
	db 10
	db 0

.S1:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 105
	db 102
	db 32
	db 115
	db 116
	db 97
	db 116
	db 101
	db 109
	db 101
	db 110
	db 116
	db 10
	db 0


global parse_iteration_statement:function (parse_iteration_statement.end-parse_iteration_statement)
parse_iteration_statement:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L56

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L56:

	mov eax,0
	mov [ebp-4],eax

	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_WHILE]
	sete al
	movzx eax,al
	test eax,eax
	jz .L57


	sub esp,8
	call pop
	add esp,8

	sub esp,4
	mov eax,40
	push eax
	call expect
	add esp,8

	sub esp,8
	call parse_expression
	add esp,8
	mov [ebp-8],eax

	sub esp,4
	mov eax,41
	push eax
	call expect
	add esp,8

	sub esp,8
	call parse_statement
	add esp,8
	mov [ebp-12],eax

	sub esp,4
	mov eax,3
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	mov eax,[SYM_WHILE]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-12]
	push eax
	mov eax,8
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	jmp .L58

.L57:


	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_FOR]
	sete al
	movzx eax,al
	test eax,eax
	jz .L59


	sub esp,4
	mov eax,5
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,4
	mov eax,40
	push eax
	call expect
	add esp,8

	sub esp,8
	call parse_expression
	add esp,8
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,4
	mov eax,59
	push eax
	call expect
	add esp,8

	sub esp,8
	call parse_expression
	add esp,8
	push eax
	mov eax,8
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,4
	mov eax,59
	push eax
	call expect
	add esp,8

	sub esp,8
	call parse_expression
	add esp,8
	push eax
	mov eax,12
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,4
	mov eax,41
	push eax
	call expect
	add esp,8

	sub esp,8
	call parse_statement
	add esp,8
	push eax
	mov eax,16
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

.L59:

.L58:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 105
	db 116
	db 101
	db 114
	db 97
	db 116
	db 105
	db 111
	db 110
	db 45
	db 115
	db 116
	db 97
	db 116
	db 101
	db 109
	db 101
	db 110
	db 116
	db 10
	db 0


global parse_expression:function (parse_expression.end-parse_expression)
parse_expression:
	push ebp
	mov ebp,esp
	sub esp,12

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L60

	sub esp,4
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,12

.L60:

	sub esp,12
	call parse_assignment_expression
	add esp,12
	add esp,12
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
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


global parse_assignment_expression:function (parse_assignment_expression.end-parse_assignment_expression)
parse_assignment_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L61

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L61:

	sub esp,8
	call parse_logical_or_expression
	add esp,8
	mov [ebp-4],eax

	sub esp,8
	call peek
	add esp,8
	cmp eax,61
	sete al
	movzx eax,al
	test eax,eax
	jz .L62


	sub esp,4
	mov eax,3
	push eax
	call newnode
	add esp,8
	mov [ebp-8],eax

	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,4
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_assignment_expression
	add esp,8
	push eax
	mov eax,8
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	add esp,16
	pop ebp
	ret

	jmp .L63

.L62:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret

.L63:
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
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
	db 45
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


global parse_logical_or_expression:function (parse_logical_or_expression.end-parse_logical_or_expression)
parse_logical_or_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L64

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L64:

	sub esp,8
	call parse_logical_and_expression
	add esp,8
	mov [ebp-4],eax

.L65:

	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_LOR]
	sete al
	movzx eax,al
	test eax,eax
	jz .L66


	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L67

	sub esp,12
	sub esp,12
	call peek
	add esp,12
	push eax
	mov eax,.S1
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,24

.L67:

	sub esp,4
	mov eax,3
	push eax
	call newnode
	add esp,8
	mov [ebp-8],eax

	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,4
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_logical_and_expression
	add esp,8
	push eax
	mov eax,8
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L65

.L66:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
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
	db 45
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
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 37
	db 100
	db 10
	db 0


global parse_logical_and_expression:function (parse_logical_and_expression.end-parse_logical_and_expression)
parse_logical_and_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L68

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L68:

	sub esp,8
	call parse_equality_expression
	add esp,8
	mov [ebp-4],eax

.L69:

	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_LAND]
	sete al
	movzx eax,al
	test eax,eax
	jz .L70


	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L71

	sub esp,12
	sub esp,12
	call peek
	add esp,12
	push eax
	mov eax,.S1
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,24

.L71:

	sub esp,4
	mov eax,3
	push eax
	call newnode
	add esp,8
	mov [ebp-8],eax

	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,4
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_equality_expression
	add esp,8
	push eax
	mov eax,8
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L69

.L70:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
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
	db 45
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
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 37
	db 100
	db 10
	db 0


global parse_equality_expression:function (parse_equality_expression.end-parse_equality_expression)
parse_equality_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L72

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L72:

	sub esp,8
	call parse_comparison_expression
	add esp,8
	mov [ebp-4],eax

.L73:

	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_NE]
	sete al
	movzx eax,al
	test eax,eax
	jnz .L128
	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_EQ]
	sete al
	movzx eax,al
	test eax,eax
	jz .L129
.L128:
	mov eax,1
	jmp .L130
.L129:
	mov eax,0
.L130:
	test eax,eax
	jz .L74


	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L75

	sub esp,12
	sub esp,12
	call peek
	add esp,12
	push eax
	mov eax,.S1
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,24

.L75:

	sub esp,4
	mov eax,3
	push eax
	call newnode
	add esp,8
	mov [ebp-8],eax

	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,4
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_comparison_expression
	add esp,8
	push eax
	mov eax,8
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L73

.L74:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 101
	db 113
	db 117
	db 97
	db 108
	db 105
	db 116
	db 121
	db 45
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
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 37
	db 100
	db 10
	db 0


global parse_comparison_expression:function (parse_comparison_expression.end-parse_comparison_expression)
parse_comparison_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L76

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L76:

	sub esp,8
	call parse_additive_expression
	add esp,8
	mov [ebp-4],eax

.L77:

	sub esp,8
	call peek
	add esp,8
	cmp eax,62
	sete al
	movzx eax,al
	test eax,eax
	jnz .L137
	sub esp,8
	call peek
	add esp,8
	cmp eax,60
	sete al
	movzx eax,al
	test eax,eax
	jz .L138
.L137:
	mov eax,1
	jmp .L139
.L138:
	mov eax,0
.L139:
	test eax,eax
	jnz .L134
	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_LE]
	sete al
	movzx eax,al
	test eax,eax
	jz .L135
.L134:
	mov eax,1
	jmp .L136
.L135:
	mov eax,0
.L136:
	test eax,eax
	jnz .L131
	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_GE]
	sete al
	movzx eax,al
	test eax,eax
	jz .L132
.L131:
	mov eax,1
	jmp .L133
.L132:
	mov eax,0
.L133:
	test eax,eax
	jz .L78


	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L79

	sub esp,12
	sub esp,12
	call peek
	add esp,12
	push eax
	mov eax,.S1
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,24

.L79:

	sub esp,4
	mov eax,3
	push eax
	call newnode
	add esp,8
	mov [ebp-8],eax

	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,4
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_additive_expression
	add esp,8
	push eax
	mov eax,8
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L77

.L78:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 99
	db 111
	db 109
	db 112
	db 97
	db 114
	db 105
	db 115
	db 111
	db 110
	db 45
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
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 37
	db 100
	db 10
	db 0


global parse_additive_expression:function (parse_additive_expression.end-parse_additive_expression)
parse_additive_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L80

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L80:

	sub esp,8
	call parse_multiplicative_expression
	add esp,8
	mov [ebp-4],eax

.L81:

	sub esp,8
	call peek
	add esp,8
	cmp eax,43
	sete al
	movzx eax,al
	test eax,eax
	jnz .L140
	sub esp,8
	call peek
	add esp,8
	cmp eax,45
	sete al
	movzx eax,al
	test eax,eax
	jz .L141
.L140:
	mov eax,1
	jmp .L142
.L141:
	mov eax,0
.L142:
	test eax,eax
	jz .L82


	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L83

	sub esp,12
	sub esp,12
	call peek
	add esp,12
	push eax
	mov eax,.S1
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,24

.L83:

	sub esp,4
	mov eax,3
	push eax
	call newnode
	add esp,8
	mov [ebp-8],eax

	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,4
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_multiplicative_expression
	add esp,8
	push eax
	mov eax,8
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L81

.L82:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 97
	db 100
	db 100
	db 105
	db 116
	db 105
	db 118
	db 101
	db 45
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
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 37
	db 99
	db 10
	db 0


global parse_multiplicative_expression:function (parse_multiplicative_expression.end-parse_multiplicative_expression)
parse_multiplicative_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L84

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L84:

	sub esp,8
	call parse_unary_expression
	add esp,8
	mov [ebp-4],eax

.L85:

	sub esp,8
	call peek
	add esp,8
	cmp eax,42
	sete al
	movzx eax,al
	test eax,eax
	jz .L86


	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L87

	sub esp,12
	sub esp,12
	call peek
	add esp,12
	push eax
	mov eax,.S1
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,24

.L87:

	sub esp,4
	mov eax,3
	push eax
	call newnode
	add esp,8
	mov [ebp-8],eax

	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,4
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_unary_expression
	add esp,8
	push eax
	mov eax,8
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	jmp .L85

.L86:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
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
	db 45
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
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 37
	db 99
	db 10
	db 0


global parse_unary_expression:function (parse_unary_expression.end-parse_unary_expression)
parse_unary_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L88

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L88:

	sub esp,8
	call peek
	add esp,8
	cmp eax,45
	sete al
	movzx eax,al
	test eax,eax
	jz .L89


	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L91

	sub esp,0
	mov eax,.S1
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L91:

	sub esp,8
	call pop
	add esp,8

	sub esp,4
	mov eax,2
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	mov eax,[SYM_NEGATE]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_unary_expression
	add esp,8
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	jmp .L90

.L89:


	sub esp,8
	call peek
	add esp,8
	cmp eax,42
	sete al
	movzx eax,al
	test eax,eax
	jz .L92


	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L94

	sub esp,0
	mov eax,.S2
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L94:

	sub esp,8
	call pop
	add esp,8

	sub esp,4
	mov eax,2
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	mov eax,[SYM_POINTER]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_unary_expression
	add esp,8
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	jmp .L93

.L92:


	sub esp,8
	call peek
	add esp,8
	cmp eax,38
	sete al
	movzx eax,al
	test eax,eax
	jz .L95


	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L97

	sub esp,0
	mov eax,.S3
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L97:

	sub esp,8
	call pop
	add esp,8

	sub esp,4
	mov eax,2
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	mov eax,[SYM_ADDRESS]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_unary_expression
	add esp,8
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	jmp .L96

.L95:


	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_SIZEOF]
	sete al
	movzx eax,al
	test eax,eax
	jz .L98


	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L100

	sub esp,0
	mov eax,.S4
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L100:

	mov eax,[TARGET_SIZEOF_INT]
	mov [ebp-8],eax

	sub esp,8
	call pop
	add esp,8

	sub esp,4
	mov eax,40
	push eax
	call expect
	add esp,8

	sub esp,8
	call pop
	add esp,8
	cmp eax,[SYM_CHAR]
	sete al
	movzx eax,al
	test eax,eax
	jz .L101

	mov eax,1
	mov [ebp-8],eax

.L101:

.L102:

	sub esp,8
	call peek
	add esp,8
	cmp eax,42
	sete al
	movzx eax,al
	test eax,eax
	jz .L103


	mov eax,[TARGET_SIZEOF_INT]
	mov [ebp-8],eax

	sub esp,8
	call pop
	add esp,8

	jmp .L102

.L103:

	sub esp,4
	mov eax,41
	push eax
	call expect
	add esp,8

	sub esp,4
	mov eax,2
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	mov eax,[SYM_CONSTANT]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	jmp .L99

.L98:

	sub esp,8
	call parse_postfix_expression
	add esp,8
	mov [ebp-4],eax

.L99:

.L96:

.L93:

.L90:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 117
	db 110
	db 97
	db 114
	db 121
	db 45
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
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 110
	db 101
	db 103
	db 97
	db 116
	db 105
	db 111
	db 110
	db 10
	db 0

.S2:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 110
	db 101
	db 103
	db 97
	db 116
	db 105
	db 111
	db 110
	db 10
	db 0

.S3:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 110
	db 101
	db 103
	db 97
	db 116
	db 105
	db 111
	db 110
	db 10
	db 0

.S4:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 115
	db 105
	db 122
	db 101
	db 111
	db 102
	db 10
	db 0


global parse_postfix_expression:function (parse_postfix_expression.end-parse_postfix_expression)
parse_postfix_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L104

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L104:

	sub esp,8
	call parse_primary_expression
	add esp,8
	mov [ebp-4],eax

	sub esp,8
	call peek
	add esp,8
	cmp eax,40
	sete al
	movzx eax,al
	test eax,eax
	jz .L105


	sub esp,8
	call pop
	add esp,8

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L106

	sub esp,12
	sub esp,12
	call peek
	add esp,12
	push eax
	mov eax,.S1
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,24

.L106:

	sub esp,4
	mov eax,3
	push eax
	call newnode
	add esp,8
	mov [ebp-8],eax

	mov eax,[SYM_FUNC_CALL]
	push eax
	mov eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-4]
	push eax
	mov eax,4
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_expression_list
	add esp,8
	push eax
	mov eax,8
	add eax,[ebp-8]
	pop ecx
	mov [eax],ecx

	mov eax,[ebp-8]
	mov [ebp-4],eax

	sub esp,4
	mov eax,41
	push eax
	call expect
	add esp,8

.L105:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
	db 101
	db 32
	db 112
	db 111
	db 115
	db 116
	db 102
	db 105
	db 120
	db 45
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
	db 112
	db 97
	db 114
	db 115
	db 101
	db 100
	db 32
	db 37
	db 99
	db 10
	db 0


global parse_expression_list:function (parse_expression_list.end-parse_expression_list)
parse_expression_list:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L107

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L107:

	sub esp,8
	call peek
	add esp,8
	cmp eax,41
	sete al
	movzx eax,al
	test eax,eax
	jz .L108

	mov eax,0
	add esp,16
	pop ebp
	ret

	jmp .L109

.L108:


	sub esp,4
	mov eax,3
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	mov eax,[SYM_EXPR_LIST]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call parse_expression
	add esp,8
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,8
	call peek
	add esp,8
	cmp eax,44
	sete al
	movzx eax,al
	test eax,eax
	jz .L110


	sub esp,8
	call pop
	add esp,8

	sub esp,8
	call parse_expression_list
	add esp,8
	push eax
	mov eax,8
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	jmp .L111

.L110:

	mov eax,0
	push eax
	mov eax,8
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

.L111:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret

.L109:
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
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
	db 45
	db 108
	db 105
	db 115
	db 116
	db 10
	db 0


global parse_primary_expression:function (parse_primary_expression.end-parse_primary_expression)
parse_primary_expression:
	push ebp
	mov ebp,esp
	sub esp,16

	mov eax,[PARSER_DEBUG]
	test eax,eax
	jz .L112

	sub esp,0
	mov eax,.S0
	push eax
	mov eax,[stderr]
	push eax
	call fprintf
	add esp,8

.L112:

	sub esp,8
	call peek
	add esp,8
	cmp eax,40
	sete al
	movzx eax,al
	test eax,eax
	jz .L113


	sub esp,8
	call pop
	add esp,8

	sub esp,8
	call parse_expression
	add esp,8
	mov [ebp-4],eax

	sub esp,4
	mov eax,41
	push eax
	call expect
	add esp,8

	jmp .L114

.L113:


	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_ID]
	sete al
	movzx eax,al
	test eax,eax
	jz .L115


	sub esp,4
	mov eax,2
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,4
	mov eax,[lexed_string]
	push eax
	call i_strdup
	add esp,8
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	jmp .L116

.L115:


	sub esp,8
	call peek
	add esp,8
	cmp eax,[SYM_STRING]
	sete al
	movzx eax,al
	test eax,eax
	jz .L117


	sub esp,4
	mov eax,2
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	sub esp,8
	call pop
	add esp,8
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	sub esp,4
	mov eax,[lexed_string]
	push eax
	call i_strdup
	add esp,8
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	jmp .L118

.L117:


	sub esp,4
	mov eax,[SYM_CONSTANT]
	push eax
	call expect
	add esp,8

	sub esp,4
	mov eax,2
	push eax
	call newnode
	add esp,8
	mov [ebp-4],eax

	mov eax,[SYM_CONSTANT]
	push eax
	mov eax,[ebp-4]
	pop ecx
	mov [eax],ecx

	mov eax,[lexed_number]
	push eax
	mov eax,4
	add eax,[ebp-4]
	pop ecx
	mov [eax],ecx

.L118:

.L116:

.L114:

	mov eax,[ebp-4]
	add esp,16
	pop ebp
	ret
.end:

.S0:
	db 112
	db 97
	db 114
	db 115
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
	db 45
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

section .data
	common TARGET_SIZEOF_INT 4:4

global PEEK_DEBUG
PEEK_DEBUG:
	dd 1

global PARSER_DEBUG
PARSER_DEBUG:
	dd 1
	common tk 4:4
	extern i_strdup
	extern fputs
	extern puts
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
	extern find_lvalue_type
	extern find_type
	extern declarator_type
	extern declarator_name
	common TYPE_PTRPTR 4:4
	common TYPE_CHARPTR 4:4
	common TYPE_INTPTR 4:4
	common TYPE_CHAR 4:4
	common TYPE_INT 4:4
	extern newnode
	common SYM_DECLARATOR 4:4
	common SYM_LOCAL_DECLARATION 4:4
	common SYM_DECL_LIST 4:4
	common SYM_EXPR_LIST 4:4
	common SYM_FUNC_CALL 4:4
	common SYM_DECLARATION 4:4
	common SYM_STATEMENT_LIST 4:4
	common SYM_SIZEOF 4:4
	common SYM_ADDRESS 4:4
	common SYM_POINTER 4:4
	common SYM_LAND 4:4
	common SYM_LOR 4:4
	common SYM_EQ 4:4
	common SYM_NE 4:4
	common SYM_GE 4:4
	common SYM_LE 4:4
	common SYM_NEGATE 4:4
	common SYM_COMPOUND_STATEMENT 4:4
	common SYM_TRANSLATION_UNIT 4:4
	common SYM_GLOBAL_DECL 4:4
	common SYM_GLOBAL_DEF 4:4
	common SYM_FUNC_DECL 4:4
	common SYM_FUNC_DEF 4:4
	common SYM_STRING 4:4
	common SYM_CONSTANT 4:4
	common SYM_ID 4:4
	common SYM_WHILE 4:4
	common SYM_FOR 4:4
	common SYM_ELSE 4:4
	common SYM_IF 4:4
	common SYM_RETURN 4:4
	common SYM_CHAR 4:4
	common SYM_INT 4:4
	common SYM_EOF 4:4
	extern lex
	common lexed_number 4:4
	common lexed_string 4:4
	extern debug_warning
	extern error
