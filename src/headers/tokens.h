#define TOK_EOF 		-1

#define TOK_NUMBER 		128
#define TOK_ID 			129
#define TOK_STRING		130

#define TOK_INT 		144
#define TOK_CHAR		145
#define TOK_SHORT		146
#define TOK_LONG		147
#define TOK_STRUCT		148
#define TOK_VOID		149

#define TOK_RETURN 		160
#define TOK_IF			161
#define TOK_ELSE		162
#define TOK_WHILE		163
#define TOK_DO			164
#define TOK_FOR			165
#define TOK_BREAK		166
#define TOK_CONTINUE	167
#define TOK_SWITCH		168
#define TOK_CASE		169
#define TOK_DEFAULT		170

#define TOK_SIZEOF		180

#define TOK_LAND		192
#define TOK_LOR			193
#define TOK_EQ			194
#define TOK_NE			195
#define TOK_LE			196
#define TOK_GE			197
#define TOK_ARROW		198

char *token_to_string(int token);