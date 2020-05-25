#include <iostream>
#include <fstream>

int make_elf_header(std::ofstream& elf);

int make_programm_header(std::ofstream& elf, int programm_size);

const int TOTAL_HEADERS_SIZE = 120;

int make_elf_header(std::ofstream& elf)
{
	char file_signature[4] = { 0x7f, 0x45, 0x4c, 0x46 };

	char EI_CLASS          = 2;

	char EI_DATA           = 1;

	char EI_VERSION        = 1;

	char EI_OSABI          = 0;

	char EI_ABIVERSION     = 0;

	char EI_PAD[7]         = { 0, 0, 0, 0, 0, 0, 0 };

	char e_type[2]         = { 2, 0 };

	char e_machine[2]      = { 0x3e, 0 };

	char e_version[4]      = { 1, 0, 0, 0 };

	char e_entry[8]        = { 0x78, 0, 0x40, 0, 0, 0, 0, 0 };

	char e_phoff[8]        = { 0x40, 0, 0, 0, 0, 0, 0, 0};

	char e_shoff[8]        = { 0, 0, 0, 0, 0, 0, 0, 0 };

	char e_flags[4]        = { 0, 0, 0, 0 };

	char e_ehsize[2]       = { 0x40, 0 };

	char e_phentsize[2]    = { 0x38, 0 };

	char e_phnum[2]        = { 1, 0 };

	char e_shentsize[2]    = { 0x40, 0 };

	char e_shnum[2]        = { 0, 0 };

	char e_shstrndx[2]     = { 0, 0 };

	elf.write (file_signature, sizeof file_signature);
	elf.write (&EI_CLASS,      sizeof EI_CLASS);
	elf.write (&EI_DATA,       sizeof EI_DATA);
	elf.write (&EI_VERSION,    sizeof EI_VERSION);
	elf.write (&EI_OSABI,      sizeof EI_OSABI);
	elf.write (&EI_ABIVERSION, sizeof EI_ABIVERSION);
	elf.write (EI_PAD,         sizeof EI_PAD);
	elf.write (e_type,         sizeof e_type);
	elf.write (e_machine,      sizeof e_machine);
	elf.write (e_version,      sizeof e_version);
	elf.write (e_entry,        sizeof e_entry);
	elf.write (e_phoff,        sizeof e_phoff);
	elf.write (e_shoff,        sizeof e_shoff);
	elf.write (e_flags,        sizeof e_flags);
	elf.write (e_ehsize,       sizeof e_ehsize);
	elf.write (e_phentsize,    sizeof e_phentsize);
	elf.write (e_phnum,        sizeof e_phnum);
	elf.write (e_shentsize,    sizeof e_shentsize);
	elf.write (e_shnum,        sizeof e_shnum);
	elf.write (e_shstrndx,     sizeof e_shstrndx);

	return 0;
}

int make_programm_header(std::ofstream& elf, int program_size)
{
	char p_type[4]      = { 1, 0, 0, 0 };

	char p_flags[4]     = { 5, 0, 0, 0 };

	char p_offset[8]    = { 0, 0, 0, 0, 0, 0, 0, 0 };

	char p_vadr[8]      = { 0, 0, 0x40, 0, 0, 0, 0, 0 };

	char p_paddr[8]     = { 0, 0, 0x40, 0, 0, 0, 0, 0 };

	long long p_filesz  = program_size;

	long long p_memsz   = program_size;

	char p_align[8]     = { 0, 0, 0x20, 0, 0, 0, 0, 0 };

	elf.write (p_type,                                    sizeof p_type);
	elf.write (p_flags,                                   sizeof p_flags);
	elf.write (p_offset,                                  sizeof p_offset);
	elf.write (p_vadr,                                    sizeof p_vadr);
	elf.write (p_paddr,                                   sizeof p_paddr);
	elf.write (reinterpret_cast<const char*> (&p_filesz), sizeof p_filesz);
	elf.write (reinterpret_cast<const char*> (&p_memsz),  sizeof p_memsz);
	elf.write (p_align,                                   sizeof p_align);

	return 0;
}


int test()
{
	std::ofstream elf("elf_executable.bin", std::ios::binary);

	make_elf_header(elf);
	make_programm_header(elf, 173);

	char hello_world[30] = { 0x48, 0xc7, 0xc0, 0x01, 0, 0, 0, 0x48, 0xc7, 0xc7, 0x01, 0, 0, 0, 0x48, 0xc7, 0xc6, 0xa2, 0x00, 0x40, 0, 0x48, 0xc7, 0xc2, 0x0b, 0, 0, 0, 0x0f, 0x05 };

	char program_end[12] = { 0x48, 0xc7, 0xc0, 0x3c, 0, 0, 0, 0x48, 0x31, 0xff, 0x0f, 0x05 };

	char text[11] = { 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd' };

	elf.write(hello_world, sizeof hello_world);
	elf.write(program_end, sizeof program_end);
	elf.write(text, sizeof text);

	return 0;
}
