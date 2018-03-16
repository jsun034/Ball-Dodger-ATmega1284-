#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "io.c"
//#include "timer.h"

void ADC_init() {
	ADMUX=(1<<REFS0);                        
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); 
}

uint16_t ReadADC(uint8_t ch)
{
	ADC_init();
	//Select ADC Channel ch 0-7
	ch=ch&0b00000111;
	ADMUX|=ch;
	//Start conversion
	ADCSRA|=(1<<ADSC);
	//Waiting for conversion 
	while(!(ADCSRA & (1<<ADIF)));
	ADCSRA|=(1<<ADIF);
	return(ADC);
}

// returns 1 if left is true, else 0
int checkLeft() {
	if(!ReadADC(2)) {
		return 1;
	}
	return 0;
}

int checkRight() {
	if(ReadADC(2) > 25) {
		return 1;
	}
	return 0;	
}

int checkUp() {
	if(ReadADC(3) > 25) {
		return 1;
	}
	return 0;	
}

int checkDown() {
	if(!ReadADC(3)) {
		return 1;
	}
	return 0;	
}
/*

int main(void) {

   DDRA = 0x3F; PORTA = 0xC0;  // 00000011  11111100
   DDRD = 0xFF; PORTD = 0x00; // LCD control lines
   

   char adc_value[7];
   //ADC_init();				 
   
  
   LCD_init();
   
   TimerSet(250);
   TimerOn();
   
   while (1) {
		
	
		LCD_Cursor(1);
		if(checkLeft()==1) {
			LCD_WriteData('T');
		} else {
			LCD_WriteData('F');
		}
		
		
		LCD_Cursor(3);
		if(checkRight()==1) {
			LCD_WriteData('T');
		} else {
			LCD_WriteData('F');
		}
		
		
		
		
		LCD_Cursor(5);
		if(checkUp()==1) {
			LCD_WriteData('T');
		} else {
			LCD_WriteData('F');
		}		
		
		LCD_Cursor(7);
		if(checkDown() == 1) {
			LCD_WriteData('T');
		} else {
			LCD_WriteData('F');
		}
				while(!TimerFlag) {}
				TimerFlag = 0;
		
		//LCD_DisplayString(1, "Tr");
		//LCD_DisplayString(5, "ut");
		//LCD_DisplayString(6, "h");
		//LCD_DisplayString(6, " or dare");
        

   }
}
*/
