#include <iostream>
#include <fstream>

#include "Commands.cpp"
#include "MakingELF.cpp"
#include "Command_codes.cpp"
#include "functions.cpp"

const int ret_addresses_buffer = 0x400500;

int commands_switch(int code_size, int* source_code, char* x86_commands, int* abs_addresses);

int fill_abs_addresses(int* source_code, int* abs_addresses, int code_size);

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
	int address = 0x400078 + 8;

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

	write_instruction(x86_commands, lea_r8_rsp_minus_200, cur_position, 8);

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
			/*write_instruction(x86_commands, call_num, cur_position, 1, abs_addresses[source_code[cur_command + 1]] - abs_addresses[cur_command + 2]);
			++cur_command;*/

			write_instruction(x86_commands, mov_qword_r8_num, cur_position, 3, abs_addresses[cur_command + 2]);
			write_instruction(x86_commands, sub_r8_num, cur_position, 3, 8);
			jmp_func(x86_commands, cur_position, source_code[cur_command + 1], abs_addresses, cur_command, CMD_CALL);
			
			++cur_command;
			break;

		case(CMD_RET):
			//write_instruction(x86_commands, ret, cur_position, 1);

			write_instruction(x86_commands, add_r8_num, cur_position, 3, 8);
			write_instruction(x86_commands, mov_rdi_qword_r8, cur_position, 3);
			write_instruction(x86_commands, jmp_rdi, cur_position, 2);

			break;
		}
	}

	return cur_position;
}


