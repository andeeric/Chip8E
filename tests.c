/* file tests.c */

#include <stdio.h>
#include "minunit.h"
#include "chip8.h"

int tests_run = 0;

// chip8 vars
extern unsigned short opcode;

extern void (*instruction)();

extern unsigned char memory[MEMORY_SIZE];

extern unsigned char V[NUM_OF_REGISTERS];
extern unsigned short I;
extern unsigned short pc;

extern unsigned char gfx[NUM_OF_PIXELS];
extern unsigned char drawFlag;

extern unsigned char chip8Fontset[FONTSET_SIZE];

extern unsigned char delayTimer;
extern unsigned char soundTimer;

extern unsigned short stack[STACK_SIZE];
extern unsigned short sp;

extern unsigned char key[KEYPAD_SIZE];

// Tests
static char * testInitialize() {
	initialize();

	mu_assert("error, pc != 0x200", pc == 0x200);
	mu_assert("error, opcode != 0", opcode == 0);
	mu_assert("error, I != 0", I == 0);
	mu_assert("error, sp != 0", sp == 0);

	for(int i = 0; i < NUM_OF_PIXELS; i++)
		mu_assert("error, gfx[i] != NULL", gfx[i] == NULL);

	for(int i = 0; i < STACK_SIZE; i++)
		mu_assert("error, stack[i] != NULL", stack[i] == NULL);

	for(int i = 0; i < NUM_OF_REGISTERS; i++)
		mu_assert("error, V[i] != NULL", V[i] == NULL);

	for(int i = 0; i < MEMORY_FONTSET; i++)
		mu_assert("error, memory[i] != NULL (before fontset)", memory[i] == NULL);
	for(int i = 0; i < FONTSET_SIZE; i++)
		mu_assert("error, fontset not loaded properly", memory[MEMORY_FONTSET + i] == chip8Fontset[i]);
	for(int i = MEMORY_PROGRAM; i < MEMORY_SIZE; i++)
		mu_assert("error, programmable memory[i] != NULL", memory[i] == NULL);

	mu_assert("error, delayTimer != 0", delayTimer == 0);
	mu_assert("error, soundTimer != 0", soundTimer == 0);

	return 0;
}

// 00E0 Display - disp_clear: Clears the screen
static char * test00E0() {
    for(int i = 0; i < NUM_OF_PIXELS; i++) {
        if(i % 45 == 0)
            gfx[i] = 1;
    }

    instr00E0();

	for(int i = 0; i < NUM_OF_PIXELS; i++)
        mu_assert("error instr00E0, gfx[i] != 0", gfx[i] == 0);

    return 0;
}

// 00EE Flow - return;: Returns from a subroutine
static char * test00EE() {
    // IMPLEMENT
    // Go to subroutine
    // test return
    return 0;
}

// 1NNN Flow - Goto NNN;: Jumps to address NNN.
static char * test1NNN() {
    opcode = 0x1020;
    instr1NNN();
    mu_assert("error instr1NNN, pc != 0x0020", pc == 0x0020);

    return 0;
}

// 2NNN Flow - *(0xNNN)(): Calls subroutine at NNN
static char * test2NNN() {
    opcode = 0x2020;
    pc = 10;
    sp = 0;

    instr2NNN();

    mu_assert("error instr2NNN, pc != 0x0020", pc == 0x0020);
    mu_assert("error instr2NNN, sp != 1", sp == 1);
    mu_assert("error instr2NNN, stack[0] != 10", stack[0] == 10);

    return 0;
}

// 3XNN Cond - if(Vx == NN): Skips the next instruction if Vx equals NN.
static char * test3XNN() {
    // Vx != NN
    pc = 0;
    opcode = 0x3103;
    V[1] = 2;

    instr3XNN();

    mu_assert("error instr3XNN, pc != 0x0002", pc == 0x0002);

    // Vx == NN
    pc = 0;
    opcode = 0x3102;

    instr3XNN();

    mu_assert("error instr3XNN, pc != 0x0004", pc == 0x0004);

    return 0;
}

// 4XNN Cond - if(Vx != NN): Skips the next instruction if Vx does not equal
static char * test4XNN() {
    // Vx != NN
    pc = 0;
    opcode = 0x3103;
    V[1] = 2;

    instr4XNN();

    mu_assert("error instr4XNN, pc != 0x0004", pc == 0x0004);

    // Vx == NN
    pc = 0;
    opcode = 0x3102;

    instr4XNN();

    mu_assert("error instr4XNN, pc != 0x0002", pc == 0x0002);

    return 0;
}

// 5XY0 Cond - if(Vx == Vy): Skips the next instruction if Vx equals Vy.
static char * test5XY0() {
    // Vx != Vy
    pc = 0;
    opcode = 0x5120;
    V[1] = 10;
    V[2] = 5;

    instr5XY0();

    mu_assert("error instr5XY0, pc != 0x0002", pc == 0x0002);

    // Vx == Vy
    pc = 0;
    opcode = 0x5120;
    V[1] = 90;
    V[2] = 90;

    instr5XY0();

    mu_assert("error instr5XY0, pc != 0x0004", pc == 0x0004);

    return 0;
}

// 6XNN Const - Vx = NN: Sets Vx to NN.
static char * test6XNN() {
    pc = 0;
    opcode = 0x6340;
    V[3] = 10;

    instr6XNN();

    mu_assert("error instr6XNN, V3 != 0x0040", V[3] == 0x0040);
    mu_assert("error instr6XNN, pc != 0x0002", pc == 0x0002);

    return 0;
}

// 7XNN Const - Vx += NN: Adds NN to Vx. (Carry flag is not changed)
static char * test7XNN() {
    pc = 0;
    opcode = 0x7340;
    V[3] = 0x40;
    V[0xF] = 0;

    instr7XNN();

    mu_assert("error instr7XNN, V3 != 0x0080", V[3] == 0x0080);
    mu_assert("error instr7XNN, Carry flag changed", V[0xF] == 0);
    mu_assert("error instr7XNN, pc != 0x0002", pc == 0x0002);

    return 0;
}

// 8XY0 Assign - Vx = Vy: Sets Vx to the value of Vy.
static char * test8XY0() {
    pc = 0;
    opcode = 0x8230;
    V[2] = 10;
    V[3] = 253;

    instr8XY0();

    mu_assert("error instr8XY0, Unexpected val change in V3", V[3] == 253);
    mu_assert("error instr8XY0, V2 != V3", V[2] == V[3]);
    mu_assert("error instr8XY0, pc != 0x0002", pc == 0x0002);

    return 0;
}

// 8XY1 BitOp - Vx = Vx | Vy: Sets Vx to Vx OR Vy (Bitwise OR)
static char * test8XY1() {
    pc = 0;
    opcode = 0x8581;
    V[5] = 0b0010;
    V[8] = 0b1100;

    instr8XY1();

    mu_assert("error instr8XY1, Unexpected val change in V8", V[8] == 0b1100);
    mu_assert("error instr8XY1, V5 != 0b1110", V[5] == 0b1110);
    mu_assert("error instr8XY1, pc != 0x0002", pc == 0x0002);

    return 0;
}

// 8XY2 BitOp - Vx = Vx & Vy: Sets Vx to Vx AND Vy (Bitwise AND)
static char * test8XY2() {
    pc = 0;
    opcode = 0x8582;
    V[5] = 0b0100;
    V[8] = 0b1100;

    instr8XY2();

    mu_assert("error instr8XY2, Unexpected val change in V8", V[8] == 0b1100);
    mu_assert("error instr8XY2, V5 != 0b0100", V[5] == 0b0100);
    mu_assert("error instr8XY2, pc != 0x0002", pc == 0x0002);

    return 0;
}

// 8XY3 BitOp - Vx = Vx ^ Vy: Sets Vx to Vx XOR Vy
static char * test8XY3() {
    pc = 0;
    opcode = 0x8583;
    V[5] = 0b1010;
    V[8] = 0b0010;

    instr8XY3();

    mu_assert("error instr8XY2, Unexpected val change in V8", V[8] == 0b0010);
    mu_assert("error instr8XY3, V5 != 0b1000", V[5] == 0b1000);
    mu_assert("error instr8XY3, pc != 0x0002", pc == 0x0002);

    return 0;
}

// 8XY4 Math - Vx += Vy: Adds Vy to Vx. VF is set to 1 when there's a carry, and to 0 when there isn't.
static char * test8XY4() {
    // No carry
    pc = 0;
    opcode = 0x8584;
    V[5] = 0x10;
    V[8] = 0x20;
    V[0xF] = 0;

    instr8XY4();

    mu_assert("error instr8XY4, V5 != 0x30", V[5] == 0x30);
    mu_assert("error instr8XY4, Unexpected val change in VF", V[0xF] == 0);
    mu_assert("error instr8XY4, pc != 0x0002", pc == 0x0002);

    // Carry
    V[5] = 0xFF;
    V[8] = 0x01;

    instr8XY4();

    mu_assert("error instr8XY4, V5 != 0x0", V[5] == 0x0);
    mu_assert("error instr8XY4, VF not set on overflow", V[0xF] == 1);

    return 0;
}

// 8XY5 Math - Vx -= Vy: Vy is subtracted from Vx. VF is set to 0 when there's a borrow and 1 when there isn't.
static char * test8XY5() {
    // No borrow
    pc = 0;
    opcode = 0x8584;
    V[5] = 0x20;
    V[8] = 0x10;
    V[0xF] = 0;

    instr8XY5();

    mu_assert("error instr8XY5, V5 != 0x10", V[5] == 0x10);
    mu_assert("error instr8XY5, VF = 0 on borrow", V[0xF] == 1);
    mu_assert("error instr8XY5, pc != 0x0002", pc == 0x0002);

    // Borrow
    V[5] = 0x05;
    V[8] = 0x07;

    instr8XY5();

    mu_assert("error instr8XY5, V5 != 254", V[5] == 254);
    mu_assert("error instr8XY5, VF = 1 on borrow", V[0xF] == 0);

    return 0;
}

// 8XY6 BitOp - Vx = Vy >> 1: Shifts Vy right by one and stores the result to Vx. Set register VF to the least significant bit prior to the shift
static char * test8XY6() {
    pc = 0;
    opcode = 0x8236;
    V[2] = 83;
    V[3] = 2;

    instr8XY6();

    mu_assert("error instr8XY6, unexpected val change in V3", V[3] == 0x0002);
    mu_assert("error instr8XY6, VF != 0", V[0xF] == 0);
    mu_assert("error instr8XY6, V2 != 1", V[2] == 0x0001);
    mu_assert("error instr8XY6, pc != 0x0002", pc == 0x0002);

    V[2] = 83;
    V[3] = 9;

    instr8XY6();

    mu_assert("error instr8XY6, unexpected val change in V3", V[3] == 0x0009);
    mu_assert("error instr8XY6, VF != 1", V[0xF] == 1);
    mu_assert("error instr8XY6, V2 != 4", V[2] == 0x0004);

    return 0;
}

// 8XY7 Math - Vx = Vy - Vx: Sets Vx to Vy minus Vx. VF is set to 0 when there's a borrow and 1 when there isn't.
static char * test8XY7() {
    // No borrow
    pc = 0;
    opcode = 0x8587;
    V[5] = 0x10;
    V[8] = 0x20;
    V[0xF] = 0;

    instr8XY7();

    mu_assert("error instr8XY7, V5 != 0x10", V[5] == 0x10);
    mu_assert("error instr8XY7, VF = 0 on no borrow", V[0xF] == 1);
    mu_assert("error instr8XY7, Unexpected val change in V8", V[8] == 0x20);
    mu_assert("error instr8XY7, pc != 0x0002", pc == 0x0002);

    // Borrow
    opcode = 0x8587;
    V[5] = 0x01;
    V[8] = 0x00;
    V[0xF] = 0;

    instr8XY7();

    mu_assert("error instr8XY7, V5 != 0xFF", V[5] == 0xFF);
    mu_assert("error instr8XY7, VF = 1 on borrow", V[0xF] == 0);
    mu_assert("error instr8XY7, Unexpected val change in V8", V[8] == 0x00);

    return 0;
}

// 8XYE BitOp - Vx = Vy << 1: Store the value of register VY shifted left one bit in register VX. Set register VF to the most significant bit prior to the shift
static char * test8XYE() {
    pc = 0;
    opcode = 0x823E;
    V[2] = 83;
    V[3] = 2;

    instr8XYE();

    mu_assert("error instr8XYE, unexpected val change in V3", V[3] == 0x0002);
    mu_assert("error instr8XYE, VF != 0", V[0xF] == 0);
    mu_assert("error instr8XYE, V2 != 4", V[2] == 0x0004);
    mu_assert("error instr8XYE, pc != 0x0002", pc == 0x0002);

    V[2] = 83;
    V[3] = 128;

    instr8XYE();

    mu_assert("error instr8XYE, unexpected val change in V3", V[3] == 128);
    mu_assert("error instr8XYE, VF != 1", V[0xF] == 1);
    mu_assert("error instr8XYE, V2 != 0", V[2] == 0x0000);
}

// 9XY0 Cond - if(Vx != Vy): Skips the next instruction if Vx doesn't equal Vy.
static char * test9XY0() {
    // Skip
    pc = 0;
    opcode = 0x9120;
    V[1] = 0x10;
    V[2] = 0x20;

    instr9XY0();

    mu_assert("error instr9XY0, pc != 0x0004", pc == 0x0004);

    // No skip
    pc = 0;
    V[1] = 0x10;
    V[2] = 0x10;

    instr9XY0();

    mu_assert("error instr9XY0, pc != 0x0002", pc == 0x0002);

    return 0;
}

// ANNN MEM - I = NNN: Sets I to the address NNN.
static char * testANNN() {
    pc = 0;
    opcode = 0xA120;
    I = 0x20;

    instrANNN();

    mu_assert("error instrANNN, I != 0x0120", I == 0x0120);
    mu_assert("error instrANNN, pc != 0x0002", pc == 0x0002);

    return 0;
}

// BNNN Flow - PC = V0 + NNN: Jumps to the address NNN plus V0.
static char * testBNNN() {
    pc = 0;
    opcode = 0xB100;
    V[0] = 0x01;

    instrBNNN();

    mu_assert("error instrBNNN, pc != 0x0101", pc == 0x0101);

    return 0;
}

// CXNN Rand - Vx = rand() & NN: Sets Vx to the result of a bitwise AND operation on a random number and NN.
static char * testCXNN() {
    pc = 0;
    opcode = 0xC180;
    V[1] = 0x10;

    instrCXNN();

    mu_assert("error instrCXNN, V1 unchanged (small chance this test fails since it uses a randomised number, run again for certainty)", V[1] != 0x0010);
    mu_assert("error instrCXNN, pc != 0x0002", pc == 0x0002);

    return 0;
}

// DXYN Disp - draw(Vx, Vy, N): Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
static char * testDXYN() {
    // TODO
    return 0;
}

// EX9E KeyOp - if(key() == Vx): Skips the next instruction if the key stored in Vx is pressed.
static char * testEX9E() {
    // Not pressed
    pc = 0;
    opcode = 0xE19E;
    V[1] = 0;
    key[0] = 0;

    instrEX9E();

    mu_assert("error instrEX9E, pc != 0x0002", pc == 0x0002);

    // Pressed
    pc = 0;
    key[0] = 1;

    instrEX9E();

    mu_assert("error instrEX9E, pc != 0x0004", pc == 0x0004);

    return 0;
}

// EXA1 KeyOp - if(key() != Vx): Skips the next instruction if the key stored in Vx is not pressed.
static char * testEXA1() {
    // Not pressed
    pc = 0;
    opcode = 0xE3A1;
    V[3] = 5;
    key[5] = 0;

    instrEXA1();

    mu_assert("error instrEXA1, pc != 0x0004", pc == 0x0004);

    // Pressed
    pc = 0;
    key[5] = 1;

    instrEXA1();

    mu_assert("error instrEXA1, pc != 0x0002", pc == 0x0002);

    return 0;
}

// FX07 Timer - Vx = get_delay(): Sets Vx to the value of the delay timer.
static char * testFX07() {
    pc = 0;
    opcode = 0xF707;
    V[7] = 230;
    delayTimer = 50;

    instrFX07();

    mu_assert("error instrFX07, unexpected val change in delayTimer", delayTimer == 50);
    mu_assert("error instrFX07, V7 != 50", V[7] == 50);
    mu_assert("error instrFX07, pc != 0x0002", pc == 0x0002);

    return 0;
}

// FX0A KeyOp - Vx = get_key(): A key press is awaited, and then stored in VX.
static char * testFX0A() {
    // TODO
    return 0;
}

// FX15 Timer - delay_timer(Vx): Sets the delay timer to Vx.
static char * testFX15() {
    pc = 0;
    opcode = 0xF215;
    V[2] = 89;
    delayTimer = 100;

    instrFX15();

    mu_assert("error instrFX15, unexpected val change in V2", V[2] == 89);
    mu_assert("error instrFX15, delayTimer != 89", delayTimer == 89);
    mu_assert("error instrFX15, pc != 0x0002", pc == 0x0002);

    return 0;
}

// FX18 Sound - sound_timer(Vx): Sets the sound timer to Vx.
static char * testFX18() {
    pc = 0;
    opcode = 0xF215;
    V[2] = 89;
    soundTimer = 100;

    instrFX18();

    mu_assert("error instrFX18, unexpected val change in V2", V[2] == 89);
    mu_assert("error instrFX18, soundTimer != 89", delayTimer == 89);
    mu_assert("error instrFX18, pc != 0x0002", pc == 0x0002);

    return 0;
}

// FX1E MEM - I += Vx: Adds Vx to I.
static char * testFX1E() {
    pc = 0;
    opcode = 0xF91E;
    V[9] = 10;
    I = 255;

    instrFX1E();

    mu_assert("error instrFX1E, unexpected val change in V9", V[9] == 10);
    mu_assert("error instrFX1E, I != 265", I == 265);
    mu_assert("error instrFX1E, pc != 0x0002", pc == 0x0002);

    return 0;
}

// FX29 MEM - I = sprite_addr[Vx]: Sets I to the location of the sprite for the character in Vx.
static char * testFX29() {
    // TODO
    return 0;
}

// FX33 BCD: Store binary-coded decimal representation of VX at the addresses I, I + 1 and I + 2
static char * testFX33() {
    // TODO
    return 0;
}

// FX55 MEM - reg_dump(Vx, &I): Stores V0 to Vx (including Vx) in memory starting at address I.
static char * testFX55() {
    // TODO
    return 0;
}

// FX65 MEM - reg_load(Vx, &I): Fills V0 to Vx (including Vx) with values from memory starting at address I.
static char * testFX65() {
    // TODO
    return 0;
}

static char * all_tests() {
    mu_run_test(testInitialize);

    // Instructions
    mu_run_test(test00E0);
    mu_run_test(test00EE);
    mu_run_test(test1NNN);
    mu_run_test(test2NNN);
    mu_run_test(test3XNN);
    mu_run_test(test4XNN);
    mu_run_test(test5XY0);
    mu_run_test(test6XNN);
    mu_run_test(test7XNN);
    mu_run_test(test8XY0);
    mu_run_test(test8XY1);
    mu_run_test(test8XY2);
    mu_run_test(test8XY3);
    mu_run_test(test8XY4);
    mu_run_test(test8XY5);
    mu_run_test(test8XY6);
    mu_run_test(test8XY7);
    mu_run_test(test8XYE);
    mu_run_test(test9XY0);
    mu_run_test(testANNN);
    mu_run_test(testBNNN);
    mu_run_test(testCXNN);
    mu_run_test(testDXYN);
    mu_run_test(testEX9E);
    mu_run_test(testEXA1);
    mu_run_test(testFX07);
    mu_run_test(testFX0A);
    mu_run_test(testFX15);
    mu_run_test(testFX18);
    mu_run_test(testFX1E);

    return 0;
}

// Run tests
int testmain(int argc, char **argv) {
	char *result = all_tests();
	if (result != 0) {
		printf("%s\n", result);
	}
	else {
		printf("ALL TESTS PASSED\n");
	}
	printf("Tests run: %d\n", tests_run);

	return result != 0;
}
