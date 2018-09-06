/* file chip8.c */

#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

// Two byte opcode
unsigned short opcode;
void (*instruction)();

// 4K memory
unsigned char memory[MEMORY_SIZE];

// Registers
unsigned char V[NUM_OF_REGISTERS];  // V0, V1, ..., V16
unsigned short I;	                // Index register
unsigned short pc;
	                // Program counter
// Graphics
unsigned char gfx[NUM_OF_PIXELS];
unsigned char drawFlag;

// Fontset
unsigned char chip8Fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// 60Hz timers
unsigned char delayTimer;
unsigned char soundTimer;

// Stack
unsigned short stack[STACK_SIZE];
unsigned short sp;	                // Stack pointer

// HEX keypad
unsigned char key[KEYPAD_SIZE] =
{
	0, 0, 0, 0,	// 1 2 3 4
	0, 0, 0, 0,	// Q W E R
	0, 0, 0, 0,	// A S D F
	0, 0, 0, 0,	// Z X C V
};

void initialize() {
	pc 		= 0x200;	// Program counter starts at 0x200
	opcode 	= 0;		// Reset current opcode
	I 		= 0;		// Reset index register
	sp 		= 0;		// Reset stack pointer

	for(int i = 0; i < MEMORY_SIZE; ++i)    // Clear memory
		memory[i] = 0;
    for(int i = 0; i < NUM_OF_REGISTERS; ++i)   // Clear registers
		V[i] = 0;
    for(int i = 0; i < STACK_SIZE; ++i)     // Clear stack
		stack[i] = 0;
	for(int i = 0; i < NUM_OF_PIXELS; ++i)  // Clear display
		gfx[i] = 0;

	drawFlag = 0;

	for(int i = 0; i < FONTSET_SIZE; i++)	// Load fontset
		memory[i + MEMORY_FONTSET] = chip8Fontset[i];

	delayTimer = 0;	// Reset timers
	soundTimer = 0;
}

int loadGame(char *file) {
	FILE *fptr = fopen(file, "rb");
	if(!fptr) {
		fprintf(stderr, "Error: Unable to open game file\n");
		fclose(fptr);
		return -1;
	}

	fseek(fptr, 0, SEEK_END);
	int size = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);

	if(size > MEMORY_SIZE - MEMORY_PROGRAM) {
        fprintf(stderr, "Error: Game file larger than Chip8 program memory\n");
        fclose(fptr);
        return -1;
	}

	unsigned char *buffer = (unsigned char*) malloc(sizeof(unsigned char) * size);
	if(buffer == NULL) {
        fprintf(stderr, "Error: Unable to allocate buffer memory\n");
        fclose(fptr);
        free(buffer);
        return -1;
	}

	int n = fread(buffer, 1, size, fptr);
	if(!n || n != size) {
		fprintf(stderr, "Error: Unable to read game file\n");
		fclose(fptr);
        free(buffer);
		return -1;
	}

	for(int i = 0; i < size; i++)
		memory[MEMORY_PROGRAM + i] = buffer[i];

	fclose(fptr);
	free(buffer);

	return 0;
}

void emulateCycle() {
	// Fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Decode and execute opcode
	switch(opcode & 0xF000) {
		case 0x0000:
			switch(opcode & 0x000F) {
				case 0x0000:	// 0x00E0
					instruction = &instr00E0;
                    break;
				case 0x000E:	// 0x00EE
					instruction = &instr00EE;
                    break;
				default:
					printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
			}
            break;
		case 0x1000:
			instruction = &instr1NNN;
			break;
		case 0x2000:
			instruction = &instr2NNN;
			break;
		case 0x3000:
			instruction = &instr3XNN;
			break;
		case 0x4000:
			instruction = &instr4XNN;
			break;
		case 0x5000:
			instruction = &instr5XY0;
			break;
		case 0x6000:
			instruction = &instr6XNN;
			break;
		case 0x7000:
			instruction = &instr7XNN;
			break;
		case 0x8000:
			switch(opcode & 0x000F) {
				case 0x0000:
					instruction = &instr8XY0;
					break;
				case 0x0001:
					instruction = &instr8XY1;
					break;
				case 0x0002:
					instruction = &instr8XY2;
					break;
				case 0x0003:
					instruction = &instr8XY3;
					break;
				case 0x0004:
					instruction = &instr8XY4;
					break;
				case 0x0005:
					instruction = &instr8XY5;
					break;
				case 0x0006:
					instruction = &instr8XY6;
					break;
				case 0x0007:
					instruction = &instr8XY7;
					break;
				case 0x000E:
					instruction = &instr8XYE;
					break;
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
			}
			break;
		case 0x9000:
			instruction = &instr9XY0;
			break;
		case 0xA000:
			instruction = &instrANNN;
			break;
		case 0xB000:
			instruction = &instrBNNN;
			break;
		case 0xC000:
			instruction = &instrCXNN;
			break;
		case 0xD000:
			instruction = &instrDXYN;
			break;
		case 0xE000:
			switch(opcode & 0x00FF) {
				case 0x009E:
					instruction = &instrEX9E;
					break;
				case 0x00A1:
					instruction = &instrEXA1;
					break;
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
			}
			break;
		case 0xF000:
			switch(opcode & 0x00FF) {
				case 0x0007:
					instruction = &instrFX07;
					break;
				case 0x000A:
					instruction = &instrFX0A;
					break;
				case 0x0015:
					instruction = &instrFX15;
					break;
				case 0x0018:
					instruction = &instrFX18;
					break;
				case 0x001E:
					instruction = &instrFX1E;
					break;
				case 0x0029:
					instruction = &instrFX29;
					break;
				case 0x0033:
					instruction = &instrFX33;
					break;
				case 0x0055:
					instruction = &instrFX55;
					break;
				case 0x0065:
					instruction = &instrFX65;
					break;
			}
			break;
		default:
			printf("Unknown opcode: 0x%X\n", opcode);
	}

	if(*instruction != NULL)
		instruction();

	// Update timers
	if(delayTimer > 0)
		delayTimer--;

	if(soundTimer > 0) {
		if(soundTimer == 1)
			printf("\a");
		soundTimer--;
	}
}

unsigned char * getDrawFlag() {
	return &drawFlag;
}

unsigned char * getGfx() {
    return gfx;
}

void setKey(unsigned char k, unsigned char s) {
    if(k > KEYPAD_SIZE - 1) {
        printf("Error: Key index overflow");
        return;
    }
    if(s != 1 && s != 0)
        return;

    key[k] = s;
}

/* The 35 CPU instructions */

// 0NNN Call: Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
void instr0NNN() {
	fprintf(stderr, "CPU instruction 0x0NNN (Call RCA 1802 program at address NNN) is not supported\n");
	exit(EXIT_FAILURE);
}

// 00E0 Display - disp_clear: Clears the screen
void instr00E0() {
	for(int i = 0; i < NUM_OF_PIXELS; i++)
		gfx[i] = 0;
	drawFlag = 1;
	pc += 2;
}

// 00EE Flow - return;: Returns from a subroutine
void instr00EE() {
	pc = stack[--sp];
	pc += 2;
}

// 1NNN Flow - Goto NNN;: Jumps to address NNN.
void instr1NNN() {
	pc = opcode & 0x0FFF;
}

// 2NNN Flow - *(0xNNN)(): Calls subroutine at NNN
void instr2NNN() {
	stack[sp++] = pc;
	pc = opcode & 0x0FFF;
}

// 3XNN Cond - if(Vx == NN): Skips the next instruction if Vx equals NN.
void instr3XNN() {
	if(V[(unsigned char) (opcode >> 8 & 0x0F)] == (unsigned char) opcode)
		pc += 4;
	else
		pc += 2;
}

// 4XNN Cond - if(Vx != NN): Skips the next instruction if Vx does not equal NN.
void instr4XNN() {
	if(V[(unsigned char) (opcode >> 8 & 0x0F)] != (unsigned char) opcode)
		pc += 4;
	else
		pc += 2;
}

// 5XY0 Cond - if(Vx == Vy): Skips the next instruction if Vx equals Vy.
void instr5XY0() {
	if(V[(unsigned char) (opcode >> 8 & 0x0F)] == V[(unsigned char) (opcode >> 4 & 0x0F)])
		pc += 4;
	else
		pc += 2;
}

// 6XNN Const - Vx = NN: Sets Vx to NN.
void instr6XNN() {
	V[(unsigned char) (opcode >> 8 & 0x0F)] = (unsigned char) opcode;
	pc += 2;
}

// 7XNN Const - Vx += NN: Adds NN to Vx. (Carry flag is not changed)
void instr7XNN() {
	V[(unsigned char) (opcode >> 8 & 0x0F)] += (unsigned char) opcode;
	pc += 2;
}

// 8XY0 Assign - Vx = Vy: Sets Vx to the value of Vy.
void instr8XY0() {
	V[(unsigned char) (opcode >> 8 & 0x0F)] = V[(unsigned char) (opcode >> 4 & 0x0F)];
	pc += 2;
}

// 8XY1 BitOp - Vx = Vx | Vy: Sets Vx to Vx OR Vy (Bitwise OR)
void instr8XY1() {
	V[(unsigned char) (opcode >> 8 & 0x0F)] |= V[(unsigned char) (opcode >> 4 & 0x0F)];
	pc += 2;
}

// 8XY2 BitOp - Vx = Vx & Vy: Sets Vx to Vx AND Vy (Bitwise AND)
void instr8XY2() {
	V[(unsigned char) (opcode >> 8 & 0x0F)] &= V[(unsigned char) (opcode >> 4 & 0x0F)];
	pc += 2;
}

// 8XY3 BitOp - Vx = Vx ^ Vy: Sets Vx to Vx XOR Vy
void instr8XY3() {
	V[(unsigned char) (opcode >> 8 & 0x0F)] ^= V[(unsigned char) (opcode >> 4 & 0x0F)];
	pc += 2;
}

// 8XY4 Math - Vx += Vy: Adds Vy to Vx. VF is set to 1 when there's a carry, and to 0 when there isn't.
void instr8XY4() {
	if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
		V[0xF] = 1; // Carry
	else
		V[0xF] = 0;

	V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
	pc += 2;
}

// 8XY5 Math - Vx -= Vy: Vy is subtracted from Vx. VF is set to 0 when there's a borrow and 1 when there isn't.
void instr8XY5() {
	// Vx = V[(unsigned char) (opcode >> 8 & 0x0F)];
	// Vy = V[(unsigned char) (opcode >> 4 & 0x0F)];
	if(V[(unsigned char) (opcode >> 8 & 0x0F)] > V[(unsigned char) (opcode >> 4 & 0x0F)])
		V[0xF] = 1;
	else
		V[0xF] = 0;

    V[(unsigned char) (opcode >> 8 & 0x0F)] = V[(unsigned char) (opcode >> 8 & 0x0F)] - V[(unsigned char) (opcode >> 4 & 0x0F)];
	pc += 2;
}

// 8XY6 BitOp - Vx = Vy >> 1: Shifts Vy right by one and stores the result to Vx. Set register VF to the least significant bit prior to the shift
void instr8XY6() {
	V[(unsigned char) (opcode >> 8 & 0x0F)] = V[(unsigned char) (opcode >> 4 & 0x0F)] >> 1;

	V[0xF] = V[(unsigned char) (opcode >> 4 & 0x0F)] & 1;
	pc += 2;
}

// 8XY7 Math - Vx = Vy - Vx: Sets Vx to Vy minus Vx. VF is set to 0 when there's a borrow and 1 when there isn't.
void instr8XY7() {
	if(V[(unsigned char) (opcode >> 4 & 0x0F)] > V[(unsigned char) (opcode >> 8 & 0x0F)])
		V[0xF] = 1;
	else
		V[0xF] = 0;

	V[(unsigned char) (opcode >> 8 & 0x0F)] = V[(unsigned char) (opcode >> 4 & 0x0F)] - V[(unsigned char) (opcode >> 8 & 0x0F)];

	pc += 2;
}

// 8XYE BitOp - Vx = Vy << 1: Store the value of register VY shifted left one bit in register VX. Set register VF to the most significant bit prior to the shift
void instr8XYE() {
	V[(unsigned char) (opcode >> 8 & 0x0F)] = V[(unsigned char) (opcode >> 4 & 0x0F)] << 1;

	V[0xF] = V[(unsigned char) (opcode >> 4 & 0x0F)] >> 7 & 1;
	pc += 2;
}

// 9XY0 Cond - if(Vx != Vy): Skips the next instruction if Vx doesn't equal Vy.
void instr9XY0() {
	if(V[(unsigned char) (opcode >> 8 & 0x0F)] != V[(unsigned char) (opcode >> 4 & 0x0F)])
		pc += 4;
	else
		pc += 2;
}

// ANNN MEM - I = NNN: Sets I to the address NNN.
void instrANNN() {
	I = opcode & 0x0FFF;
	pc += 2;
}

// BNNN Flow - PC = V0 + NNN: Jumps to the address NNN plus V0.
void instrBNNN() {
	pc = V[0] + (opcode & 0x0FFF);
}

// CXNN Rand - Vx = rand() & NN: Sets Vx to the result of a bitwise AND operation on a random number and NN.
void instrCXNN() {
	V[(unsigned char) (opcode >> 8 & 0x0F)] = (rand() % 256) & (unsigned char) opcode;
	pc += 2;
}

// Disp - draw(Vx, Vy, N): Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
void instrDXYN() {
	unsigned short x = V[(opcode & 0x0F00) >> 8];
    unsigned short y = V[(opcode & 0x00F0) >> 4];
    unsigned short height = opcode & 0x000F;
    unsigned short pixel;

    V[0xF] = 0;
    for (int yline = 0; yline < height; yline++) {
        pixel = memory[I + yline];
        for(int xline = 0; xline < 8; xline++) {
            if((pixel & (0x80 >> xline)) != 0) {
                if(gfx[(x + xline + ((y + yline) * 64))] == 1)
                    V[0xF] = 1;
                gfx[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
    }

    drawFlag = 1;
    pc += 2;
}

// EX9E KeyOp - if(key() == Vx): Skips the next instruction if the key stored in Vx is pressed.
void instrEX9E() {
	if(key[V[(unsigned char) (opcode >> 8 & 0x0F)]] == 1)
		pc += 4;
	else
		pc += 2;
}

// EXA1 KeyOp - if(key() != Vx): Skips the next instruction if the key stored in Vx is not pressed.
void instrEXA1() {
	if(key[V[(unsigned char) (opcode >> 8 & 0x0F)]] == 0)
		pc += 4;
	else
		pc += 2;
}

// FX07 Timer - Vx = get_delay(): Sets Vx to the value of the delay timer.
void instrFX07() {
	V[(unsigned char) (opcode >> 8 & 0x0F)] = delayTimer;
	pc += 2;
}

// FX0A KeyOp - Vx = get_key(): A key press is awaited, and then stored in VX.
void instrFX0A() {
    char pressed = 0;

    for(int i = 0; i < KEYPAD_SIZE; i++) {
        if(key[i]) {
            V[(unsigned char) (opcode >> 8 & 0x0F)] = i;
            pressed = 1;
        }
    }
    if(!pressed) {
        return;
    }

	pc += 2;
}

// FX15 Timer - delay_timer(Vx): Sets the delay timer to Vx.
void instrFX15() {
	delayTimer = V[(unsigned char) (opcode >> 8 & 0x0F)];

	pc += 2;
}

// FX18 Sound - sound_timer(Vx): Sets the sound timer to Vx.
void instrFX18() {
	soundTimer = V[(unsigned char) (opcode >> 8 & 0x0F)];

	pc += 2;
}

// FX1E MEM - I += Vx: Adds Vx to I.
void instrFX1E() {
	I += V[(unsigned char) (opcode >> 8 & 0x0F)];
	pc += 2;
}

// FX29 MEM - I = sprite_addr[Vx]: Sets I to the location of the sprite for the character in Vx.
void instrFX29() {
    I = V[(unsigned char) (opcode >> 8 & 0x0F)] * 0x5 + MEMORY_FONTSET;
    pc += 2;
}

// FX33 BCD: Store binary-coded decimal representation of VX at the addresses I, I + 1 and I + 2
void instrFX33() {
	memory[I]	  = V[(opcode & 0x0F00) >> 8] / 100;
	memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
	memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
	pc += 2;
}

// FX55 MEM - reg_dump(Vx, &I): Stores V0 to Vx (including Vx) in memory starting at address I.
void instrFX55() {
	for(int i = 0; i <= ((unsigned char) (opcode >> 8 & 0x0F)); i++) {
		memory[I + i] = V[i];
	}

	I += ((opcode & 0x0F00) >> 8) + 1;
	pc += 2;
}

// FX65 MEM - reg_load(Vx, &I): Fills V0 to Vx (including Vx) with values from memory starting at address I.
void instrFX65() {
	for(int i = 0; i <= ((unsigned char) (opcode >> 8 & 0x0F)); i++) {
		V[i] = memory[I + i];
	}

	I += ((opcode & 0x0F00) >> 8) + 1;
	pc += 2;
}
