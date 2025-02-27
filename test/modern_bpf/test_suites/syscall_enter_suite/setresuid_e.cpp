#include "../../event_class/event_class.h"

#ifdef __NR_setresuid
TEST(SyscallEnter, setresuidE)
{
	auto evt_test = new event_test(__NR_setresuid, ENTER_EVENT);

	evt_test->enable_capture();

	/*=============================== TRIGGER SYSCALL ===========================*/

	uid_t ruid = (uint32_t)-1;
	uid_t euid = (uint32_t)-1;
	uid_t suid = (uint32_t)-1;
	/* If one of the arguments equals -1, the corresponding value is not changed. */
	assert_syscall_state(SYSCALL_SUCCESS, "setresuid", syscall(__NR_setresuid, ruid, euid, suid), NOT_EQUAL, -1);

	/*=============================== TRIGGER SYSCALL ===========================*/

	evt_test->disable_capture();

	evt_test->assert_event_presence();

	if(HasFatalFailure())
	{
		return;
	}

	evt_test->parse_event();

	evt_test->assert_header();

	/*=============================== ASSERT PARAMETERS  ===========================*/

	/* Parameter 1: ruid (type: PT_GID) */
	evt_test->assert_numeric_param(1, (uint32_t)ruid);

	/* Parameter 2: euid (type: PT_GID) */
	evt_test->assert_numeric_param(2, (uint32_t)euid);

	/* Parameter 3: suid (type: PT_GID) */
	evt_test->assert_numeric_param(3, (uint32_t)suid);

	/*=============================== ASSERT PARAMETERS  ===========================*/

	evt_test->assert_num_params_pushed(3);
}
#endif
