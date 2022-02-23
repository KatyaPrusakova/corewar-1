#include "vm.h"

void	vm_execute_instruction(t_process *process,
t_arena *arena)
{
	t_size	new_pc;

	if (arena->verbosity & VM_VERBOSE_OPS)
		vm_print_instr(arena, process);
	g_instr_funcs[process->current_instruction.opcode - 1](arena, process);
	new_pc = (process->pc + vm_instr_size(&process->current_instruction))
		% MEM_SIZE;
	if (arena->verbosity & VM_VERBOSE_PC)
	{
		print(" => pc: %d (%#06x) => %d (%#06x) \n",
			process->pc, process->pc, new_pc, new_pc);
	}
	process->pc = new_pc;
}

void	vm_execute_process(t_process *process, t_arena *arena)
{
	if (process->cycles_before_execution == -1)
		vm_init_instruction_execution(process, arena);
	if (process->cycles_before_execution > 0)
		process->cycles_before_execution--;
	if (process->cycles_before_execution == 0)
	{
		vm_execute_instruction(process, arena);
		process->cycles_before_execution = (-1);
	}
}

void	vm_execute_cycle(t_process *process, t_arena *arena)
{
	if ((arena->verbosity & VM_VERBOSE_CYCLES) != 0)
		print("This is cycle\t%d\n", arena->current_cycle);
	while (process)
	{
		vm_execute_process(process, arena);
		process = process->next;
	}
	arena->current_cycle++;
}
