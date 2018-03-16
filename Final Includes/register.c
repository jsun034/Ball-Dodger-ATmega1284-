#include <avr/io.h>
#include <util/delay.h>

#define DS_PORT    PORTC
#define DS_PIN     0
#define ST_CP_PORT PORTC
#define ST_CP_PIN  1
#define SH_CP_PORT PORTC
#define SH_CP_PIN  2

#define DS_low()  DS_PORT&=~_BV(DS_PIN)
#define DS_high() DS_PORT|=_BV(DS_PIN)
#define ST_CP_low()  ST_CP_PORT&=~_BV(ST_CP_PIN)
#define ST_CP_high() ST_CP_PORT|=_BV(ST_CP_PIN)
#define SH_CP_low()  SH_CP_PORT&=~_BV(SH_CP_PIN)
#define SH_CP_high() SH_CP_PORT|=_BV(SH_CP_PIN)

//Define functions
//===============================================

void output_led_state(unsigned int __led_state);
//===============================================
/*
int main (void)
{
	DDRC  = 0b00000111; //1 = output, 0 = input
	PORTC = 0b00000000;
	
	while(1)
	{
		// Output a knight rider pattern
		
		for (unsigned char i=15;i>=0;i--)
		{
			output_led_state(_BV(i));
			_delay_ms(250);
		}
		for (unsigned char i=1;i<15;i++)
		{
			output_led_state(_BV(i));
			_delay_ms(250);
		}
	}
}
*/




void output_led_state(unsigned int __led_state)
{
	SH_CP_low();
	ST_CP_low();
	//_delay_ms(10);
	for (int i=0;i<16;i++)
	{
		if ((_BV(i) & __led_state) == _BV(i))  //bit_is_set doesn’t work on unsigned int so we do this instead
		DS_high();
		else
		DS_low();
		SH_CP_high();
		SH_CP_low();
		
	}
	ST_CP_high();
	//_delay_ms(10);
}