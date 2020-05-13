#include <iostream>
#include <fstream>

#include "Commands.cpp"
#include "MakingELF.cpp"


int commands_switch(int code_size, int* source_code, char* x86_commands);

int end_func(char* x86_commands, int& cur_position);

int push_num(char* x86_commands, int& cur_position, int number);

int push_reg(char* x86_commands, int& cur_position, char reg_number);

int mov_number_to_reg(char* x86_commands, int& cur_position, char reg_number, int number);

int pop_to_reg(char* x86_commands, int& cur_position, char reg_code);


char const AX_CODE = 0;
char const BX_CODE = 3;
char const CX_CODE = 1;
char const DX_CODE = 2;
char const R12_CODE = 4;

char push_small_num = { 0x6a };
char push_big_num = { 0x68 };

char push_ax = { 0x50 };
char push_bx = { 0x53 };
char push_cx = { 0x51 };
char push_dx = { 0x52 };

char pop_ax = { 0x58 };
char pop_bx = { 0x5b };
char pop_cx = { 0x59 };
char pop_dx = { 0x5a };

char push_r12[2] = { 0x41, 0x54 };

char mov_to_r12[3] = { 0x48, 0xc7, 0xc3 };


int main()
{
	std::ifstream input_code("input.txt");
	std::ofstream elf("D:\\Linux_share_folder\\elf_executable.bin", std::ios::binary);

	int code_size = 0;
	input_code >> code_size;

	int* source_code = new int[code_size]();

	for (int i = 0; i < code_size; ++i)
		input_code >> source_code[i];

	char* x86_commands = new char[10000]();

	int x86_code_size = commands_switch(code_size, source_code, x86_commands) + 1;

	make_elf_header(elf);
	make_programm_header(elf, TOTAL_HEADERS_SIZE + x86_code_size); //size = number of bytes in program + 120
	elf.write(x86_commands, x86_code_size);

	return 0;
}

int commands_switch(int code_size, int* source_code, char* x86_commands)
{
	int cur_position = 0;

	for (int i = 0; i < code_size; ++i)
	{
		switch (source_code[i])
		{

		case CMD_END:
			end_func(x86_commands, cur_position);
			break;
			
		case CMD_PUSH:
			push_num(x86_commands, cur_position, source_code[i + 1]);
			++i;
			break;

		case (CMD_PUSH_ax):
			push_reg(x86_commands, cur_position, AX_CODE);
			break;

		case (CMD_PUSH_bx):
			push_reg(x86_commands, cur_position, BX_CODE);
			break;

		case (CMD_PUSH_cx):
			push_reg(x86_commands, cur_position, CX_CODE);
			break;

		case (CMD_PUSH_dx):
			push_reg(x86_commands, cur_position, DX_CODE);
			break;

		case (CMD_POP_ax):
			pop_to_reg(x86_commands, cur_position, AX_CODE);
			break;

		case (CMD_POP_bx):
			pop_to_reg(x86_commands, cur_position, BX_CODE);
			break;

		case (CMD_POP_cx):
			pop_to_reg(x86_commands, cur_position, CX_CODE);
			break;

		case (CMD_POP_dx):
			pop_to_reg(x86_commands, cur_position, DX_CODE);
			break;
		}
	}

	return cur_position;
}

int end_func(char* x86_commands, int& cur_position)
{
	char program_end[12] = { 0x48, 0xc7, 0xc0, 0x3c, 0, 0, 0, 0x48, 0x31, 0xff, 0x0f, 0x05 }; 

	for (int i = 0; i < 12; ++i)
		x86_commands[cur_position + i] = program_end[i];

	cur_position += 12;

	return 0;
}

int push_num(char* x86_commands, int& cur_position, int number)
{
	const char* number_chars = reinterpret_cast <const char*> (&number);

	if (-128 <= number && number <= 127)
	{
		x86_commands[cur_position] = push_small_num;

		x86_commands[cur_position + 1] = number_chars[0];

		cur_position += 2;
	}

	else
	{
		x86_commands[cur_position] = push_big_num;

		++cur_position;

		for (int i = 0; i < 4; ++i)
			x86_commands[cur_position + i] = number_chars[i];

		cur_position += 4;
	}

	return 0;
}

int push_reg(char* x86_commands, int& cur_position, char reg_code)
{
	switch (reg_code)
	{
	case (AX_CODE):
		x86_commands[cur_position] = push_ax;
		++cur_position;
		break;
	
	case (BX_CODE):
		x86_commands[cur_position] = push_bx;
		++cur_position;
		break;

	case (CX_CODE):
		x86_commands[cur_position] = push_cx;
		++cur_position;
		break;

	case (DX_CODE):
		x86_commands[cur_position] = push_dx;
		++cur_position;
		break;

	case (R12_CODE):
		x86_commands[cur_position] =  push_r12[0];
		x86_commands[cur_position + 1] = push_r12[1];

		cur_position += 2;	
		break;
	}

	return 0;
}

int mov_number_to_reg(char* x86_commands, int& cur_position, char reg_code, int number)
{
	switch (reg_code)
	{
	case (R12_CODE):
		x86_commands[cur_position]     = mov_to_r12[0];
		x86_commands[cur_position + 1] = mov_to_r12[1];
		x86_commands[cur_position + 2] = mov_to_r12[2];

		cur_position += 3;
		break;
	}

	int number_to_char = number;

	const char* number_chars = reinterpret_cast <const char*> (&number_to_char);

	for (int i = 0; i < 4; ++i)
		x86_commands[cur_position + i] = number_chars[i];

	cur_position += 4;

	return 0;
}

int pop_to_reg(char* x86_commands, int& cur_position, char reg_code)
{
	switch (reg_code)
	{
	case (AX_CODE):
		x86_commands[cur_position] = pop_ax;
		++cur_position;
		break;

	case (BX_CODE):
		x86_commands[cur_position] = pop_bx;
		++cur_position;
		break;

	case (CX_CODE):
		x86_commands[cur_position] = pop_cx;
		++cur_position;
		break;

	case (DX_CODE):
		x86_commands[cur_position] = pop_dx;
		++cur_position;
		break;
	}

	return 0;
}
