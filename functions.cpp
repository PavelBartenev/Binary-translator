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

		write_instruction(x86_commands, number_chars, cur_position, 4);
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
		write_instruction(x86_commands, add_rdi_rsi, cur_position, 3);

	else if (operation == CMD_SUB)
		write_instruction(x86_commands, sub_rdi_rsi, cur_position, 3);

	push_reg(x86_commands, cur_position, RDI_CODE);

	return 0;
}

int imul_func(char* x86_commands, int& cur_position)
{
	pop_to_reg(x86_commands, cur_position, RSI_CODE);

	pop_to_reg(x86_commands, cur_position, RDI_CODE);                  //7 bytes

	write_instruction(x86_commands, imul_rdi_rsi, cur_position, 4);

	push_reg(x86_commands, cur_position, RDI_CODE);

	return 0;
}

int idiv_func(char* x86_commands, int& cur_position)
{

	write_instruction(x86_commands, imul_rdi_rax, cur_position, 3);

	pop_to_reg(x86_commands, cur_position, RSI_CODE);

	pop_to_reg(x86_commands, cur_position, AX_CODE);                //12 bytes

	write_instruction(x86_commands, imul_rsi, cur_position, 3);

	push_reg(x86_commands, cur_position, AX_CODE);
	
	write_instruction(x86_commands, mov_rax_rdi, cur_position, 3);

	return 0;
}

int jmp_func(char* x86_commands, int& cur_position, int dest_command_index, int* abs_addresses, int cur_command_index, int jump_code)
{
	int abs_dest_address = abs_addresses[dest_command_index];

	if (jump_code == CMD_JMP)
		write_instruction(x86_commands, jmp, cur_position, 1);
	
	else if (jump_code == CMD_JA)
		write_instruction(x86_commands, ja, cur_position, 2);
	
	else if (jump_code == CMD_JB)
		write_instruction(x86_commands, jb, cur_position, 2);
	
	else if (jump_code == CMD_JE)
		write_instruction(x86_commands, je, cur_position, 2);

	int rel_address = abs_dest_address - abs_addresses[cur_command_index] - commands_size_in_bytes[jump_code];

	const char* rel_address_to_chars = reinterpret_cast <const char*> (&rel_address);

	write_instruction(x86_commands, rel_address_to_chars, cur_position, 4);
	
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
