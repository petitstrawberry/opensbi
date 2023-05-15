// SPDX-License-Identifier: BSD-2-Clause
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_system.h>

static int sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  const struct sbi_trap_regs *regs,
				  unsigned long *out_val,
				  struct sbi_trap_info *out_trap)
{
	int ret = SBI_ENOTSUPP;

	if (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	if (ret >= 0) {
		*out_val = ret;
		ret = 0;
	}

	return ret;
}

static int sbi_ecall_susp_probe(unsigned long extid, unsigned long *out_val)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	for (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		if (sbi_system_suspend_supported(type)) {
			*out_val = 1;
			return 0;
		}
	}

	*out_val = 0;
	return 0;
}

struct sbi_ecall_extension ecall_susp;

static int sbi_ecall_susp_register_extensions(void)
{
	unsigned long out_val;

	sbi_ecall_susp_probe(SBI_EXT_SUSP, &out_val);
	if (!out_val)
		return 0;

	return sbi_ecall_register_extension(&ecall_susp);
}

struct sbi_ecall_extension ecall_susp = {
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.probe			= sbi_ecall_susp_probe,
	.handle			= sbi_ecall_susp_handler,
};
