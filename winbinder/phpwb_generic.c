/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 General-purpose functions (not exported to the ZEND engine)

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"

//------------------------------------------------------------- GLOBAL VARIABLES

const char *pszWbobjName = "WinBinder Object";

//------------------------------------------------------------- PUBLIC FUNCTIONS

/* Accepts a limited subset of the parameters accepted by zend_parse_parameters() */
int parse_array(zval *array, const char *fmt, ...)
{
	int i, nelem;
	void *arg;
	zval *entry = NULL;
	va_list ap;
	HashTable *target_hash;

// #if (PHP_MAJOR_VERSION >= 5)
// 	TSRMLS_FETCH();
// #endif

	va_start(ap, fmt);

	target_hash = HASH_OF(array);
	if (!target_hash){
		return 0;
	}
	nelem = zend_hash_num_elements(target_hash);
	zend_hash_internal_pointer_reset(target_hash);

	// Parse loop
	for (i = 0; i < (int)strlen(fmt); i++)
	{

		arg = va_arg(ap, void *);
		if (!arg){
			break;
		}

		// Requested items past the length of the array must return NULL
		if (i >= nelem)
		{

			switch (fmt[i])
			{

			case 'l':
				*((long long *)arg) = 0;
				break;

			case 'd':
				*((double *)arg) = 0;
				break;

			case 's':
				*((long long *)arg) = (long long)NULL;
				break;

			default:
				wbError(TEXT("parse_array"), MB_ICONWARNING, TEXT("Invalid format character '%c' in function"), fmt[i]);
				continue;
			}
			continue;
		}
		else if ((entry = zend_hash_get_current_data(target_hash)) == NULL)
		{
			wbError(TEXT("parse_array"), MB_ICONWARNING, TEXT("Could not retrieve element %d from array in function"), i);
			continue;
		}
		else
		{

			if (!entry){
				break;
			}
			switch (fmt[i])
			{

			case 'l':
				if (Z_TYPE_P(entry) == IS_NULL)
				{
					*((long long *)arg) = (long long)NULL;
				}
				else{
					*((long long *)arg) = Z_LVAL_P(entry);
				}
				break;

			case 'd':
				if (Z_TYPE_P(entry) == IS_NULL)
				{
					*((long long *)arg) = (long long)NULL;
				}
				else{
					*((double *)arg) = Z_DVAL_P(entry);
				}
				break;

			case 's':
				if (Z_TYPE_P(entry) == IS_STRING)
				{
					*((long long *)arg) = (long long)(Z_STRVAL_P(entry));
				}
				else if (Z_TYPE_P(entry) == IS_NULL)
				{
					*((long long *)arg) = (long long)NULL;
				}
				else{
					*((long long *)arg) = (long long)NULL;
				}
				break;

			default:
				wbError(TEXT("parse_array"), MB_ICONWARNING, TEXT("Invalid format string '%s' in function"), fmt);
				continue;

			} // switch

		} // else

		if (i < nelem - 1){
			zend_hash_move_forward(target_hash);
		}
	}

	va_end(ap);
	return nelem;
}

/* Function to process arrays. Returns a pointer to a zval element from array zitems or
  NULL if end of array is reached */

zval *process_array(zval *zitems)
{
	static int nelem = 0;
	static int nelems = 0;
	static HashTable *target_hash = NULL;
	zval *entry = NULL;

	if (Z_TYPE_P(zitems) != IS_ARRAY){
		return FALSE;
	}
	// Prepare to read items from zitem array

	if (!nelems && !nelem)
	{

		target_hash = HASH_OF(zitems);
		if (!target_hash){
			return FALSE;
		}
		nelems = zend_hash_num_elements(target_hash);

		if (!nelems)
		{

			// Array is empty: reset everything

			target_hash = NULL;
			nelems = 0;
			nelem = 0;
			return NULL;
		}
		else
		{

			// Start array

			zend_hash_internal_pointer_reset(target_hash);
			nelem = 0;
		}
	}
	else
	{

		if (nelem < nelems - 1)
		{

			zend_hash_move_forward(target_hash);
			nelem++;
		}
		else if (nelem == nelems - 1)
		{

			// End of array: reset everything

			target_hash = NULL;
			nelems = 0;
			nelem = 0;
			return NULL;
		}
	}

	// Get zval data

	if ((entry = zend_hash_get_current_data(target_hash)) == NULL){
		wbError(TEXT("process_array"), MB_ICONWARNING, TEXT("Could not retrieve element %d from array in function"));
	}
	return entry;
}

TCHAR *Utf82WideChar(const char *str, int len)
{
	TCHAR *wstr;
	int wlen = 0;
	if (!str){
		return NULL;
	}
	if (len <= 0){
		len = strlen(str);
	}
	wlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	wstr = wbMalloc(sizeof(TCHAR) * (wlen));
	if (len > 0){
		MultiByteToWideChar(CP_UTF8, 0, str, len, wstr, wlen);
	}
	wstr[wlen - 1] = '\0';
	return wstr;
}

void Utf82WideCharCopy(const char *str, int str_len, TCHAR *wcs, int wcs_len)
{
	if (!str || !wcs){
		return;
	}
	MultiByteToWideChar(CP_UTF8, 0, str, str_len, wcs, wcs_len);
}

char *ConvertUTF16ToUTF8(LPCWSTR pszTextUTF16, int *plen)
{
	char *str = 0;

	if (pszTextUTF16 == NULL){
		return NULL;
	}
	int utf16len = wcslen(pszTextUTF16);
	int utf8len = WideCharToMultiByte(CP_UTF8, 0, pszTextUTF16, utf16len,
									  NULL, 0, NULL, NULL);

	if (utf8len == 0){
		return NULL;
	}
	str = wbMalloc(utf8len);
	int size = WideCharToMultiByte(CP_UTF8, 0, pszTextUTF16, utf16len, str, utf8len, 0, 0);
	if (plen)
	{
		if (size > 0){
			size--;
		}
		*plen = size;
	}
	return str;
}

char *WideChar2Utf8(LPCTSTR wcs, int *plen)
{
	char *str = NULL;
	int str_len = 0;

	if (!wcs){
		return NULL;
	}

    str_len = WideCharToMultiByte(CP_UTF8, 0, wcs, -1, NULL, 0, NULL, NULL);
    if (str_len == 0) {
        return NULL;
    }

	str = wbMalloc(str_len);
	int size = WideCharToMultiByte(CP_UTF8, 0, wcs, -1, str, str_len, NULL, NULL);
	str[str_len - 1] = '\0';
	if (plen)
	{
		if (size > 0){
			size--;
		}
		*plen = size;
	}

	return str;
}

void WideCharCopy(LPCTSTR wcs, char *s, int len)
{
	if (wcs && s){
		WideCharToMultiByte(CP_UTF8, 0, wcs, -1, s, len, NULL, NULL);
	}
}

void dumptcs(TCHAR *str)
{
	int i;
	int len = wcslen(str);

	printf("Dump String (%d): ", len);
	for (i = 0; i < len; i++)
	{
		printf("%x,", str[i]);
	}
	printf("\n");
}


void _var_dump(const char *var_name, ...) {
    va_list args;
    va_start(args, var_name);

    char format_specifier = *va_arg(args, char *);

    switch (format_specifier) {
        case 'i':
            printf("%s: int(%d)\n", var_name, va_arg(args, int));
            break;
        case 'f':
            printf("%s: float(%f)\n", var_name, va_arg(args, double));
            break;
        case 'd':
            printf("%s: double(%lf)\n", var_name, va_arg(args, double));
            break;
        case 'c':
            printf("%s: char(%c)\n", var_name, va_arg(args, int));
            break;
        case 'b':
            printf("%s: bool(%s)\n", var_name, va_arg(args, int) ? "true" : "false");
            break;
        default:
            printf("%s: unknown type\n", var_name);
            break;
    }

    va_end(args);
}


char *ConvertBSTRToLPSTR(BSTR bstrIn) {
  LPSTR pszOut = NULL;
  if (bstrIn != NULL)
  {
	int nInputStrLen = SysStringLen (bstrIn);
	
	// Double NULL Termination
	int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, bstrIn, nInputStrLen, NULL, 0, 0, 0) + 2; 
	pszOut = malloc(nOutputStrLen);

	if (pszOut)
	{
	  memset (pszOut, 0x00, sizeof (char)*nOutputStrLen);
	  WideCharToMultiByte (CP_ACP, 0, bstrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
	}
  }
  return pszOut;
}

// https://stackoverflow.com/questions/32424125/c-code-to-get-local-time-offset-in-minutes-relative-to-utc
int time_offset()
{
    time_t gmt, rawtime = time(NULL);
    struct tm *ptm;

#if !defined(WIN32)
    struct tm gbuf;
    ptm = gmtime_r(&rawtime, &gbuf);
#else
    ptm = gmtime(&rawtime);
#endif
    // Request that mktime() looksup dst in timezone database
    ptm->tm_isdst = -1;
    gmt = mktime(ptm);

    return (int)difftime(rawtime, gmt);
}


int get_system_timezone(char *tzchar)
{
	int to;
	to = time_offset();
	to = 0-(to/60/60);
	if(to < 0) sprintf(tzchar,"Etc/GMT%i\0",to);
	else sprintf(tzchar,"Etc/GMT+%i\0",to);
	return 1;
}

//------------------------------------------------------------------ END OF FILE
