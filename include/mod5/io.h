#define IO_LED *((char *)0xffffff00)
#define IO_SWITCH *((char *)0xffffff04)
#define IO_KEY *((char *)0xffffff08)

#define IO_LED8 *((char *)0xffffff13)
#define IO_LED9 *((char *)0xffffff17)
#define IO_HEX0 *((char *)0xffffff10)
#define IO_HEX1 *((char *)0xffffff14)
#define IO_HEX2 *((char *)0xffffff18)
#define IO_HEX3 *((char *)0xffffff1C)
void wait(int time);