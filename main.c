#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "io.c"
#include "timer.h"
#include "joystick.c"
#include "register.c"
#include <avr/eeprom.h>


unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}

typedef struct _task {
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

//Global Variables
int LED_Player = 0b0100000011100111; //16-bit (character is 2 dots long)
int pcount = 0;
uint16_t HighScoreData = -1;
int buttonCount = 0;

enum Player_State { Init, Left, Right, Up, Down };

int Player_State_Tick(int state) {
	// transitions
	switch (state) {
		case Init:
			++pcount;
			// polling joystick
			if(checkUp() && (pcount >= 8) && ((LED_Player & 0x00FF) > 0x003F)) {
				state = Up;
			}
			else if(checkDown() && (pcount >= 8) && ((LED_Player & 0x00FF) < 0x00FC)) {
				state = Down;
			}
			if(checkLeft() && (pcount >= 8) && (((LED_Player & 0xFF00) << 1) <= 0x4000)) {
					state = Left;
				}
			else if(checkRight() && (pcount >= 8) && (((LED_Player & 0xFF00) >> 1) >= 0x0100)) {
					state = Right;
				}
			break;
		case Left:
			state = Init;
			pcount = 0;
			break;
		case Right:
			state = Init;
			pcount = 0;
			break;
		case Up:
			state = Init;
			pcount = 0;
			break;
		case Down:
			state = Init;
			pcount = 0;
			break;			
		default:
			state = Init;
			pcount = 0;
			break;
	}
	// actions
	switch(state) {
		case Init:
			output_led_state(LED_Player);
			break;
		case Left:
			// if moving left is possible
			//01000000
			if(((LED_Player & 0xFF00) << 1) <= 0x4000) {
				LED_Player = (LED_Player&0x00FF)|((LED_Player&0xFF00) << 1);
			}
			break;
		case Right:
			// if moving right is possible
			//00000001
			if(((LED_Player & 0xFF00) >> 1) >= 0x0100) {
				LED_Player = (LED_Player&0x00FF)|((LED_Player&0xFF00) >> 1);
			}
			break;
		case Up: 
			//if moving up is possible (not using for project)
			if((LED_Player & 0x00FF) > 0x003F) { 
				int lower = LED_Player & 0x00FF; 
				lower = lower<<1; 
				lower +=1; 
				lower &= 0x00FF; 
				int upper = (LED_Player&0xFF00);
				LED_Player = upper|lower;
			}
			break;
		case Down:
			// if moving down is possible (not using for project)
			//1111001
			if((LED_Player & 0x00FF) < 0x00FC) { 
				int lower = LED_Player & 0x00FF;
				lower = lower >> 1;
				lower |= 0x0080;
				int upper = (LED_Player&0xFF00);
				LED_Player = upper | lower;
			}
			break;
		default:
			break;
	}
	return state;
}


// ------------Variables for Ball_State-----------------

int LED_Ball = 0x0000; //16-bit ball
// ----ball pos variables----
int row = 0;
int col = 0;
int tempRow = 0;
int tempCol = 0;
// ----ball pos variables end----
int bcount = 1; // iterates and resets for speed
int speed = 15; // smaller speed value is faster
uint16_t score = -1;
// ------------Variables for Ball_State-----------------

// checks if ball intersects with player
int checkIntersect(int a, int b) {                                 
	int rowBool = 0;
	unsigned int count = 0;
	int n = a & b;
	while (n) {		// counts bits in n
		count += n & 1;
		n >>= 1;
	}
	// 7 bits in n represents intersection
	if(count == 7) {
		rowBool = 1;
	}
	return rowBool;
}

// handles LCD and User Input when player loses
void gameOver() {
	// Display final screen
	char scoreArr[4];
	itoa(score, scoreArr, 10);
	LCD_DisplayString(1, "Score:          GG...Try again?");
	LCD_Cursor(8);
	LCD_WriteData(scoreArr[0]);
	if(score > 9) {
		LCD_WriteData(scoreArr[1]);
	}
	if(score > 99) {
		LCD_WriteData(scoreArr[2]);
	}
	if(score > 999) {
		LCD_WriteData(scoreArr[3]);
	}
	LCD_Cursor(33);
	
	// Wait on User Input
	while(PINC & 0x80 && PINC & 0x40) {}
	// reset game variables
	score = 0;
	LED_Player = 0b0100000011100111;
	LED_Ball = 0x0000;
	speed = 15;
}

enum Ball_State { Start, Speed1 };

// State for managing output and frequency of projectiles
int Ball_State_Tick(int state) {
	// transitions
	switch (state) {
		case Start:
			++score;
			
			// Update High Score and Speed if necessary 
			if(score > HighScoreData) {
				eeprom_update_word((uint16_t*)0, score);
				HighScoreData = score;
			}
			if(score >= 10 && score < 20) {
				speed = 10;
			}
			else if(score >= 20) {
				speed = 8;
			}
			
			// Display Current Score
			char scoreArr[4];
			itoa(score, scoreArr, 10);
			LCD_DisplayString(1, "Score:          High Score:  ");
			LCD_Cursor(8);
			LCD_WriteData(scoreArr[0]);
			if(score > 9) { LCD_WriteData(scoreArr[1]); }
			if(score > 99) { LCD_WriteData(scoreArr[2]); }
			if(score > 999) { LCD_WriteData(scoreArr[3]); }
			
			// Display High Score
			char HighScoreArr[4];
			itoa(HighScoreData, HighScoreArr, 10);
			LCD_Cursor(29);
			LCD_WriteData(HighScoreArr[0]);
			if(HighScoreData > 9) {
				LCD_WriteData(HighScoreArr[1]);
				} else {
				LCD_WriteData(' ');
			}
			if(HighScoreData > 99) {
				LCD_WriteData(HighScoreArr[2]);
				} else {
				LCD_WriteData(' ');
			}
			if(HighScoreData > 999) {
				LCD_WriteData(HighScoreArr[3]);
				} else {
				LCD_WriteData(' ');
			}
			
			// Randomly generate and update projectile(ball) position
			row = (rand()%8)+1;
			tempRow = 0b0000000011111110;
			for(int i = 0; i < 8-row; ++i) {
				tempRow = tempRow<<1;
				tempRow += 1;
			}
			tempRow &= 0x00FF;
			col = 9;
			
			state = Speed1;
			break;
		case Speed1:
			if(checkIntersect(LED_Ball, LED_Player)) {
				gameOver();
			}
			if(bcount >= speed) {
				bcount = 1;
				--col;
				tempCol = 1<<(8+(8-col));
				LED_Ball = tempCol | tempRow;
				if(col == 0) {
					state = Start;
				} 
			
			}
			output_led_state(LED_Ball);
			++bcount;
			break;
		default:
			state = Start;
			break;
		}
		return state;
	}

void Menu() {
	LCD_DisplayString(1, "Press to Play!                  ");
	while(PINC & 0x80 && PINC & 0x40) {}	
}

void resetHighScore() {
	eeprom_update_word((uint16_t*)0, 0);
	HighScoreData = 0;	
}


int main(void)
{
	DDRA = 0x3F; PORTA = 0xC0;  // LCD Screen (output) and Joystick (input)
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	DDRC  = 0x07; PORTC = 0xF8;	// Shift Registers
	HighScoreData = eeprom_read_word((uint16_t*)0);
	
	LCD_init();
	
	// Period for the tasks
	unsigned long int Player_State_calc = 5;
	unsigned long int Ball_State_calc = 10;
	
	//Calculating GCD
	unsigned long int GCD = 5;

	//Recalculate GCD periods for scheduler
	unsigned long int Player_State_period = Player_State_calc/GCD;
	unsigned long int Ball_State_period = Ball_State_calc/GCD;
	
	//Declare an array of tasks
	static task task1, task2;
	task *tasks[] = { &task1, &task2 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = Player_State_period;//Task Period.
	task1.elapsedTime = Player_State_period;//Task current elapsed time.
	task1.TickFct = &Player_State_Tick;//Function pointer for the tick.

	// Task 2
	task2.state = -1;//Task initial state.
	task2.period = Ball_State_period;//Task Period.
	task2.elapsedTime = Ball_State_period;//Task current elapsed time.
	task2.TickFct = &Ball_State_Tick;//Function pointer for the tick.

	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
	// Simple Menu
	Menu();
	
	while (1) {
		// soft reset
		if(!(PINC & 0x40)) {
			resetHighScore();
			score = 0;
			LED_Player = 0b0100000011100111;
			LED_Ball = 0x0000;
			speed = 15;
		}
		for (unsigned short i = 0; i < numTasks; i++ ) {
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;

    }
	
	//if error
	return 0;
}


