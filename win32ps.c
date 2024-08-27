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

/*
   Win32ps is a tiny module which utilizes PSAPI (Process Status Helper) to
   get information about global memory usage and process specific memory and
   cpu time utilization.
*/

#include "php.h"
#include "ext/standard/info.h"
#include "php_win32ps.h"


// PHP 8 arginfo
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_win32_ps_list_procs, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_ps_stat_proc, 0, 0, IS_ARRAY, 0)
ZEND_ARG_TYPE_INFO(0, process, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_win32_ps_stat_mem, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

/* {{{ win32ps_functions[] */
zend_function_entry win32ps_functions[] = {
	PHP_FE(win32_ps_list_procs,		arginfo_win32_ps_list_procs)
	PHP_FE(win32_ps_stat_proc,		arginfo_win32_ps_stat_proc)
	PHP_FE(win32_ps_stat_mem,		arginfo_win32_ps_stat_mem)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ win32ps_module_entry */
zend_module_entry win32ps_module_entry = {
	STANDARD_MODULE_HEADER,
	"win32ps",
	win32ps_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(win32ps),
	PHP_WIN32PS_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_WIN32PS
ZEND_GET_MODULE(win32ps)
#endif

#define php_win32ps_error(message, pid) php_error_docref(NULL, E_WARNING, "Process Status Error (%d) " message " (PID %d)", GetLastError(), pid)

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(win32ps)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Win32 Process Status Support", "enabled");
	php_info_print_table_row(2, "Extension Version", PHP_WIN32PS_VERSION);
	php_info_print_table_end();
}
/* }}} */

/* {{{ int php_win32ps_procinfo */
PHP_WIN32PS_API int php_win32ps_procinfo(int proc, zval *array, int error_flags)
{
	char f[MAX_PATH + 1] = {0};
	zval mem, tms;
	int l;
	HANDLE h;
	SYSTEMTIME nows;
	FILETIME creat, kern, user, tmp, now;
	unsigned __int64 creatx, kernx, userx, nowx;
	PROCESS_MEMORY_COUNTERS memory = {sizeof(PROCESS_MEMORY_COUNTERS)};
	
	if (proc <= 0) {
		/* no info for the IDLE process */
		if (error_flags & PHP_WIN32PS_PID) {
			php_win32ps_error("Invalid process id", proc);
		}
		return FAILURE;
	}
	
	if (!(h = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, 0, proc))) {
		/* we are not allowed to query this process */
		if (error_flags & PHP_WIN32PS_OPEN_HANDLE) {
			php_win32ps_error("Could not open process handle", proc);
		}
		return FAILURE;
	}
	
	if (proc == 8) {
		memcpy(f, "*SYSTEM*", l = sizeof("*SYSTEM*") - 1);
	} else {
		if (!(l = GetModuleFileNameEx(h, NULL, f, MAX_PATH))) {
			if (error_flags & PHP_WIN32PS_FILE_NAME) {
				php_win32ps_error("Could not determine executable path", proc);
			}
			CloseHandle(h);
			return FAILURE;
		}
	}
			
	if (!GetProcessMemoryInfo(h, &memory, sizeof(memory))) {
		if (error_flags & PHP_WIN32PS_MEM_INFO) {
			php_win32ps_error("Could not determine memory usage", proc);
		}
		CloseHandle(h);
		return FAILURE;
	}
	
	if (!GetProcessTimes(h, &creat, &tmp, &kern, &user)) {
		if (error_flags & PHP_WIN32PS_PROC_TIMES) {
			php_win32ps_error("Could not determine process times", proc);
		}
		CloseHandle(h);
		return FAILURE;
	}
	CloseHandle(h);
	
	add_assoc_long(array, "pid", (long) proc);
	
	/* sanitize odd paths */
	if (!strncmp(f, "\\??\\", 3)) {
		add_assoc_stringl(array, "exe", &f[4], l - 4);
	} else if (!strncasecmp(f, "\\SystemRoot\\", sizeof("\\SystemRoot\\") - 1)) {
		char *p, *r;
		
		if ((r = getenv("SystemRoot")) || (r = getenv("SYSTEMROOT"))) {
			l = (int) spprintf(&p, 0, "%s%s", r, &f[sizeof("\\SystemRoot")-1]);
			add_assoc_stringl(array, "exe", p, l);
		} else {
			add_assoc_stringl(array, "exe", f, l);
		}
	} else {
		add_assoc_stringl(array, "exe", f, l);
	}
	
	//MAKE_STD_ZVAL(mem);
	array_init(&mem);
	add_assoc_zval(array, "mem", &mem);
	/* Number of page faults. */
	add_assoc_long(&mem, "page_fault_count", (long) memory.PageFaultCount);
	/* Peak working set size, in bytes. */
	add_assoc_long(&mem, "peak_working_set_size", (long) memory.PeakWorkingSetSize);
	/* Current working set size, in bytes. */
	add_assoc_long(&mem, "working_set_size", (long) memory.WorkingSetSize);
	/* Peak paged pool usage, in bytes. */
	add_assoc_long(&mem, "quota_peak_paged_pool_usage", (long) memory.QuotaPeakPagedPoolUsage);
	/* Current paged pool usage, in bytes. */
	add_assoc_long(&mem, "quota_paged_pool_usage", (long) memory.QuotaPagedPoolUsage);
	/* Peak nonpaged pool usage, in bytes. */
	add_assoc_long(&mem, "quota_peak_non_paged_pool_usage", (long) memory.QuotaPeakNonPagedPoolUsage);
	/* Current nonpaged pool usage, in bytes. */
	add_assoc_long(&mem, "quota_non_paged_pool_usage", (long) memory.QuotaNonPagedPoolUsage);
	/* Current space allocated for the pagefile, in bytes. Those pages may or may not be in memory. */
	add_assoc_long(&mem, "pagefile_usage", (long) memory.PagefileUsage);
	/* Peak space allocated for the pagefile, in bytes. */
	add_assoc_long(&mem, "peak_pagefile_usage", (long) memory.PeakPagefileUsage);

    //MAKE_STD_ZVAL(tms);
	array_init(&tms);
	add_assoc_zval(array, "tms", &tms);
	
	/* Current time, 100's of nsec */
	GetSystemTime(&nows);
	SystemTimeToFileTime(&nows, &now);
	memcpy(&nowx, &now, sizeof(unsigned __int64));
	
	/* Creation time, in seconds with msec precision */
	memcpy(&creatx, &creat, sizeof(unsigned __int64));
	add_assoc_double(&tms, "created", (double) ((__int64) ((nowx-creatx) / 10000Ui64)) / 1000.0);
	/* Kernel time, in seconds with msec precision */
	memcpy(&kernx, &kern, sizeof(unsigned __int64));
	add_assoc_double(&tms, "kernel", (double) ((__int64) (kernx / 10000Ui64)) / 1000.0);
	/* User time, in seconds with msec precision */
	memcpy(&userx, &user, sizeof(unsigned __int64));
	add_assoc_double(&tms, "user", (double) ((__int64) (userx / 10000Ui64)) / 1000.0);
	
	return SUCCESS;
}
/* }}} */

/* {{{ php_win32ps_meminfo */
PHP_WIN32PS_API void php_win32ps_meminfo(zval *array)
{
	MEMORYSTATUSEX memory = {sizeof(MEMORYSTATUSEX)};
	
	GlobalMemoryStatusEx(&memory);
	
	/* usage of physical memory in percent */
	add_assoc_long(array, "load", (long) memory.dwMemoryLoad);
	/* add 1024 bytes as unit, as PHP lacks i64 support */
	add_assoc_long(array, "unit", 1024L);
	/* Total size of physical memory, in kbytes. */
	add_assoc_long(array, "total_phys", (long) (memory.ullTotalPhys / 1024Ui64));
	/* Size of physical memory available, in kbytes. */
	add_assoc_long(array, "avail_phys", (long) (memory.ullAvailPhys / 1024Ui64));
	/* Size of the committed memory limit, in kbytes. */
	add_assoc_long(array, "total_pagefile", (long) (memory.ullTotalPageFile / 1024Ui64));
	/* Size of available memory to commit, in kbytes. */
	add_assoc_long(array, "avail_pagefile", (long) (memory.ullAvailPageFile / 1024Ui64));
	/* Total size of the user mode portion of the virtual address space of the calling process, in kbytes. */
	add_assoc_long(array, "total_virtual", (long) (memory.ullTotalVirtual / 1024Ui64));
	/* Size of unreserved and uncommitted memory in the user mode portion of the virtual address space of the calling process, in kbytes. */
	add_assoc_long(array, "avail_virtual", (long) (memory.ullAvailVirtual / 1024Ui64));
}
/* }}} */

/* {{{ proto array win32_ps_list_procs()
	List running processes */
PHP_FUNCTION(win32_ps_list_procs)
{
	int i, processes[PHP_WIN32PS_MAXPROC], proc_count = 0;
	
	if (ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}
	
	if (!EnumProcesses(processes, sizeof(processes), &proc_count)) {
		php_win32ps_error("Could not enumerate running processes", 0);
		RETURN_FALSE;
	}
	
	array_init(return_value);
	for (i = 0; i < proc_count/sizeof(int); ++i) {
		zval entry;
		
		array_init(&entry);
		if (SUCCESS == php_win32ps_procinfo(processes[i], &entry, PHP_WIN32PS_HANDLE_OPS)) {
			add_next_index_zval(return_value, &entry);
		} else {
			zval_ptr_dtor(&entry);
		}
	}
}
/* }}} */

/* {{{ proto array win32_ps_stat_proc([int pid])
	Get process info of process with pid or the current process */
PHP_FUNCTION(win32_ps_stat_proc)
{
	zend_long process = 0;
	
	//if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &process))
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(process)
	ZEND_PARSE_PARAMETERS_END();
	
	if (process <= 0) {
		process = (long) GetCurrentProcessId();
	}
	
	array_init(return_value);
	php_win32ps_procinfo((int) process, return_value, PHP_WIN32PS_ALL);
}
/* }}} */

/* {{{ proto array win32_ps_stat_mem()
	Get memory info */
PHP_FUNCTION(win32_ps_stat_mem)
{
	if (ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}
	
	array_init(return_value);
	php_win32ps_meminfo(return_value);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */