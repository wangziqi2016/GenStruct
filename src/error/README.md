
## Error

This directory contains error handling macro and some testing macros.

- SYSEXPECT() is called after a library or system call. It reports error stored in OS error number.
- error_exit() prints an error message, and then terminates the program. It accepts printf-style arguments
- dbg_printf() only prints under debug compilation mode.
- TEST_BEGIN() and TEST_PASS() macros are called before and after a test.
