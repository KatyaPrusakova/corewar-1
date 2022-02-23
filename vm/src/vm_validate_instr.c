
// Checks that the instruction arguments are valid and
// fetches the next instruction if needed.

#include "vm.h"

t_instr  vm_validate_instr(t_process *process, t_arena *arena)
{
    t_int32 arg_value;

    vm_instr_get_param_value(&arg_value, arena, process, 0);
    arg_value *= -1;
    if (arg_value > arena->player_count || arg_value < 1)
    {
        process->pc = (process->pc + vm_instr_size(&process->current_instruction))
		% MEM_SIZE;
        vm_init_instruction_execution(process, arena);
    }
    return (process->current_instruction);
}
