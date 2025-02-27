#include "../../event_class/event_class.h"

#if defined(__NR_open) && defined(__NR_fstat)

#include <sys/stat.h> /* Definitions for `fstat` syscall. */

TEST(SyscallExit, openX_success)
{
	auto evt_test = new event_test(__NR_open, EXIT_EVENT);

	evt_test->enable_capture();

	/*=============================== TRIGGER SYSCALL  ===========================*/

	/* Syscall special notes:
	 * With `O_TMPFILE` flag the pathname must be a directory.
	 */
	const char* pathname = ".";
	int flags = O_RDWR | O_TMPFILE | O_DIRECTORY;
	mode_t mode = 0;
	int fd = syscall(__NR_open, pathname, flags, mode);
	assert_syscall_state(SYSCALL_SUCCESS, "open", fd, NOT_EQUAL, -1);

	/* Call `fstat` to retrieve the `dev` and `ino`. */
	struct stat file_stat;
	assert_syscall_state(SYSCALL_SUCCESS, "fstat", syscall(__NR_fstat, fd, &file_stat), NOT_EQUAL, -1);
	uint32_t dev = (uint32_t)file_stat.st_dev;
	uint64_t inode = file_stat.st_ino;
	close(fd);

	/*=============================== TRIGGER SYSCALL  ===========================*/

	evt_test->disable_capture();

	evt_test->assert_event_presence();

	if(HasFatalFailure())
	{
		return;
	}

	evt_test->parse_event();

	evt_test->assert_header();

	/*=============================== ASSERT PARAMETERS  ===========================*/

	/* Parameter 1: ret (type: PT_FD)*/
	evt_test->assert_numeric_param(1, (int64_t)fd);

	/* Parameter 2: name (type: PT_FSPATH) */
	evt_test->assert_charbuf_param(2, pathname);

	/* Parameter 3: flags (type: PT_FLAGS32) */
	evt_test->assert_numeric_param(3, (uint32_t)PPM_O_RDWR | PPM_O_TMPFILE | PPM_O_DIRECTORY);

	/* Parameter 4: mode (type: PT_UINT32) */
	/* mode is 0 so it must remain 0. */
	evt_test->assert_numeric_param(4, (uint32_t)mode);

	/* Parameter 5: dev (type: PT_UINT32) */
	evt_test->assert_numeric_param(5, (uint32_t)dev);

	/* Parameter 6: ino (type: PT_UINT64) */
	evt_test->assert_numeric_param(6, inode);

	/*=============================== ASSERT PARAMETERS  ===========================*/

	evt_test->assert_num_params_pushed(6);
}

TEST(SyscallExit, openX_failure)
{
	auto evt_test = new event_test(__NR_open, EXIT_EVENT);

	evt_test->enable_capture();

	/*=============================== TRIGGER SYSCALL  ===========================*/

	/* Syscall special notes:
	 * With `O_TMPFILE` flag the pathname must be a directory
	 * but here it is a filename so the call will fail!
	 */
	const char* pathname = "mock_path";
	int flags = O_RDWR | O_TMPFILE | O_DIRECTORY;
	mode_t mode = 0;
	assert_syscall_state(SYSCALL_FAILURE, "open", syscall(__NR_open, pathname, flags, mode));
	int64_t errno_value = -errno;

	/*=============================== TRIGGER SYSCALL  ===========================*/

	evt_test->disable_capture();

	evt_test->assert_event_presence();

	if(HasFatalFailure())
	{
		return;
	}

	evt_test->parse_event();

	evt_test->assert_header();

	/*=============================== ASSERT PARAMETERS  ===========================*/

	/* Parameter 1: ret (type: PT_FD)*/
	evt_test->assert_numeric_param(1, (int64_t)errno_value);

	/* Parameter 2: name (type: PT_FSPATH) */
	evt_test->assert_charbuf_param(2, pathname);

	/* Parameter 3: flags (type: PT_FLAGS32) */
	evt_test->assert_numeric_param(3, (uint32_t)PPM_O_RDWR | PPM_O_TMPFILE | PPM_O_DIRECTORY);

	/* Parameter 4: mode (type: PT_UINT32) */
	/* mode is 0 so it must remain 0. */
	evt_test->assert_numeric_param(4, (uint32_t)mode);

	/* Syscall fails so dev=0 && ino=0.*/

	/* Parameter 5: dev (type: PT_UINT32) */
	evt_test->assert_numeric_param(5, (uint32_t)0);

	/* Parameter 6: ino (type: PT_UINT64) */
	evt_test->assert_numeric_param(6, (uint64_t)0);

	/*=============================== ASSERT PARAMETERS  ===========================*/

	evt_test->assert_num_params_pushed(6);
}
#endif
