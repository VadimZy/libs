#include "../../event_class/event_class.h"

#ifdef __NR_symlinkat
TEST(SyscallExit, symlinkatX)
{
	auto evt_test = new event_test(__NR_symlinkat, EXIT_EVENT);

	evt_test->enable_capture();

	/*=============================== TRIGGER SYSCALL  ===========================*/

	const char* target = "//**null-target**//";
	int32_t mock_dirfd = AT_FDCWD;
	const char* path = "//**null-path**//";
	assert_syscall_state(SYSCALL_FAILURE, "symlinkat", syscall(__NR_symlinkat, target, mock_dirfd, path));
	int64_t errno_value = -errno;

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

	/* Parameter 1: res (type: PT_ERRNO) */
	evt_test->assert_numeric_param(1, (int64_t)errno_value);

	/* Parameter 2: target (type: PT_CHARBUF) */
	evt_test->assert_charbuf_param(2, target);

	/* Parameter 3: linkdirfd (type: PT_FD) */
	evt_test->assert_numeric_param(3, (int64_t)PPM_AT_FDCWD);

	/* Parameter 4: linkpath (type: PT_FSPATH) */
	evt_test->assert_charbuf_param(4, path);

	/*=============================== ASSERT PARAMETERS  ===========================*/

	evt_test->assert_num_params_pushed(4);
}
#endif
