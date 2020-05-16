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

int jmp_func(char* x86_commands, int& cur_position, int dest_command_index, int* abs_addresses, int cur_command_index, int jump_code);

int cmp_func(char* x86_commands, int& cur_position);

int input_to_reg(char* x86_commands, int& cur_position, int reg_code);

int input_string(char* x86_commands, int& cur_position, int reg_code);

int write_instruction(char* x86_commands, char* instruction, int& cur_position, int size, int number);

int write_instruction(char* x86_commands, char* instruction, int& cur_position, int size);

int write_instruction(char* x86_commands, char* instruction, int& cur_position, int size, char number);

int output_reg(char* x86_commands, int& cur_position, int reg_code);

char ENTER = { 0x0a };

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

char push_ax[1] = { 0x50 };
char push_bx[1] = { 0x53 };
char push_cx[1] = { 0x51 };
char push_dx[1] = { 0x52 };
char push_rsi[1] = { 0x56 };
char push_rdi[1] = { 0x57 };

char push_r14[2] = { 0x41, 0x56 };

char pop_ax[1] = { 0x58 };
char pop_bx[1] = { 0x5b };
char pop_cx[1] = { 0x59 };
char pop_dx[1] = { 0x5a };
char pop_rsi[1] = { 0x5e };
char pop_rdi[1] = { 0x5f };

char pop_r13 = { 0x5d };
char pop_r14 = { 0x5e };

char push_r12[2] = { 0x41, 0x54 };

char mov_to_r12[3] = { 0x48, 0xc7, 0xc3 };

char mov_rdi_rax[3] = { 0x48, 0x89, 0xc7 };
char mov_rsi_rdx[3] = { 0x48, 0x89, 0xd6 };
char mov_rax_rdi[3] = { 0x48, 0x89, 0xf8 };
char mov_rcx_rsi[3] = { 0x48, 0x89, 0xf1 };
char mov_rsi_rcx[3] = { 0x48, 0x89, 0xce };
char mov_rbx_rax[3] = { 0x48, 0x89, 0xc3 };
char mov_rcx_rax[3] = { 0x48, 0x89, 0xc1 };
char mov_rdx_rax[3] = { 0x48, 0x89, 0xc2 };
char mov_rbx_num[3] = { 0x48, 0xc7, 0xc3 };
char mov_rdx_rsi[3] = { 0x48, 0x89, 0xf2 };
char mov_rax_num[3] = { 0x48, 0xc7, 0xc0 };
char mov_rdi_num[3] = { 0x48, 0xc7, 0xc7 };
char mov_rdx_rcx[3] = { 0x48, 0x89, 0xca };
char mov_rax_rax[3] = { 0x48, 0x89, 0xc0 };
char mov_rax_rbx[3] = { 0x48, 0x89, 0xc0 };
char mov_rax_rcx[3] = { 0x48, 0x89, 0xc8 };
char mov_rax_rdx[3] = { 0x48, 0x89, 0xd0 };
char mov_rax_rsi[3] = { 0x48, 0x89, 0xf0 };

char mov_byte_rsi_dl[2] = { 0x88, 0x16 };

char movzx_rdx_byte_rsi[3] = { 0x0f, 0xb6, 0x16 };

char mov_rdx_num[3] = { 0x48, 0xc7, 0xc2 };

char add_r14_r13[3] = { 0x4d, 0x01, 0xee };
char add_rdi_rsi[3] = { 0x48, 0x01, 0xf7 };
char add_rax_rdx[3] = { 0x48, 0x01, 0xd0 };
char add_rdx_num[3] = { 0x48, 0x81, 0xc2 };

char sub_rdi_rsi[3] = { 0x48, 0x29, 0xf7 };
char sub_rdx_num[3] = { 0x48, 0x83, 0xea };
char sub_rcx_rsi[3] = { 0x48, 0x29, 0xf1 };

char imul_rdi_rsi[4] = { 0x48, 0x0f, 0xaf, 0xfe };
char imul_rsi[3] = { 0x48, 0xf7, 0xfe };
char imul_rax_rax_num[3] = { 0x48, 0x6b ,0xc0 };

char idiv_rsi[3] = { 0x48, 0xf7, 0xfe };
char idiv_rbx[3] = { 0x48, 0xf7, 0xfb };

char jmp[1] = { 0xe9 };
char ja[2] = { 0x0f, 0x87 };
char jb[2] = { 0x0f, 0x82 };
char je[2] = { 0x0f, 0x84 };
char je_short[1] = { 0x74 };
char jmp_rdi[2] = { 0xff, 0xe7 };

char cmp_reg[2] = { 0x48, 0x39 };
char cmp_rdi_rsi[3] = { 0x48, 0x39, 0xf7 };
char cmp_rdx_num[3] = { 0x48, 0x83, 0xfa };
char cmp_byte_rcx_num[2] = { 0x80, 0x39 };
char cmp_byte_rsi_num[2] = { 0x80, 0x3e };
char cmp_rax_num[2] = { 0x48, 0x3d };

char syscall[2] = { 0x0f, 0x05 };

char lea_rsi_rsp_minus_160[8] = { 0x48, 0x8d, 0xb4, 0x24, 0x60, 0xff, 0xff, 0xff };
char lea_rcx_rsp_minus_160[8] = { 0x48, 0x8d, 0x8c, 0x24, 0x60, 0xff, 0xff, 0xff };


char xor_rax_rax[3] = { 0x48, 0x31, 0xc0 };
char xor_rdi_rdi[3] = { 0x48, 0x31, 0xff };
char xor_rdx_rdx[3] = { 0x48, 0x31, 0xd2 };

char inc_rcx[3] = { 0x48, 0xff, 0xc1 };
char inc_rsi[3] = { 0x48, 0xff, 0xc6 };

char dec_rsi[3] = { 0x48, 0xff, 0xce };

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

		else if ((CMD_JMP <= source_code[cur_command] && source_code[cur_command] <= CMD_JE) || source_code[cur_command] == CMD_CALL)
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

	for (int cur_command = 0; cur_command < code_size; ++cur_command)
	{
		switch (source_code[cur_command])
		{
		case CMD_END:
			end_func(x86_commands, cur_position);
			break;
			
		case CMD_PUSH:
			push_num(x86_commands, cur_position, source_code[cur_command + 1]);
			++cur_command;
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

		case (CMD_POP):
			write_instruction(x86_commands, pop_rdi, cur_position, 1);
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
			jmp_func(x86_commands, cur_position, source_code[cur_command + 1], abs_addresses, cur_command, CMD_JMP);
			++cur_command;
			break;

		case (CMD_JA):
			cmp_func(x86_commands, cur_position);
			jmp_func(x86_commands, cur_position, source_code[cur_command + 1], abs_addresses, cur_command, CMD_JA);
			++cur_command;
			break;

		case (CMD_JB):
			cmp_func(x86_commands, cur_position);
			jmp_func(x86_commands, cur_position, source_code[cur_command + 1], abs_addresses, cur_command, CMD_JB);
			++cur_command;
			break;

		case (CMD_JE):
			cmp_func(x86_commands, cur_position);
			jmp_func(x86_commands, cur_position, source_code[cur_command + 1], abs_addresses, cur_command, CMD_JE);
			++cur_command;
			break;

		case (CMD_IN_ax):
			input_to_reg(x86_commands, cur_position, AX_CODE);
			break;

		case (CMD_IN_bx):
			input_to_reg(x86_commands, cur_position, BX_CODE);
			break;

		case (CMD_IN_cx):
			input_to_reg(x86_commands, cur_position, CX_CODE);
			break;

		case (CMD_IN_dx):
			input_to_reg(x86_commands, cur_position, DX_CODE);
			break;

		case(CMD_OUT_ax):
			output_reg(x86_commands, cur_position, AX_CODE);
			break;

		case(CMD_OUT_bx):
			output_reg(x86_commands, cur_position, BX_CODE);
			break;

		case(CMD_OUT_cx):
			output_reg(x86_commands, cur_position, CX_CODE);
			break;

		case(CMD_OUT_dx):
			output_reg(x86_commands, cur_position, DX_CODE);
			break;

		case(CMD_OUT):
			write_instruction(x86_commands, pop_rsi, cur_position, 1);
			output_reg(x86_commands, cur_position, RSI_CODE);
			break;

		case(CMD_CALL):
			push_num(x86_commands, cur_position, abs_addresses[cur_command + 2]);
			jmp_func(x86_commands, cur_position, source_code[cur_command + 1], abs_addresses, cur_command, CMD_JMP);
			++cur_command;
			break;

		case(CMD_RET):
			write_instruction(x86_commands, pop_rdi, cur_position, 1);
			write_instruction(x86_commands, jmp_rdi, cur_position, 2);
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
		x86_commands[cur_position] = push_ax[0];
		++cur_position;
		break;
	
	case (BX_CODE):
		x86_commands[cur_position] = push_bx[0];                                     
		++cur_position;
		break;

	case (CX_CODE):
		x86_commands[cur_position] = push_cx[0];
		++cur_position;
		break;

	case (DX_CODE):
		x86_commands[cur_position] = push_dx[0];
		++cur_position;
		break;

	case (RSI_CODE):
		x86_commands[cur_position] = push_rsi[0];
		++cur_position;
		break;

	case (RDI_CODE):
		x86_commands[cur_position] = push_rdi[0];
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
		x86_commands[cur_position] = pop_ax[0];
		++cur_position;
		break;

	case (BX_CODE):
		x86_commands[cur_position] = pop_bx[0];
		++cur_position;
		break;

	case (CX_CODE):
		x86_commands[cur_position] = pop_cx[0];                           //1 byte
		++cur_position;
		break;

	case (DX_CODE):
		x86_commands[cur_position] = pop_dx[0];
		++cur_position;
		break;

	case (RSI_CODE):
		x86_commands[cur_position] = pop_rsi[0];
		++cur_position;
		break;

	case (RDI_CODE):
		x86_commands[cur_position] = pop_rdi[0];
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

int jmp_func(char* x86_commands, int& cur_position, int dest_command_index, int* abs_addresses, int cur_command_index, int jump_code)
{
	int abs_dest_address = abs_addresses[dest_command_index];

	if (jump_code == CMD_JMP)
	{
		x86_commands[cur_position] = jmp[0];

		++cur_position;
	}
	else if (jump_code == CMD_JA)
	{

		x86_commands[cur_position] = ja[0];
		x86_commands[cur_position + 1] = ja[1];

		cur_position += 2;
	}
	else if (jump_code == CMD_JB)
	{

		x86_commands[cur_position] = jb[0];
		x86_commands[cur_position + 1] = jb[1];

		cur_position += 2;
	}
	else if (jump_code == CMD_JE)
	{

		x86_commands[cur_position] = je[0];
		x86_commands[cur_position + 1] = je[1];

		cur_position += 2;
	}

	int rel_address = abs_dest_address - abs_addresses[cur_command_index] - commands_size_in_bytes[jump_code];

	const char* rel_address_to_chars = reinterpret_cast <const char*> (&rel_address);

	for (int i = 0; i < 4; ++i)
		x86_commands[cur_position + i] = rel_address_to_chars[i];

	cur_position += 4;
	
	return 0;
}

int cmp_func(char* x86_commands, int& cur_position)
{
	write_instruction(x86_commands, pop_rsi, cur_position, 1);
	write_instruction(x86_commands, pop_rdi, cur_position, 1);

	write_instruction(x86_commands, cmp_rdi_rsi, cur_position, 3);

	write_instruction(x86_commands, push_rdi, cur_position, 1);
	write_instruction(x86_commands, push_rsi, cur_position, 1);

	return 0;
}

int input_to_reg(char* x86_commands, int& cur_position, int reg_code)
{
	write_instruction(x86_commands, push_ax, cur_position, 1);

	write_instruction(x86_commands, push_cx, cur_position, 1);

	write_instruction(x86_commands, push_dx, cur_position, 1);

	input_string(x86_commands, cur_position, reg_code);

	write_instruction(x86_commands, xor_rax_rax, cur_position, 3);

	write_instruction(x86_commands, lea_rsi_rsp_minus_160, cur_position, 8);

	write_instruction(x86_commands, movzx_rdx_byte_rsi, cur_position, 3);

	write_instruction(x86_commands, cmp_rdx_num, cur_position, 3, ENTER);

	write_instruction(x86_commands, je_short, cur_position, 1, (char)(0x13));

	write_instruction(x86_commands, inc_rsi, cur_position, 3);

	write_instruction(x86_commands, sub_rdx_num, cur_position, 3, '0');

	write_instruction(x86_commands, imul_rax_rax_num, cur_position, 3, (char)(0x0a));

	write_instruction(x86_commands, add_rax_rdx, cur_position, 3);

	write_instruction(x86_commands, jmp, cur_position, 1, -28);

	switch (reg_code)
	{
	case(AX_CODE):
		write_instruction(x86_commands, mov_rdi_rax, cur_position, 3);
		write_instruction(x86_commands, pop_dx, cur_position, 1);
		write_instruction(x86_commands, pop_cx, cur_position, 1);
		write_instruction(x86_commands, pop_rdi, cur_position, 1);
		break;
	case(BX_CODE):
		write_instruction(x86_commands, mov_rbx_rax, cur_position, 3);
		write_instruction(x86_commands, pop_dx, cur_position, 1);
		write_instruction(x86_commands, pop_cx, cur_position, 1);
		write_instruction(x86_commands, pop_ax, cur_position, 1);
		break;
	case(CX_CODE):
		write_instruction(x86_commands, mov_rcx_rax, cur_position, 3);
		write_instruction(x86_commands, pop_dx, cur_position, 1);
		write_instruction(x86_commands, pop_ax, cur_position, 1);
		write_instruction(x86_commands, pop_ax, cur_position, 1);
		break;
	case(DX_CODE):
		write_instruction(x86_commands, mov_rdx_rax, cur_position, 3);
		write_instruction(x86_commands, pop_cx, cur_position, 1);
		write_instruction(x86_commands, pop_cx, cur_position, 1);
		write_instruction(x86_commands, pop_ax, cur_position, 1);
		break;
	}

	return 0;
}

int input_string(char* x86_commands, int& cur_position, int reg_code)
{
	write_instruction(x86_commands, lea_rsi_rsp_minus_160, cur_position, 8);

	write_instruction(x86_commands, xor_rax_rax, cur_position, 3);

	write_instruction(x86_commands, xor_rdi_rdi, cur_position, 3);

	write_instruction(x86_commands, mov_rdx_num, cur_position, 3, 1);

	write_instruction(x86_commands, syscall, cur_position, 2);

	write_instruction(x86_commands, cmp_byte_rsi_num, cur_position, 2, ENTER);

	write_instruction(x86_commands, je, cur_position, 2, 8);

	write_instruction(x86_commands, inc_rsi, cur_position, 3);

	write_instruction(x86_commands, jmp, cur_position, 1, -32);

	return 0;
}

int write_instruction(char* x86_commands, char* instruction, int& cur_position, int size, int number)
{
	for (int i = 0; i < size; ++i)
		x86_commands[cur_position + i] = instruction[i];

	cur_position += size;

	char* number_to_chars = reinterpret_cast<char*>(&number);

	for (int i = 0; i < 4; ++i)
		x86_commands[cur_position + i] = number_to_chars[i];

	cur_position += 4;

	return 0;
}

int write_instruction(char* x86_commands, char* instruction, int& cur_position, int size)
{
	for (int i = 0; i < size; ++i)
		x86_commands[cur_position + i] = instruction[i];

	cur_position += size;

	return 0;
}

int write_instruction(char* x86_commands, char* instruction, int& cur_position, int size, char number)
{
	for (int i = 0; i < size; ++i)
		x86_commands[cur_position + i] = instruction[i];

	cur_position += size;

	x86_commands[cur_position] = number;

	++cur_position;

	return 0;
}

int output_reg(char* x86_commands, int& cur_position, int reg_code)
{
	write_instruction(x86_commands, push_ax, cur_position, 1);
	write_instruction(x86_commands, push_bx, cur_position, 1);
	write_instruction(x86_commands, push_cx, cur_position, 1);
	write_instruction(x86_commands, push_dx, cur_position, 1);

	switch (reg_code) 
	{
	case(AX_CODE):
		write_instruction(x86_commands, mov_rax_rax, cur_position, 3);
		break;

	case(BX_CODE):
		write_instruction(x86_commands, mov_rax_rbx, cur_position, 3);
		break;

	case(CX_CODE):
		write_instruction(x86_commands, mov_rax_rcx, cur_position, 3);
		break;

	case(DX_CODE):
		write_instruction(x86_commands, mov_rax_rdx, cur_position, 3);
		break;

	case(RSI_CODE):
		write_instruction(x86_commands, mov_rax_rsi, cur_position, 3);
		break;
	}

	write_instruction(x86_commands, lea_rsi_rsp_minus_160, cur_position, 8);

	write_instruction(x86_commands, mov_rcx_rsi, cur_position, 3);

	write_instruction(x86_commands, mov_rbx_num, cur_position, 3, 10);

	write_instruction(x86_commands, xor_rdx_rdx, cur_position, 3);

	write_instruction(x86_commands, idiv_rbx, cur_position, 3);

	write_instruction(x86_commands, add_rdx_num, cur_position, 3, 0x30);

	write_instruction(x86_commands, cmp_rax_num, cur_position, 2, 0);

	write_instruction(x86_commands, je, cur_position, 2, 10);

	write_instruction(x86_commands, mov_byte_rsi_dl, cur_position, 2);

	write_instruction(x86_commands, dec_rsi, cur_position, 3);

	write_instruction(x86_commands, jmp, cur_position, 1, -34);

	write_instruction(x86_commands, mov_byte_rsi_dl, cur_position, 2);

	write_instruction(x86_commands, sub_rcx_rsi, cur_position, 3);

	write_instruction(x86_commands, inc_rcx, cur_position, 3);

	write_instruction(x86_commands, mov_rax_num, cur_position, 3, 1);

	write_instruction(x86_commands, mov_rdi_num, cur_position, 3, 1);

	write_instruction(x86_commands, mov_rdx_rcx, cur_position, 3);

	write_instruction(x86_commands, syscall, cur_position, 2);

	write_instruction(x86_commands, pop_dx, cur_position, 1);
	write_instruction(x86_commands, pop_cx, cur_position, 1);
	write_instruction(x86_commands, pop_bx, cur_position, 1);
	write_instruction(x86_commands, pop_ax, cur_position, 1);

	return 0;
}

