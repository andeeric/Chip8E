/* file chip8.h */

#ifndef CHIP8_H
#define CHIP8_H

// CPU Specs
#define MEMORY_SIZE 4096
#define NUM_OF_REGISTERS 16
#define NUM_OF_PIXEL_COLS 64
#define NUM_OF_PIXEL_ROWS 32
#define NUM_OF_PIXELS (NUM_OF_PIXEL_COLS * NUM_OF_PIXEL_ROWS)
#define STACK_SIZE 16
#define KEYPAD_SIZE 16
#define FONTSET_SIZE 80

// Memory map
#define MEMORY_START 0
#define MEMORY_FONTSET 0x050
#define MEMORY_PROGRAM 0x200

// SDL DELAY
#define DELAY_MS 16

void initialize();
int loadGame(char *file);
void emulateCycle();
unsigned char * getDrawFlag();
unsigned char * getGfx();
void setKey(unsigned char k, unsigned char s);
void delay(int milliSecs);
void terminate();

// CPU instructions
void instr0NNN();
void instr00E0();
void instr00EE();
void instr1NNN();
void instr2NNN();
void instr3XNN();
void instr4XNN();
void instr5XY0();
void instr6XNN();
void instr7XNN();
void instr8XY0();
void instr8XY1();
void instr8XY2();
void instr8XY3();
void instr8XY4();
void instr8XY5();
void instr8XY6();
void instr8XY7();
void instr8XYE();
void instr9XY0();
void instrANNN();
void instrBNNN();
void instrCXNN();
void instrDXYN();
void instrEX9E();
void instrEXA1();
void instrFX07();
void instrFX0A();
void instrFX15();
void instrFX18();
void instrFX1E();
void instrFX29();
void instrFX33();
void instrFX55();
void instrFX65();

#endif /* CHIP8_H */
