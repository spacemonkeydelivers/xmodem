#include <string.h>
#include <stdio.h>
#include "xmodem.h"
#include <stdint.h>

#include <avr/io.h>


#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define DLY_1S 100000
#define MAX_RETRY 25

//#define BUFF_LEN 1030
#define BUFF_LEN 133
//#define BUFF_LEN 20
#define HDR_LEN 3
#define CRC_LEN 2
#define FIRST_NUM 1
#define SECOND_NUM 2



unsigned short crc16(const unsigned char *buf, int sz)
{
	unsigned short crc = 0;
	while (--sz >= 0)
	{
		int i;
		crc ^= (unsigned short) *buf++ << 8;
		for (i = 0; i < 8; i++)
		{
			if (crc & 0x8000)
			{
				crc = crc << 1 ^ 0x1021;
			}
			else
			{
				crc <<= 1;
			}
        }
	}
	return crc;
}

int CheckPacket(unsigned char* buff, unsigned char num, struct xmodem_receiver *rx)
{
//	PORTA = num;
	if (num != buff[FIRST_NUM])
	{
		return 0;
	}
	if (buff[FIRST_NUM] + buff[SECOND_NUM] != 0xFF)
	{
		return 0;
	}
/*	unsigned short crc = crc16(buff, size);
	if (memcmp(buff+size, &crc, CRC_LEN))
	{
		return 0;
	}
*/	return 1;
}

int Start(struct xmodem_receiver *rx)
{
	unsigned char xmodem_buffer[BUFF_LEN];
	unsigned short cur_char = 0;
	unsigned char wrong_char = 0;
	unsigned char cur_packet = 1;

	while (1)
	{
		while (cur_char < BUFF_LEN)
		{
			xmodem_buffer[cur_char] = rx->get_char();
			if (cur_char == 0)
			{
				switch (xmodem_buffer[cur_char])
				{
					case SOH:
					{
						cur_char++;
						break;
					}
					case EOT:
					{
						rx->put_char(ACK);
						return 0; /*finished ok*/
					}
					default:
					{
						//PORTA = xmodem_buffer[0];
						wrong_char++;
						if (wrong_char == 5)
						{
							return -1; /*too many errors*/
						}
						rx->put_char(NAK);
						break;
					}
				}
			}
			else
			{
				cur_char++;
			}
		}
		if (CheckPacket(xmodem_buffer, cur_packet, rx))
		{
			cur_packet++;
			//rx->writer();

			cur_char = 0;
			rx->put_char(ACK);
		}
		else
		{
			cur_char = 0;
			rx->put_char(NAK);
		}
	}
}


int xmodem_receive(struct xmodem_receiver *rx)
{
	unsigned char retry_count = 0;

	while (retry_count < MAX_RETRY)
	{
		rx->put_char('C');
		if (rx->char_avail())
		{
			if (!Start(rx))
			{
				return 0; /*finished ok*/
			}
		}
		rx->delay_1s();
	}
}

