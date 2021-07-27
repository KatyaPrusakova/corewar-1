#include "../inc/vm.h"

/*
** Reading the arguments passed to the VM. -n flag is followed by a player number that is
** given to the player named in the following argument. If -n is not specified,
*/

t_uint32	vm_reverse_bytes(t_uint32 src)
{
	t_uint32	dst;

	mcpy_safe((void *)&dst, (void *)&src + 3, sizeof(t_byte));
	mcpy_safe((void *)&dst + 1, (void *)&src + 2, sizeof(t_byte));
	mcpy_safe((void *)&dst + 2, (void *)&src + 1, sizeof(t_byte));
	mcpy_safe((void *)&dst + 3, (void *)&src, sizeof(t_byte));
	return (dst);
}

void	vm_error(const char *message)
{
	print("%s", message);
	exit(0);
}

void	vm_get_data_from_file(t_arena *arena, int player_number, int fd)
{
	t_player	*player;
	t_byte		buf[MEM_SIZE];

	player = &arena->all_players[player_number];
	read(fd, buf, sizeof(t_byte) * 4);
	mcpy_safe((void*)&player->header.magic, (void*)buf, sizeof(t_uint32));
	player->header.magic = vm_reverse_bytes(player->header.magic);
	print("magic number: %x\n", player->header.magic);
	read(fd, buf, sizeof(t_byte) * PROG_NAME_LENGTH);
	buf[PROG_NAME_LENGTH] = '\0';
	s_cpy(player->header.prog_name, (const char*)buf);
	print("prog_name: %s\n", player->header.prog_name);
	read(fd, buf, sizeof(t_byte) * 4);
	print("four NULL bytes: %d%d%d%d\n", buf[0], buf[1], buf[2], buf[3]);
	read(fd, buf, sizeof(t_byte) * 4);
	mcpy_safe((void*)&player->header.prog_size, (void*)buf, sizeof(t_uint32));
	player->header.prog_size = vm_reverse_bytes(player->header.prog_size);
	print("prog_size %d\n", player->header.prog_size);
}

void	vm_parse_bytecode(t_arena *arena, t_uint32 *player_number, char *name)
{
	int	fd;

	if (s_cmp(".cor", s_rchr(name, '.')))
		vm_error("Champions must be .cor files\n");
	fd = open(name, O_RDONLY);
	if (fd < 0)
		vm_error("Unable to open .cor file \n");
	vm_get_data_from_file(arena, *player_number, fd);
		
}

void	vm_create_player(t_arena *arena, t_uint32 *player_number, char *name)
{
	t_player	player;

	if (arena->all_players[*player_number].number)
		*player_number += 1;
	if (*player_number > MAX_PLAYERS)
		vm_error("player_number is not within MAX_PLAYERS\n");
	mzero(&player, sizeof(t_player));
	player.number = *player_number;
	arena->all_players[*player_number - 1] = player;
	vm_parse_bytecode(arena, player_number, name);
	*player_number += 1;
}


/*
**	Checking that the arguments given to the vm are valid. Saving the room
**	numbers to all_players[].number defined by the -n flag. -n and -dump
**	flags must be followed by a numeric argument.
*/

void	vm_read_input(t_arena *arena, int argc, char **argv)
{
	int i;
	int	set_number;

	i = 0;
	while(++i < argc)
	{
			if (argv[i][0] == '-' && i + 1 == argc)
				vm_error("No argument after -flag\n");
		if (!s_cmp(argv[i], "-n"))
		{
			set_number = s_toi(argv[++i]);
			if (set_number <= 0 || set_number > MAX_PLAYERS)
				vm_error("Invalid value after -n flag\n");
			if (arena->all_players[set_number - 1].number)
				vm_error("All champions must be given unique numbers\n");
			s_cpy(arena->all_players[set_number - 1].header.prog_name, argv[++i]);
			arena->all_players[set_number - 1].number = set_number;
		}
		else if (!s_cmp(argv[i], "-dump"))
		{
			arena->dump_nbr_cycles = s_toi(argv[++i]);
			if (!arena->dump_nbr_cycles)
				vm_error("Invalid value after -dump\n");
		}
	}
}

void	vm_init(t_arena *arena, t_uint32 argc, char **argv)
{
	t_uint32	i;
	t_uint32	player_number;

	i = 1;
	player_number = 1;
	vm_read_input(arena, argc, argv);
	while (i < argc)
	{
		if (!s_cmp(argv[i], "-n"))
			i += 3;
		else if (!s_cmp(argv[i], "-dump"))
			i += 2;
		else
			vm_create_player(arena, &player_number, argv[i++]);
	}
}

int main(int argc, char **argv)
{
	t_arena	arena;

	if (argc < 2)
	{
		print("usage: vm [player_count][.cor]\n");
		return (0);
	}
	mzero(&arena, sizeof(t_arena));
	vm_init(&arena, argc, argv);
	return (0);
}