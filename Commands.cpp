enum Commands
{
	CMD_END,
	CMD_PUSH,
	CMD_POP,
	CMD_ADD,
	CMD_SUB,
	CMD_MUL,
	CMD_DIV,
	CMD_SQRT,
	CMD_PUSH_ax,
	CMD_PUSH_bx,
	CMD_PUSH_cx,
	CMD_PUSH_dx,
	CMD_POP_ax,
	CMD_POP_bx,
	CMD_POP_cx,
	CMD_POP_dx,
	CMD_JMP,
	CMD_JA,
	CMD_JB,
	CMD_JE,
	CMD_IN_ax,
	CMD_IN_bx,
	CMD_IN_cx,
	CMD_IN_dx,
	CMD_OUT_ax,
	CMD_OUT_bx,
	CMD_OUT_cx,
	CMD_OUT_dx,
	CMD_OUT,
	CMD_FOUT,
	CMD_CALL,
	CMD_RET,
	CMD_PUSHR,
	CMD_POPR,
	CMD_SET_POINT_COLOR,
	CMD_SET_LINE_COLOR,
	CMD_SET_LINE_WIDTH,
	CMD_SET_POINT,
	CMD_SET_LINE,
	CMD_CREATE_WINDOW,
};


int commands_size_in_bytes[30] = {};

int fill_sizes()
{
	commands_size_in_bytes[CMD_END] = 12;

	commands_size_in_bytes[CMD_PUSH] = 2; //or 5

	commands_size_in_bytes[CMD_ADD] = 6;

	commands_size_in_bytes[CMD_SUB] = 6;

	commands_size_in_bytes[CMD_MUL] = 7;

	commands_size_in_bytes[CMD_DIV] = 12;

	commands_size_in_bytes[CMD_PUSH_ax] = 1;
	
	commands_size_in_bytes[CMD_PUSH_bx] = 1;

	commands_size_in_bytes[CMD_PUSH_cx] = 1;

	commands_size_in_bytes[CMD_PUSH_dx] = 1;

	commands_size_in_bytes[CMD_POP_ax] = 1;

	commands_size_in_bytes[CMD_POP_bx] = 1;

	commands_size_in_bytes[CMD_POP_cx] = 1;

	commands_size_in_bytes[CMD_POP_dx] = 1;

	commands_size_in_bytes[CMD_JMP] = 5;

	commands_size_in_bytes[CMD_JA] = 13;

	commands_size_in_bytes[CMD_JB] = 13;

	commands_size_in_bytes[CMD_JE] = 13;

	return 0;
}
