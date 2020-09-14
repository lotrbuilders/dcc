#define EOF -1
#define NULL		(void *)0 

char *stdin;
char *stdout;
char *stderr;

char *fopen(char *file, char *mode);

int getc(char *stream);
int fgetc(char *stream);
int getchar();
int ungetc(int c,char *stream);

int printf(char *format);
int fprintf(char *stream, char *format);
int snprintf(char *buffer, int size, char *format);
int sprintf(char *buffer,char *format);

int puts(char *str);
int fputs(char *str, char *stream);
int fputc(char c, char *stream);

int fscanf(void *stream,char *format);