/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 Library of ZEND-specific functions for the WinBinder extension

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"
#include "win32/getrusage.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
//-------------------------------------------------------------------- CONSTANTS

#define CALLBACK_ARGS 6 // Number of arguments of the callback function

//----------------------------------------------------------- EXPORTED FUNCTIONS

// Chinese chars from phpwb_* files for some reason?????
BOOL wbError(LPCTSTR szFunction, int nType, LPCTSTR pszFmt, ...)
{
	TCHAR szString[MAX_ERR_MESSAGE];
	TCHAR szAux[MAX_ERR_MESSAGE];
	char str[MAX_ERR_MESSAGE];

	int messageType;

	TCHAR *szMsg = 0;
	TCHAR *szTitle = 0;

	// Build the string

	va_list args;
	va_start(args, pszFmt);
	vswprintf(szAux, MAX_ERR_MESSAGE, pszFmt, args);
	va_end(args);

	// Prepend the function name

	wcscpy(szString, szFunction);
	wcscat(szString, TEXT(": "));
	wcscat(szString, szAux);
	WideCharCopy(szString, str, MAX_ERR_MESSAGE);

	switch (nType)
	{
	case MB_OK:
	case MB_ICONINFORMATION:
		messageType = E_NOTICE;
		break;

	case MB_ICONWARNING:
		messageType = E_WARNING;
		break;

	case MB_ICONSTOP:
	case MB_ICONQUESTION:
	default:
		messageType = E_ERROR;
		break;
	}

	// Normal error with stack trace
	php_error_docref(NULL, messageType, str);


	/* if not debug mode show friendly error box (only for fatal errors)
	if (INI_INT("winbinder.debug_level") == 0 && messageType == E_ERROR)
	{
		szMsg = Utf82WideChar(str, 0);
		szTitle = Utf82WideChar("wbError", 0);
		wbMessageBox(NULL, szMsg, szTitle, nType);
	}
	*/
	return FALSE;
}

// *** The use of parameter pwboParent in wbCallUserFunction() is not clear

UINT64 wbCallUserFunction(LPCTSTR pszFunctionName, LPDWORD pszObject, PWBOBJ pwboParent, PWBOBJ pctrl, UINT64 id, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
	zval fname = {0};
	zval return_value = {0};
	zval parms[CALLBACK_ARGS];
	BOOL bRet;
	UINT64 ret = 0;
	char *pszFName;
	char *pszOName;
	zend_string *funName;
	int name_len = 0;

	if (pszObject == NULL)
	{
		name_len = 0;
	}

	// Is there a callback function assigned to the window?
//    printf("wbCallUserFunction Debug: functionName=%ls, object=%p, pwboParent=%p, pctrl=%p, id=%llu, lParam1=%lld, lParam2=%lld, lParam3=%lld\n",
//               pszFunctionName, pszObject, pwboParent, pctrl, id, lParam1, lParam2, lParam3);
	pszFName = WideChar2Utf8(pszFunctionName, &name_len);
	if (!pszFName || !*pszFName)
	{
		TCHAR szTitle[256];
		char title[256];
        efree(pszFName);  // Free memory allocated for pszFName
		if (GetWindowText(pwboParent->hwnd, szTitle, 256))
		{
			WideCharCopy(szTitle, title, 256);
			wbError(TEXT("wbCallUserFunction"), MB_ICONWARNING, TEXT("No callback function assigned to window '%s'"), title);
		}
		else
			wbError(TEXT("wbCallUserFunction"), MB_ICONWARNING, TEXT("No callback function assigned to window #%ld"), (LONG_PTR)pwboParent);
		return FALSE;
	}

	ZVAL_STRING(&fname, pszFName);

	/* why we test again ??? GYW
	// Error checking is VERY POOR for user methods (i.e. when pszObjectName is not NULL)
	if(pszObject == NULL && !zend_is_callable(&fname, 0, &funName)) {
		zend_error(E_WARNING, "%s(): '%s' is not a function or cannot be called",
		  get_active_function_name(TSRMLS_C), funName);
		if (funName) efree(funName);				// These two lines prevent a leakage
		return FALSE;
	}
	*/

	// In case of an object
	//if(pszObjectName && *pszObjectName) {
	//	ZVAL_STRING(&oname, pszObjectName);
	//}

	// PWBOBJ pointer
	ZVAL_LONG(&parms[0], (LONG_PTR)pwboParent);

	// id
	ZVAL_LONG(&parms[1], (LONG_PTR)id);

	// control handle
	ZVAL_LONG(&parms[2], (LONG_PTR)pctrl);

	// lparam1
	ZVAL_LONG(&parms[3], (LONG_PTR)lParam1);

	// lparam2
	ZVAL_LONG(&parms[4], (LONG_PTR)lParam2);

	// lparam3
	ZVAL_LONG(&parms[5], (LONG_PTR)lParam3);

	// Call the user function
	bRet = call_user_function(
		NULL, // CG(function_table) Hash value for the function table
		(zval *)&pszObject,			// Pointer to an object (may be NULL)
		&fname,				// Function name
		&return_value,		// Return value
		CALLBACK_ARGS,		// Parameter count
		parms				// Parameter array
		);

    // Check if its NOT FAILURE (NULL is okay as user functions may return void)
	if (bRet != SUCCESS)
	{
	    // supress if its null as the user function may not return anything
	    if(bRet != IS_NULL){
	        wbError(TEXT("wbCallUserFunction"),MB_ICONWARNING,TEXT("User function call failed %s"),(BOOL)bRet);
	    }
	}

    // Free allocated memory
    efree(pszFName);
    zval_ptr_dtor(&fname);

	switch (Z_TYPE(return_value))
	{
        case IS_LONG:
        case IS_TRUE:
        case IS_FALSE:
            ret = Z_LVAL(return_value);
            break;
	}

	zval_ptr_dtor(&return_value);

	return ret;
}

// Memory-allocation functions
void *wbMalloc(size_t size)
{
    void *ptr = emalloc(size);
    if (!ptr) {
    }
    return ptr;
}

void *wbCalloc(size_t nmemb, size_t size)
{
	return ecalloc(nmemb, size);
}

void *wbRealloc(void *ptr, size_t size)
{
	return erealloc(ptr, size);
}

char *wbStrDup(const char *string)
{
	return estrdup(string);
}

char *wbStrnDup(const char *string, size_t size)
{
	return estrndup(string, size);
}

BOOL wbFree(void *ptr)
{
	if (ptr){
		efree(ptr);
	}
	return TRUE;
}

UINT64 MemCheck(const char *message, BOOL mb){
	struct rusage r_usage;
	int *p = 0;
	p = (int*)malloc(sizeof(int)*1000);
	int ret = getrusage(RUSAGE_SELF,&r_usage);
	if(ret == 0){
		if(mb){
			printf("%s Memory: %lldMB\n", message, (r_usage.ru_maxrss/1024));
		} else {
			printf("%s Memory: %lldKB\n", message, r_usage.ru_maxrss);
		}
	} else {
		printf("MemCheck Error in getrusage. errno = %d\n", errno);
	}
	return 0;
}



//------------------------------------------------------------------ END OF FILE