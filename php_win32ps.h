/*
    +--------------------------------------------------------------------+
    | PECL :: win32ps                                                    |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted provided that the conditions mentioned |
    | in the accompanying LICENSE file are met.                          |
    +--------------------------------------------------------------------+
    | Copyright (c) 2005, Michael Wallner <mike@php.net>                 |
    +--------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_WIN32PS_H
#define PHP_WIN32PS_H

#include <windows.h>
#include <psapi.h>

#define PHP_WIN32PS_VERSION "1.0"
#define PHP_WIN32PS_MAXPROC 256

/* error flags */
#define PHP_WIN32PS_NOTHING			0
#define PHP_WIN32PS_PID				0x01
#define PHP_WIN32PS_OPEN_HANDLE		0x02
#define PHP_WIN32PS_FILE_NAME		0x04
#define PHP_WIN32PS_MEM_INFO		0x08
#define PHP_WIN32PS_PROC_TIMES		0x10

#define PHP_WIN32PS_HANDLE_OPS \
	PHP_WIN32PS_FILE_NAME|PHP_WIN32PS_MEM_INFO|PHP_WIN32PS_PROC_TIMES
#define PHP_WIN32PS_ALL \
	PHP_WIN32PS_PID|PHP_WIN32PS_OPEN_HANDLE|PHP_WIN32PS_HANDLE_OPS

extern zend_module_entry win32ps_module_entry;
#define phpext_win32ps_ptr &win32ps_module_entry

#ifdef PHP_WIN32
#	define PHP_WIN32PS_API __declspec(dllexport)
#else
#	define PHP_WIN32PS_API
#endif

#ifdef ZTS
#	include "TSRM.h"
#endif

PHP_MINFO_FUNCTION(win32ps);

PHP_WIN32PS_API int php_win32ps_procinfo(int proc, zval *array, int error_flags TSRMLS_DC);
PHP_WIN32PS_API void php_win32ps_meminfo(zval *array TSRMLS_DC);

PHP_FUNCTION(win32_ps_list_procs);
PHP_FUNCTION(win32_ps_stat_proc);
PHP_FUNCTION(win32_ps_stat_mem);

#endif	/* PHP_WIN32PS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
