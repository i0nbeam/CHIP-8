#ifndef CHIP8_OPERATIONS_H
#define CHIP8_OPERATIONS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "raylib.h"


#define RAM_SIZE 65535
#define NUM_REG 16
#define STACK_SIZE 16
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64
#define NUM_KEYS 16
#define PIXEL_SCALING 20
#define PROGRAM_START 512


typedef struct Computer
{
	uint8_t delay_timer;
	uint8_t sound_timer;
	uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
	uint8_t keyboard[NUM_KEYS];
	uint8_t reg[NUM_REG];

	uint16_t RAM[RAM_SIZE];
	uint16_t PC;
	uint16_t index_register;

} Computer;


// fontset that the computer uses

extern uint8_t fontset[80];


// FUNCTIONS

void ram_init(Computer *chip8);

// Store data into RAM, load ROM into RAM, read from RAM

uint16_t ram_read(Computer *chip8, uint16_t addr);
void ram_store(Computer *chip8, uint16_t addr, uint8_t data);
void rom_load(Computer *chip8, const char filename[]);


// stack

extern uint16_t stack[STACK_SIZE];
extern uint16_t stack_pointer;



void stack_push(uint16_t data);
void stack_pop();
uint16_t stack_isfull();
uint16_t stack_isempty();


// Fetch, decode/execute instructions from ROM

uint16_t instruction_fetch(Computer *chip8);
void instruction_execute(Computer *chip8, uint16_t opcode);


// SCREEN

void pixel_clear(Computer *chip8);
void pixel_update(Computer *chip8, uint8_t X, uint8_t Y, uint8_t height);
void pixel_draw(Computer *chip8);



// KEYBOARD

uint8_t key_to_ascii(uint8_t key);
uint8_t ascii_to_key(uint8_t ascii_key);




#endif
