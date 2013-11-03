#ifndef XMODEM_H
#define XMODEM_H


struct xmodem_receiver {
	int (*get_char)(void); 
	void (*put_char)(char c);
	int (*char_avail)();
	void (*delay_1s)(); 
	int (*writer)(unsigned char* buffer, int size);
};

struct xmodem_transmitter {
	int (*getchar)(void); 
	void (*putchar)(char c);
	int (*char_avail)();
	void (*delay_1s)(); 
	int (*reader)(char* buffer, int size);
};

int xmodem_receive(struct xmodem_receiver *rx);
int xmodem_transmit(struct xmodem_transmitter *tx);

#endif
