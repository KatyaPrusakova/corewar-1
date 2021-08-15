#include "vm.h"

// 3 : { (T_REG), (T_REG | T_DIR | T_IND), (T_DIR | T_REG) }
// sti src, lhs, rhs
// Computes lhs + rhs and uses the result as an offset to address memory
// and store the value of the register src at that memory location.

void vm_instr_sti(
		t_arena *a,
		t_process *p)
{
	t_size		mem_i;
	t_uint8		acb;
	t_byte		*dst;
	t_byte		*src;
	t_uint64	lhs;
	t_uint64	rhs;

	lhs = 0;
	rhs = 0;

	// acb
	mem_i = (p->pc + 1) % MEM_SIZE;
	acb = a->mem[mem_i];
	if (vm_check_acb(acb, 0) != REG_CODE)
		vm_error("Error st arg1!\n");

	// arg 1
	mem_i = (mem_i + 1) % MEM_SIZE;
	src = (t_byte *)&p->registers[a->mem[mem_i] - 1];

	// arg 2
	mem_i = (mem_i + 1) % MEM_SIZE;
	if (vm_check_acb(acb, 1) == REG_CODE)
	{
		vm_reverse_bytes(&lhs, &p->registers[a->mem[mem_i]], REG_ADDR_SIZE);
		mem_i = (mem_i + REG_ADDR_SIZE) % MEM_SIZE;
	}
	else if (vm_check_acb(acb, 1) == IND_CODE)
	{
		vm_reverse_bytes(&lhs, &a->mem[a->mem[mem_i]], IND_ADDR_SIZE);
		mem_i = (mem_i + IND_ADDR_SIZE) % MEM_SIZE;
	}
	else if (vm_check_acb(acb, 1) == DIR_CODE)
	{
		vm_reverse_bytes(&lhs, &a->mem[mem_i], DIR_VAL_SIZE);
		mem_i = (mem_i + DIR_VAL_SIZE) % MEM_SIZE;
	}
	else
		vm_error("Error st arg2!\n");

	// arg 3
	if (vm_check_acb(acb, 2) == REG_CODE)
	{
		vm_reverse_bytes(&rhs, &p->registers[a->mem[mem_i]], REG_ADDR_SIZE);
		mem_i = (mem_i + REG_ADDR_SIZE) % MEM_SIZE;
	}
	else if (vm_check_acb(acb, 2) == DIR_CODE)
	{
		vm_reverse_bytes(&rhs, &a->mem[mem_i], DIR_VAL_SIZE);
		mem_i = (mem_i + DIR_VAL_SIZE) % MEM_SIZE;
	}
	else
		vm_error("Error st arg3!\n");

	// store at index
	// print("%d + %d = %d\n", lhs, rhs, lhs + rhs);
	dst = &a->mem[(lhs + rhs) % MEM_SIZE];
	vm_reverse_bytes(dst, src, REG_SIZE);
	p->pc = mem_i;
}
