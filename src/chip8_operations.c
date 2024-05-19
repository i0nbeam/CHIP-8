#include "chip8_operations.h"


// Initializes timers, program counter, index register, clears screen, declare fontset

void ram_init(Computer *chip8)
{
	chip8->delay_timer = 0;
	chip8->sound_timer = 0;
	chip8->PC = PROGRAM_START;
	chip8->index_register = 0;
	memset(chip8->reg, 0, NUM_REG);

	pixel_clear(chip8);

}


uint8_t fontset[80] = 
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



// reads what is in memory address given by addr

uint16_t ram_read(Computer *chip8, uint16_t addr)
{

	return chip8->RAM[addr];

}

// stores data in address addr

void ram_store(Computer *chip8, uint16_t addr, uint8_t data)
{

	chip8->RAM[addr] = data;

}


// loads a rom called filename into memory
// use fseek to set file position to 0th byte relative to EOF, so it goes to the last byte
// ftell returns where the file position is. its gonna be the size of the file so set size
// rewind goes back to the beginning of the file, and allocate an 8bit buffer the same size of the rom (each instruction is 8 bits = 1 byte)
// fread puts the entire rom into the buffer
// load from buffer into ram in big endian

void rom_load(Computer *chip8, const char filename[])
{
	FILE *rom = fopen(filename, "rb");

	fseek(rom, 0L, SEEK_END);
	long size = ftell(rom);
	rewind(rom);
	uint8_t buffer[size];

	fread(buffer, sizeof buffer[0], size, rom);

	// load big endian
	for (int i = 0; i <= size - 2; i += 2)
	{
		ram_store(chip8, i + PROGRAM_START, buffer[i]);
		ram_store(chip8, i + 1 + PROGRAM_START, buffer[i+1]);
	}

	fclose(rom);
}



/*

KEYBOARD

*/

uint8_t key_to_ascii(uint8_t key)
{
	uint8_t ascii = 0;
	ascii = (key <= 9) ? key + 0x30 : key + 0x31;
	return ascii;

}


uint8_t ascii_to_key(uint8_t ascii_key)
{
	uint8_t key = 0;
	key = (ascii_key <= 0x39) ? ascii_key - 30 : ascii_key - 31;
        return key;

}

/*

STACK

*/

uint16_t stack[STACK_SIZE];
uint16_t stack_pointer = STACK_SIZE;

uint16_t stack_isfull()
{
	return (stack_pointer == 0);	
}

uint16_t stack_isempty()
{
	return (stack[STACK_SIZE] == 0);			
}


void stack_push(uint16_t data)
{
	if (stack_isfull())
	{
		printf("Stack full!\n");
		exit(0);
	}
	else
	{
		if (stack_isempty())
		{
			stack[stack_pointer] = data;
		}
		else
		{
			stack_pointer--;
			stack[stack_pointer] = data;
		}

	}
}


void stack_pop(Computer *chip8)
{
	if (stack_isempty())
	{
		printf("Stack is empty!\n");
	}	
	else
	{
		chip8->PC = stack[stack_pointer];
		stack_pointer++;
	}

}

/*

SCREEN

*/


void pixel_clear(Computer *chip8)
{
	memset(chip8->screen, 0, sizeof(chip8->screen));
}


void pixel_update(Computer *chip8, uint8_t posX, uint8_t posY, uint8_t height)
{	

	for (int num_byte = 0; num_byte < height; num_byte++)
	{

		uint16_t index_register = chip8->index_register;
		uint8_t current_byte = ram_read(chip8, num_byte + index_register);
		uint8_t bit_selector = 0x80;
		int num_bits_to_shift = 7;
		int xpixel = 0;

		while (bit_selector != 0)
		{
			uint8_t bit = current_byte & bit_selector;
			
			bit = bit >> num_bits_to_shift;
			uint8_t pixel = chip8->screen[num_byte + posY][xpixel + posX];
				
			switch (bit ^ pixel)
			{
			
				case 1:
					chip8->reg[0xF] = bit & pixel;
					chip8->screen[num_byte + posY][xpixel + posX] = 1;
					pixel_draw(chip8);
					break;
				case 0:
					chip8->reg[0xF] = bit & pixel;
					chip8->screen[num_byte + posY][xpixel + posX] = 0;
					pixel_draw(chip8);
					break;
				default:
					fprintf(stderr, "drawing went to default!\n");
					break;
			}

			bit_selector = bit_selector >> 1;
			num_bits_to_shift -= 1;
			xpixel += 1;

		} // end while	


	} // end for
	
	

} // end pixels_update


void pixel_draw(Computer *chip8)
{
	for (int row = 0; row < SCREEN_HEIGHT; row++)
	{
		for (int column = 0; column < SCREEN_WIDTH; column++)
		{
			switch (chip8->screen[row][column])
			{
				case 1:
					DrawRectangle(column * PIXEL_SCALING, row * PIXEL_SCALING, 20, 20, WHITE);
					break;
				case 0:
					DrawRectangle(column * PIXEL_SCALING, row * PIXEL_SCALING, 20, 20, BLACK);
					break;
			}
		}	
	}
	
	
} // end pixel_draw


// Fetch, decode, execute instructions from ROM

uint16_t instruction_fetch(Computer *chip8)
{
	uint16_t PC = chip8->PC;

	uint16_t high_byte = chip8->RAM[PC];
	high_byte = high_byte << 8;

	uint16_t low_byte = chip8->RAM[PC+1];

	uint16_t opcode = high_byte | low_byte;

	chip8->PC += 2;

	return opcode;
}



void instruction_execute(Computer *chip8, uint16_t opcode)
{

	uint16_t instr = opcode & 0xF000;

	uint16_t X = (opcode & 0x0F00) >> 8;
	uint16_t Y = (opcode & 0x00F0) >> 4;

	uint16_t N = opcode & 0x000F;
	uint16_t NN = opcode & 0x00FF;
	uint16_t NNN = opcode & 0x0FFF;

	switch(instr)
	{

		case 0x0000:
			switch (N)
			{
				case 0x0000:
					pixel_clear(chip8);
					ClearBackground(BLACK);
					break;
				case 0x000E:
					stack_pop(chip8);
					break;
			}
			break;
		case 0x1000:
			chip8->PC = NNN;
			break;
		case 0x2000:
			stack_push(chip8->PC);
			chip8->PC = NNN;
			break;
		case 0x3000:
			if (chip8->reg[X] == NN) chip8->PC += 2;
			break;
		case 0x4000:
			if (chip8->reg[X] != NN) chip8->PC += 2;
			break;
		case 0x5000:
			if (chip8->reg[X] == chip8->reg[Y]) chip8->PC += 2;
			break;
		case 0x6000:
			chip8->reg[X] = NN;
			break;
		case 0x7000:
			chip8->reg[X] += NN;
			break;
		case 0x8000:
			switch (N)
			{
				case 0x00:
					chip8->reg[X] = chip8->reg[Y];
					break;
				case 0x01:
					chip8->reg[X] = chip8->reg[X] | chip8->reg[Y];
					break;
				case 0x02:
					chip8->reg[X] = chip8->reg[X] & chip8->reg[Y];
					break;
				case 0x03:
					chip8->reg[X] = chip8->reg[X] ^ chip8->reg[Y];
					break;
				case 0x04:
					uint16_t sum = chip8->reg[X] + chip8->reg[Y];
					chip8->reg[X] = sum & 0xFF;
					chip8->reg[0xF] = (sum > 255) ? 1 : 0;
					break;
				case 0x05:
					chip8->reg[X] = chip8->reg[X] - chip8->reg[Y];
					chip8->reg[0xF] = (chip8->reg[X] > chip8->reg[Y]) ? 1 : 0;
					break;
				case 0x06:
					uint16_t lsb = chip8->reg[Y] & 1;
					chip8->reg[X] = chip8->reg[Y] >> 1;
					chip8->reg[0xF] = lsb;
					break;
				case 0x07:
					chip8->reg[X] = chip8->reg[Y] - chip8->reg[X];
					chip8->reg[0xF] = (chip8->reg[Y] > chip8->reg[X]) ? 1 : 0;
					break;
				case 0x0E:
					uint16_t msb = chip8->reg[Y] & 0x80;
					chip8->reg[X] = chip8->reg[Y] << 1;
					chip8->reg[0xF] = msb;
					break;
			}
			break;
		case 0x9000:
			if (chip8->reg[X] != chip8->reg[Y]) chip8->PC += 2;
			break;
		case 0xA000:
			chip8->index_register = NNN;
			break;
		case 0xB000:
			chip8->PC = NNN + chip8->reg[0];
			break;
		case 0xC000:
			uint8_t randbyte = rand() % 255;
			chip8->reg[X] = randbyte & NN;
			break;
		case 0xD000:
			uint16_t posX = chip8->reg[X];
			uint16_t posY = chip8->reg[Y];
			pixel_update(chip8, posX, posY, N);
			break;
		case 0xE000:
			switch (NN)
			{
				case 0x9E:
					uint8_t keydown = chip8->reg[X];
					uint8_t keydown_ascii = key_to_ascii(keydown);
					if (IsKeyDown(keydown_ascii)) chip8->PC += 2;
					break;
				case 0xA1:
					uint8_t keyup = chip8->reg[X];
					uint8_t keyup_ascii = key_to_ascii(keyup);
					if (IsKeyUp(keyup_ascii)) chip8->PC += 2;
					break;
			}
			break;
		case 0xF000:
			switch (NN)
			{
				case 0x07:
					chip8->reg[X] = chip8->delay_timer;
					break;
				case 0x0A:
					uint8_t key = 0;
					do
					{
						key = GetKeyPressed();

					} while (key == 0);
					chip8->reg[X] = key;
					break;
				case 0x15:
					chip8->delay_timer = chip8->reg[X];
					break;
				case 0x18:
					chip8->sound_timer = chip8->reg[X];
					break;
				case 0x1E:
					chip8->index_register += chip8->reg[X];
					break;
				case 0x29:
					uint8_t sprite = chip8->reg[X];
					chip8->index_register = sprite * 5;
					break;
				case 0x33:
					uint8_t num = chip8->reg[X];
					int offset1 = 2;
					while (num > 0)
					{
						uint8_t digit = num % 10;
						ram_store(chip8, chip8->index_register + offset1, digit);
					        num /= 10;
						offset1--;
					}
					break;
				case 0x55:
					for (int offset2 = 0; offset2 <= X; offset2++)
					{
						ram_store(chip8, chip8->index_register + offset2, chip8->reg[offset2]);
					}
					break;
				case 0x65:
					for (int regnum = 0; regnum <= X; regnum++)
					{
						uint8_t data = ram_read(chip8, chip8->index_register + regnum);
						chip8->reg[regnum] = data;
					}
					break;
			}
			break;
		default:
			printf("unknown opcode!\n");
			break;
	}


}













