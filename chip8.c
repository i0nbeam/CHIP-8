#include "chip8_operations.h"
#include "time.h"


int main(int argc, char **argv)
{
	
	srand(time(NULL));
	
	/*
	Create instance of chip8 struct
	The struct holds arrays that represent RAM, registers, and screen
	*/
	Computer chip8;
	
	// Create pointer to chip8 struct
	Computer *chip8_ptr = &chip8;

	// initialize RAM, registers
	ram_init(chip8_ptr);
	
	/* Load ROM into RAM
	   Provide ROM filename/filepath 
	*/
 	rom_load(chip8_ptr, argv[1]);
	
	// load fontset into RAM starting at address 0x0000
	for (int i = 0; i < 80; i++)
	{
		ram_store(chip8_ptr, i, fontset[i]);
	}
	
	// Init window and fps 
	InitWindow(SCREEN_WIDTH * PIXEL_SCALING, SCREEN_HEIGHT * PIXEL_SCALING, "CHIP-8 Interpreter");
	SetTargetFPS(500);
	
	// Main loop
	while(!WindowShouldClose())
	{
		// Set up window for drawing
        	BeginDrawing();

		// Fetch instruction and execute
		uint16_t opcode = instruction_fetch(chip8_ptr);
		instruction_execute(chip8_ptr, opcode);
        
		EndDrawing();
		chip8_ptr->sound_timer--;
		chip8_ptr->delay_timer--;
    	}

	CloseWindow();
	
    	return 0;

}



