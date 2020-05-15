#include <iostream>
#include <fstream>

#include "Commands.cpp"
#include "MakingELF.cpp"


int commands_switch(int code_size, int* source_code, char* x86_commands, int* abs_addresses);

int end_func(char* x86_commands, int& cur_position);

int push_num(char* x86_commands, int& cur_position, int number);

int push_reg(char* x86_commands, int& cur_position, char reg_number);

int mov_number_to_reg(char* x86_commands, int& cur_position, char reg_number, int number);

int pop_to_reg(char* x86_commands, int& cur_position, char reg_code);

int add_sub_func(char* x86_commands, int& cur_position, int operation);

int imul_func(char* x86_commands, int& cur_position);

int idiv_func(char* x86_commands, int& cur_position);

int fill_abs_addresses(int* source_code, int* abs_addresses, int code_size);

int jmp_func(char* x86_commands, int& cur_position, int dest_command, int* abs_addresses, int cur_command, int jump_code);

int cmp_func(char* x86_commands, int& cur_position);


char const AX_CODE = 0;
char const BX_CODE = 3;
char const CX_CODE = 1;
char const DX_CODE = 2;
char const R12_CODE = 4;
char const R13_CODE = 5;
char const R14_CODE = 6;
char const RSI_CODE = 7;
char const RDI_CODE = 8;

char push_small_num = { 0x6a };
char push_big_num = { 0x68 };

char push_ax = { 0x50 };
char push_bx = { 0x53 };
char push_cx = { 0x51 };
char push_dx = { 0x52 };
char push_rsi = { 0x56 };
char push_rdi = { 0x57 };

char push_r14[2] = { 0x41, 0x56 };

char pop_ax = { 0x58 };
char pop_bx = { 0x5b };
char pop_cx = { 0x59 };
char pop_dx = { 0x5a };
char pop_rsi = { 0x5e };
char pop_rdi = { 0x5f };

char pop_r13 = { 0x5d };
char pop_r14 = { 0x5e };

char push_r12[2] = { 0x41, 0x54 };

char mov_to_r12[3] = { 0x48, 0xc7, 0xc3 };

char mov_rdi_rax[3] = { 0x48, 0x89, 0xc7 };
char mov_rsi_rdx[3] = { 0x48, 0x89, 0xd6 };
char mov_rax_rdi[3] = { 0x48, 0x89, 0xf8 };

char mov_rdi_num[3] = { 0x48, 0xc7, 0xc7 };

char add_r14_r13[3] = { 0x4d, 0x01, 0xee };
char add_rdi_rsi[3] = { 0x48, 0x01, 0xf7 };

char sub_rdi_rsi[3] = { 0x48, 0x29, 0xf7 };

char imul_rdi_rsi[4] = { 0x48, 0x0f, 0xaf, 0xfe };
char imul_rsi[3] = { 0x48, 0xf7, 0xfe };

char jmp = 0xe9;
char ja[2] = { 0x0f, 0x87 };
char jb[2] = { 0x0f, 0x82 };
char je[2] = { 0x0f, 0x84 };

char cmp_reg[2] = { 0x48, 0x39 };
char cmp_rsi_rdi = { 0xfe };

int main()
{
	std::ifstream input_code("asm_output.txt");
	std::ofstream elf("D:\\Linux_share_folder\\elf_executable.bin", std::ios::binary);

	int code_size = 0;
	input_code >> code_size;

	int* source_code = new int[code_size]();

	for (int i = 0; i < code_size; ++i)
		input_code >> source_code[i];


	int* abs_addresses = new int[code_size]();

	fill_sizes();
	fill_abs_addresses(source_code, abs_addresses, code_size);

	char* x86_commands = new char[10000]();

	int x86_code_size = commands_switch(code_size, source_code, x86_commands, abs_addresses) + 1;

	make_elf_header(elf);
	make_programm_header(elf, TOTAL_HEADERS_SIZE + x86_code_size); //size = number of bytes in program + 120
	elf.write(x86_commands, x86_code_size);

	for (int i = 0; i < code_size; ++i)
		std::cout << std::hex << abs_addresses[i] << " ";

	return 0;
}

int fill_abs_addresses(int* source_code, int* abs_addresses, int code_size)
{
	int address = 0x400078;

	for (int cur_command = 0; cur_command < code_size; ++cur_command)
	{
		abs_addresses[cur_command] = address;

		if (source_code[cur_command] == CMD_PUSH)
		{
			if (-128 <= source_code[cur_command + 1] && source_code[cur_command + 1] <= 127)
				address += 2;
			else
				address += 5;

			++cur_command;
		}

		else if (CMD_JMP <= source_code[cur_command] && source_code[cur_command] <= CMD_JE)
		{
			address += commands_size_in_bytes[source_code[cur_command]];
			
			++cur_command;
		}

		else
			address += commands_size_in_bytes[source_code[cur_command]];	
	}

	return 0;
}

int commands_switch(int code_size, int* source_code, char* x86_commands, int* abs_addresses)
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

		case (CMD_ADD):
			add_sub_func(x86_commands, cur_position, CMD_ADD);
			break;

		case (CMD_SUB):
			add_sub_func(x86_commands, cur_position, CMD_SUB);
			break;

		case (CMD_MUL):
			imul_func(x86_commands, cur_position);
			break;

		case (CMD_DIV):
			idiv_func(x86_commands, cur_position);
			break;

		case (CMD_JMP):
			jmp_func(x86_commands, cur_position, source_code[i + 1], abs_addresses, i, CMD_JMP);
			++i;
			break;

		case (CMD_JA):
			jmp_func(x86_commands, cur_position, source_code[i + 1], abs_addresses, i, CMD_JA);
			++i;
			break;

		case (CMD_JB):
			jmp_func(x86_commands, cur_position, source_code[i + 1], abs_addresses, i, CMD_JB);
			++i;
			break;

		case (CMD_JE):
			jmp_func(x86_commands, cur_position, source_code[i + 1], abs_addresses, i, CMD_JE);
			++i;
			break;
		}
	}

	return cur_position;
}

int end_func(char* x86_commands, int& cur_position)
{
	char program_end[12] = { 0x48, 0xc7, 0xc0, 0x3c, 0, 0, 0, 0x48, 0x31, 0xff, 0x0f, 0x05 };   //12 bytes

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
		x86_commands[cur_position] = push_small_num;                                           //2 bytes
		                                                                                         //or
		x86_commands[cur_position + 1] = number_chars[0];

		cur_position += 2;
	}

	else
	{
		x86_commands[cur_position] = push_big_num;

		++cur_position;                                                                         //5 bytes

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
		x86_commands[cur_position] = push_bx;                                     //1 byte
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

	case (RSI_CODE):
		x86_commands[cur_position] = push_rsi;
		++cur_position;
		break;

	case (RDI_CODE):
		x86_commands[cur_position] = push_rdi;
		++cur_position;
		break;

	case (R12_CODE):
		x86_commands[cur_position] =  push_r12[0];
		x86_commands[cur_position + 1] = push_r12[1]; 

		cur_position += 2;	
		break;                                                         //2 bytes

	case (R14_CODE):
		x86_commands[cur_position] = push_r14[0];
		x86_commands[cur_position + 1] = push_r14[1];

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
		x86_commands[cur_position] = pop_cx;                           //1 byte
		++cur_position;
		break;

	case (DX_CODE):
		x86_commands[cur_position] = pop_dx;
		++cur_position;
		break;

	case (RSI_CODE):
		x86_commands[cur_position] = pop_rsi;
		++cur_position;
		break;

	case (RDI_CODE):
		x86_commands[cur_position] = pop_rdi;
		++cur_position;
		break;

	case (R13_CODE):
		x86_commands[cur_position] = pop_r13;
		++cur_position;
		break;

	case (R14_CODE):
		x86_commands[cur_position] = pop_r14;
		++cur_position;
		break;
	}

	return 0;
}

int add_sub_func(char* x86_commands, int& cur_position, int operation)
{
	pop_to_reg(x86_commands, cur_position, RSI_CODE);

	pop_to_reg(x86_commands, cur_position, RDI_CODE);

	if (operation == CMD_ADD)
	{
		for (int i = 0; i < 3; ++i)
			x86_commands[cur_position + i] = add_rdi_rsi[i];                         //6 bytes
	}

	else if (operation == CMD_SUB)
	{
		for (int i = 0; i < 3; ++i)
			x86_commands[cur_position + i] = sub_rdi_rsi[i];
	}

	cur_position += 3;

	push_reg(x86_commands, cur_position, RDI_CODE);

	return 0;
}

int imul_func(char* x86_commands, int& cur_position)
{
	pop_to_reg(x86_commands, cur_position, RSI_CODE);

	pop_to_reg(x86_commands, cur_position, RDI_CODE);                  //7 bytes

	for (int i = 0; i < 4; ++i)
		x86_commands[cur_position + i] = imul_rdi_rsi[i];

	cur_position += 4;

	push_reg(x86_commands, cur_position, RDI_CODE);

	return 0;
}

int idiv_func(char* x86_commands, int& cur_position)
{
	for (int i = 0; i < 3; ++i)
		x86_commands[cur_position + i] = mov_rdi_rax[i];
	
	cur_position += 3;

	pop_to_reg(x86_commands, cur_position, RSI_CODE);

	pop_to_reg(x86_commands, cur_position, AX_CODE);                //12 bytes

	for (int i = 0; i < 3; ++i)
		x86_commands[cur_position + i] = imul_rsi[i];

	cur_position += 3;

	push_reg(x86_commands, cur_position, AX_CODE);
	
	for (int i = 0; i < 3; ++i)
		x86_commands[cur_position + i] = mov_rax_rdi[i];

	cur_position += 3;

	return 0;
}

int jmp_func(char* x86_commands, int& cur_position, int dest_command, int* abs_addresses, int cur_command, int jump_code)
{
	int abs_dest_address = abs_addresses[dest_command];

	if (jump_code == CMD_JMP)
	{
		x86_commands[cur_position] = jmp;

		++cur_position;
	}
	else if (jump_code == CMD_JA)
	{
		cmp_func(x86_commands, cur_position);

		x86_commands[cur_position] = ja[0];
		x86_commands[cur_position + 1] = ja[1];

		cur_position += 2;
	}
	else if (jump_code == CMD_JB)
	{
		cmp_func(x86_commands, cur_position);

		x86_commands[cur_position] = jb[0];
		x86_commands[cur_position + 1] = jb[1];

		cur_position += 2;
	}
	else if (jump_code == CMD_JE)
	{
		cmp_func(x86_commands, cur_position);

		x86_commands[cur_position] = je[0];
		x86_commands[cur_position + 1] = je[1];

		cur_position += 2;
	}

	int rel_address = abs_dest_address - abs_addresses[cur_command] - commands_size_in_bytes[jump_code];

	const char* rel_address_to_chars = reinterpret_cast <const char*> (&rel_address);

	for (int i = 0; i < 4; ++i)
		x86_commands[cur_position + i] = rel_address_to_chars[i];

	cur_position += 4;
	
	return 0;
}

int cmp_func(char* x86_commands, int& cur_position)
{
	pop_to_reg(x86_commands, cur_position, RSI_CODE);
	pop_to_reg(x86_commands, cur_position, RDI_CODE);

	x86_commands[cur_position] = cmp_reg[0];
	x86_commands[cur_position + 1] = cmp_reg[1];

	cur_position += 2;

	x86_commands[cur_position] = cmp_rsi_rdi;

	push_reg(x86_commands, cur_position, RDI_CODE);
	push_reg(x86_commands, cur_position, RSI_CODE);

	++cur_position;

	return 0;
}
