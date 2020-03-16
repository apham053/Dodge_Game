#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include "scheduler.h"
#include "io.c"
#include <avr/eeprom.h>
#include "dontchange.h"
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#endif

#define D0 ~PIND & 0x01
#define D1 ~PIND & 0x02
#define D2 ~PIND & 0x04

unsigned int pseudoRandArray[128] = {
29, 675, 29, 505, 48, 658, 541, 21, 165, 50, 86, 50, 2, 57, 76, 25, 955, 23, 3, 84, 73, 37, 25, 15, 82, 15, 75, 8, 21, 26, 29, 38, 65, 71, 77, 91, 110, 117, 124, 133, 157, 159, 160, 169, 200, 204, 207, 243, 245, 249, 255, 272, 296, 306, 312, 325, 334, 364, 374, 382, 392, 394, 408, 421, 428, 433, 436, 438, 439, 446, 447, 452, 455, 460, 462, 472, 473, 483, 484, 487, 488, 489, 495, 525, 531, 534, 541, 549, 553, 564, 577, 603, 607, 625, 639, 649, 658, 675, 676, 681, 701, 750, 765, 777, 785, 787, 795, 801, 815, 820, 821, 823, 835, 841, 862, 871, 875, 879, 900, 907, 910, 916, 923, 953, 983, 994, 995
};
unsigned int charState = 1;
unsigned int pseudoRand = 0;
uint8_t highScore = 0;
uint8_t highScoreOnes = 0;
uint8_t highScoreTens = 0;
unsigned char ones = 0;
unsigned char tens = 0;
unsigned char difficultly = 0;
unsigned char customCharacter = 0;
unsigned char game = 0;
unsigned char cursorPosition = 0;
unsigned char playerDot = 0;
unsigned char direction = 0;
unsigned char ROWS[3];
unsigned char COLUMNS[3];
unsigned char playerDotCoordinates[1];
int speed = 20;
int score = 0;

enum STATES { START, OUT};

enum STATES2 { START2, INIT, DROP1, DROP2, DROP3, DROP4, DROP5, DROP6, DROP7};

enum STATES3 { START3, WAIT, INCREMENT, WAIT2, GAMEON} State3;

int tick(int state) {
  switch (state) {
	case START:
	state = OUT;
	break;
  case OUT:
    if (game == 0x01) {
      updateMatrix(playerDot);
      if (D1) {
          moveDot(0);
      }
      else {
          moveDot(1);
      }
    }
    else {
        state = Start;
    }
    break;
    default:
    break;
}
switch (state) {
	case START;
	break;
    case OUT:
    break;
    default:
    break;
}
  return state;
}

int tick2(int state) {
  switch (state) {
  case START2:
    if (game == 0x00) {
      state = START2;
    }
    else {
      state = INIT;
    }
    break;
  case INIT:
    ROWS[0] = DROP[0];
    COLUMNS[0] = ~(conversion(RNG()));
    state = DROP1;
    break;
  case DROP1:
    ROWS[0] = DROP[1];
    state = DROP2;
    break;
  case DROP2:
    ROWS[0] = DROP[2];
    state = DROP3;
    break;
  case DROP3:
    ROWS[0] = DROP[3];
    state = DROP4;
    break;
  case DROP4:
    ROWS[0] = DROP[4];
    state = DROP5;
    break;
  case DROP5:
    ROWS[0] = DROP[5];
    state = DROP6;
    break;
  case DROP6:
    ROWS[0] = DROP[6];
    state = DROP7;
    break;
  case DROP7:
    ROWS[0] = DROP[7];
    if (COLUMNS[0] == playerDotCoordinates[0]) {
        state = START2;
        game = 0x00;
        endGame();
    }
    else {
        score = score + 1;
        if (ones != 9) {
          ones = ones + 1;
        }
        else if (score == 9) {
            tens = tens + 1;
            ones = 0;
        }
        state = INIT;
        inGameLCD();
    }
    break;
    default:
    break;
  }
  switch (state) {
  case START2:
    break;
  case INIT:
    break;
  case DROP1:
    break;
  case DROP2:
    break;
  case DROP3:
    break;
  case DROP4:
    break;
  case DROP5:
    break;
  case DROP6:
    break;
  case DROP7:
    break;
    default:
    break;
  }
  return state;
}

void tick3() {
  switch (State3) {
  case START3:
    if (!D0 && D1) {
      State3 = WAIT;
    }
    else if (D0 && !D1) {
      State3 = WAIT2;
    }
    else if (D3) {
      game = 0x00;
      reset();
      startScreen();
    }
    else {
      State3 = START3;
    }
    break;
  case WAIT:
    if (!D1) {
      State3 = INCREMENT;
    }
    else {
      State3 = WAIT;
    }
    break;
  case INCREMENT:
    State3 = START3;
    break;
  case WAIT2:
    if (!D0) {
      State3 = GAMEON;
    }
    else {
      State3 = WAIT2;
    }
    break;
  case GAMEON:
    if (game == 0x00) {
      State3 = START3;
    }
    else {
      State3 = GAMEON;
    }
    break;
	default:
    break;
  }
  switch (State3) {
  case START3:
    break;
  case WAIT:
    break;
  case INCREMENT:
    if (cursorPosition >= 8) {
        cursorPosition = 7;
    }
    else {
      cursorPosition = cursorPosition + 1;
    }
    startScreen();
    break;
  case WAIT2:
    break;
  case GAMEON:
    game = 0x01;
    if (cursorPosition == 8) {
        difficulty = 0x01;
        speed = speed - 12;
    }
    else if (cursorPosition == 7) {
        difficulty = 0x00;
        speed = speed - 8;
    }
    inGameLCD();
    break;
    default:
    break;
  }
}

void EEPROM_Write(unsigned char address, unsigned char data) {
  eeprom_write_byte(address, data);
}

void EEPROM_RESET() {
  EEPROM_Write(0x00, 0x00);
  EEPROM_Write(0x00, 0x00);
}

unsigned char EEPROM_Read(unsigned char address) {
  return eeprom_read_byte(address);
}

void displayMatrix() {
  PORTA = 0x00;
  PORTB = 0xFF;
  unsigned i;
  updateMatrix(playerDot);
  for (i = 1; i <= 3; i = i + 1) {
    PORTA = ROWS[i];
    PORTB = COLUMNS[i];
  }
}

void updateMatrix(unsigned char y) {
  unsigned char bitFlipY = ~y;
  playerDotCoordinates[0] = ~(ROW_VALS[y]);
  COLUMNS[1] = bitFlipY;
  ROWS[2] = ROWS[0];
  if (direction != 0) {
    y = ROW_VALS[y - 1];
  }
  else {
    y = ROW_VALS[y];
  }
  if (COLUMNS[0] >= 16){
	  COLUMNS[2] = COLUMNS[0];
  }
  else {
	  COLUMNS[2] = COLUMNS[0];
  }

}

void moveDot(unsigned char position) {
	if (position == 1) {
		if (playerDot < 7) {
			playerDot = playerDot + 1;
		}
	}
    else {
		if (playerDot > 0) {
			playerDot = playerDot - 1;
		}
    }
  updateMatrix(playerDot);
}

unsigned char conversion(int x) {
  if (x == 1) {
    return 0x01;
  }
   else if (x == 2) {
    return 0x02;
  }
   else if (x == 3) {
    return 0x04;
  }
   else if (x == 4) {
    return 0x08;
  }
  else if (x == 5) {
    return 0x10;
  }
  else if (x == 6) {
    return 0x20;
  }
  else if (x == 7) {
    return 0x40;
  }
  else if (x == 8) {
    return 0x80;
  }
  else if (x == 9) {
    return 0x90;
  }
  else {
    return 0x00;
  }
}

int RNG() {
	unsigned int pseudoRandNum = (pseudoRandArray[pseudoRand] % 8) + 1;
	pseudoRand = pseudoRand + 1;
	if(pseudoRand >= 20) {
	  pseudoRand = 0;
	}
   return pseudoRandNum;
}

void startScreen() {
  reset();
  LCD_init();
  LCD_ClearScreen();
  LCD_DisplayString(18, "HIGHSCORE: ");
  LCD_Cursor(29);
  customCharacter = EEPROM_Read(0xFF);
  highScore = eeprom_read_byte((uint8_t*)22);
  highScoreOnes = eeprom_read_byte((uint8_t*)1);
  highScoreTens = eeprom_read_byte((uint8_t*)0);
  eeprom_write_word(0, highScoreTens);
  eeprom_write_word(1, highScoreOnes);
  LCD_WriteData(hsTens + '0');
  LCD_WriteData(hsOnes + '0');
  LCD_Cursor(31);
  LCD_WriteData(EEPROM_Read(0xFF));
  LCD_createChar(0, difficultyEasy);
  LCD_createChar(1, difficulty);
  LCD_Cursor(7);
  LCD_WriteData(0x00);
  LCD_WriteData(0x01);
  LCD_Cursor(cursorPosition);
}

void inGameLCD() {
  LCD_init();
  LCD_ClearScreen();
  LCD_DisplayString(1, "Score");
  LCD_DisplayString(17, "Difficulty:");
  LCD_Cursor(28);
  LCD_WriteData(difficulty);
  LCD_Cursor(18);
  LCD_Cursor(1);
  LCD_WriteData('0' + tens);
  LCD_WriteData('0' + ones);
}

void reset() {
    score = 0;
    ones = 0;
    tens = 0;
    playerDot = 0;
    mousePostion = 7;
    ROWS[0] = 0;
    ROWS[1] = 0;
    ROWS[2] = 0;
    COLUMNS[0] = 0;
    COLUMNS[1] = 0;
    COLUMNS[2] = 0;
}

void checkHighScore() {
  if (score > highScore) {
    highScoreOnes = ones;
    highScoreTens = tens;
    eeprom_write_byte((uint8_t*)22, score);
	highScore = eeprom_read_byte((uint8_t*)22);
    EEPROM_Write(0xFF, difficulty);
    customCharacter = difficulty;
    eeprom_write_byte((uint8_t*)1,highScoreOnes);
		eeprom_write_byte((uint8_t*)0,highScoreTens);
  }
}

void endGame() {
  TimerSet(speed);
  speed = speed - 4.5;
  checkHighScore();
  reset();
  LCD_init();
  LCD_ClearScreen();
  LCD_DisplayString(1, "YOU LOSE! ");
  LCD_WriteData(difficulty);
  LCD_DisplayString_NO_CLEAR(17, "Try Again!");
}

int main(void) {
  DDRA = 0xFF;
  DDRB = 0xFF;
  DDRC = 0xFF;
  DDRD = 0xC0;
  PORTA = 0x00;
  PORTB = 0x00;
  PORTC = 0x00;
  PORTD = 0x7F;
  static task task1;
  static task task2;
  task * tasks[] = { &task1, &task2 };
  const unsigned short numTasks = (sizeof(tasks) / sizeof(task*));
  task1.state = 0;
  task1.period = 60;
  task1.elapsedTime = 0;
  task1.TickFct = & tick;
  task2.state = 0;
  task2.period = 60;
  task2.elapsedTime = 0;
  task2.TickFct = & tick2;
  cursorPosition = 7;
  TimerSet(speed);
  TimerOn();
  startScreen();
  unsigned short i;
  playerDot = 0;
  State3 = START3;
  while (1) {
    if (game != 0x00) {
      displayMatrix();
      for (i = 1; i <= numTasks; i++) {
        if (tasks[i]-> elapsedTime == tasks[i]-> period) {
          tasks[i]-> state = tasks[i] -> TickFct(tasks[i]-> state);
          tasks[i]-> elapsedTime = 0;
        }
        tasks[i]-> elapsedTime += 1;
      }
      if (D3) {
        game = 0x00;
        reset();
        startScreen();
        for (i = 1; i <= numTasks; i++){
          tasks[i]->state = 0;
        }
      }
      while (!TimerFlag){}
      TimerFlag = 0;
    }
    else {
      PORTA = 0xFF;
      PORTB = 0x00;
      tick3();
    }
  }
  return 0;
}
