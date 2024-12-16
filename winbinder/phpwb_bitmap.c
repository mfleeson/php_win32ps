/*******************************************************************************

WINBINDER - The native Windows binding for PHP for PHP

Copyright  Hypervisual - see LICENSE.TXT for details
Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

ZEND wrapper for bitmap functions

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include <math.h>
#include "phpwb.h"

//----------------------------------------------------------- EXPORTED FUNCTIONS

/**
* [ index is the index of the image on the file if filename is an icon library. Default is 0.]
*
* @return  [type]  [return description]
*/
ZEND_FUNCTION(wb_load_image)
{
	char *s;
	size_t s_len;
	zend_long index;
	zend_long param = 0;
	zend_bool index_isnull;
	zend_bool param_isnull;
	HANDLE hImage;
	TCHAR *wcs = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s|ll", &s, &s_len, &index, &param) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(s,s_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(index, index_isnull)
		Z_PARAM_LONG_OR_NULL(param, param_isnull)
	ZEND_PARSE_PARAMETERS_END();

	wcs = Utf82WideChar(s, s_len);
	hImage = wbLoadImage(wcs, index, param);
	wbFree(wcs);
	//wbFree(s); // dont free filenames, mangles them :)

	if (!hImage) {
		RETURN_NULL();
	} else {
		RETURN_LONG((LONG_PTR)hImage);
	}
}

ZEND_FUNCTION(wb_save_image)
{
	zend_long hbm;
	char *s;
	size_t s_len;

	TCHAR *wcs = 0;
	BOOL ret;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &hbm, &s, &s_len) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_LONG(hbm)
	Z_PARAM_STRING(s, s_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!hbm){
		RETURN_BOOL(FALSE);
	}

	wcs = Utf82WideChar(s, s_len);

	ret = wbSaveBitmap((HBITMAP)hbm, wcs);
	//wbFree(wcs);
	#//wbFree(s);

	RETURN_BOOL(ret);
}

// Zend function to wrap wbRotateBitmap
ZEND_FUNCTION(wb_rotate_image)
{
    zend_long hbm;
    zend_long angle;
    HANDLE ret;

    // Parse parameters
    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_LONG(hbm)
    Z_PARAM_LONG(angle)
    ZEND_PARSE_PARAMETERS_END();

    if (!hbm) {
        RETURN_NULL();
    }

    // Call the wbRotateBitmap function
    ret = wbRotateBitmap((HBITMAP)hbm, angle);

    // Return the rotated image handle or NULL
    if (ret != NULL) {
        RETURN_LONG((zend_long)ret);
    } else {
        RETURN_NULL();
    }
}

// Zend function to wrap wbResizeBitmap
ZEND_FUNCTION(wb_resize_image)
{
    HBITMAP hbm;
    zend_long newWidth, newHeight;
    HBITMAP ret;

    // Parse parameters
    ZEND_PARSE_PARAMETERS_START(3, 3)
    Z_PARAM_LONG(hbm)
    Z_PARAM_LONG(newWidth)
	Z_PARAM_LONG(newHeight) 
    ZEND_PARSE_PARAMETERS_END();

    if (!hbm) {
        RETURN_NULL();
    }

    // Call the wbResizeBitmap function
    ret = wbResizeBitmap((HBITMAP)hbm, newWidth, newHeight);

    // Return the rotated image handle or NULL
    if (ret != NULL) {
        RETURN_LONG((zend_long)ret);
    } else {
        RETURN_NULL();
    }
}


ZEND_FUNCTION(wb_create_image)
{
	zend_long w, h, bmi = 0, bits = 0;
	int nargs;
	nargs = ZEND_NUM_ARGS();
	zend_bool bmi_isnull, bits_isnull;

	// if (zend_parse_parameters(nargs TSRMLS_CC, "ll|ll", &w, &h, &bmi, &bits) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 4)
	Z_PARAM_LONG(w)
	Z_PARAM_LONG(h)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG_OR_NULL(bmi, bmi_isnull)
	Z_PARAM_LONG_OR_NULL(bits, bits_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (nargs == 3)
	{
		wbError(TEXT("wb_create_image"), MB_ICONWARNING, TEXT("Invalid parameter count passed to function"));
		RETURN_LONG(0);
	}

	RETURN_LONG((LONG_PTR)wbCreateBitmap(w, h, (BITMAPINFO *)bmi, (void *)bits));
}

ZEND_FUNCTION(wb_get_image_data)
{
	zend_long hbm;
	BYTE *lpBits = NULL;
	DWORDLONG size;
	zend_bool compress4to3 = FALSE;
	zend_bool compress4to3_isnull;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &hbm, &compress4to3) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_LONG(hbm)
	Z_PARAM_OPTIONAL // Everything after optional
	Z_PARAM_BOOL_OR_NULL(compress4to3, compress4to3_isnull)
	ZEND_PARSE_PARAMETERS_END();

	// lpBits long pointer to BYTE array
	size = wbGetBitmapBits((HBITMAP)hbm, &lpBits, compress4to3);

	if (!lpBits){
		RETURN_NULL();
	}
	// 2016_08_12 - Jared Allard: we don't need a TRUE to be passed anymore.
	RETVAL_STRINGL(lpBits, size);
	//wbFree(lpBits);
}

ZEND_FUNCTION(wb_create_mask)
{
	zend_long hbm, c;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &hbm, &c) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_LONG(hbm)
	Z_PARAM_LONG(c)
	ZEND_PARSE_PARAMETERS_END();


	if (!hbm){
		RETURN_NULL();
	}
	RETURN_LONG((LONG_PTR)wbCreateMask((HBITMAP)hbm, c));
}


ZEND_FUNCTION(wb_screenshot)
{
    char *filename = NULL;
    size_t filename_len;
    wchar_t* wstr = NULL;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING_OR_NULL(filename, filename_len)
    ZEND_PARSE_PARAMETERS_END();

    // If filename is not NULL, convert it to wide string
    if(filename != NULL) {
        int wchars_num = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
        wstr = malloc(wchars_num * sizeof(wchar_t));
        MultiByteToWideChar(CP_UTF8, 0, filename, -1, wstr, wchars_num);
    }

    // Pass filename (as wide string) or NULL to CaptureScreen
    RETURN_LONG((LONG_PTR)CaptureScreen(wstr));

    // If wstr was allocated, free it
    if(wstr != NULL) {
        free(wstr);
    }
}



ZEND_FUNCTION(wb_destroy_image)
{
	zend_long hbm;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &hbm) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(hbm)
	ZEND_PARSE_PARAMETERS_END();

	if (!hbm){
		RETURN_BOOL(FALSE);
	}
	RETURN_BOOL(wbDestroyBitmap((HBITMAP)hbm));
}

#ifdef __LCC__

// This is needed by PHP 5.1

int _finite(double x)
{
	return (fpclassify(x) >= 0);
}

#endif

//------------------------------------------------------------------ END OF FILE