/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 ZEND wrapper for treeview control

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"

//---------------------------------------------------------- EXTERNAL PROTOTYPES

extern UINT64 wbGetTreeViewItemLevel(PWBOBJ pwbo, HTREEITEM hItem);

//----------------------------------------------------------- EXPORTED FUNCTIONS

/* Returns the level of a node in a treeview */

ZEND_FUNCTION(wb_get_level)
{
	zend_long pwbo, item = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pwbo, &item) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(item)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	 }
	if (((PWBOBJ)pwbo)->uClass == TreeView)
	{
		if (item)
		{
			RETURN_LONG(wbGetTreeViewItemLevel((PWBOBJ)pwbo, (HTREEITEM)item));
		}
		else
		{
			RETURN_NULL();
		}
	}
	else
	{
		RETURN_NULL();
	}
}

ZEND_FUNCTION(wb_set_treeview_item_selected)
{
	zend_long pwbo, item;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pwbo, &item) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(item)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	 }
	RETURN_BOOL(wbSetTreeViewItemSelected((PWBOBJ)pwbo, (HTREEITEM)item));
}

ZEND_FUNCTION(wb_set_treeview_item_text)
{
	zend_long pwbo, item;
	char *s;
	size_t s_len;

	TCHAR *wcs = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lls", &pwbo, &item, &s, &s_len) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(item)
		Z_PARAM_STRING(s,s_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	 }
	wcs = Utf82WideChar(s, s_len);
	RETURN_BOOL(wbSetTreeViewItemText((PWBOBJ)pwbo, (HTREEITEM)item, wcs));
}

ZEND_FUNCTION(wb_get_treeview_item_text)
{
	zend_long pwbo, item = 0;
	TCHAR szItem[MAX_ITEM_STRING];

	char *str = 0;
	int str_len = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pwbo, &item) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(item)
	ZEND_PARSE_PARAMETERS_END();


	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	 }
	wbGetTreeViewItemText((PWBOBJ)pwbo, (HTREEITEM)item, szItem, MAX_ITEM_STRING - 1);
	str = WideChar2Utf8(szItem, &str_len);
	RETURN_STRINGL(str, str_len);
}

/*

Change the item value.
If zparam is NULL, does not change associated value (does nothing).

*/

ZEND_FUNCTION(wb_set_treeview_item_value)
{
	zend_long pwbo, item, lparam = 0;
	zval *zparam, zcopy;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llz!", &pwbo, &item, &zparam) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(item)
		Z_PARAM_ZVAL(zparam)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	 }
	switch (Z_TYPE_P(zparam))
	{

	case IS_NULL: // Do not change lparam
		lparam = 0;
		break;
	default:
		// ****** Possible leak: Should free the existing copy of zparam here, if any
		ZVAL_LONG(&zcopy, 0);
		zcopy = *zparam;
		break;
	}

	RETURN_BOOL(wbSetTreeViewItemValue((PWBOBJ)pwbo, (HTREEITEM)item, lparam));
}

/*

int wb_create_treeview_item( int wbobject, string caption [, mixed param [, int where [, int image [, int selectedimage [, insertiontype]]]]])

Create a new treeview item and returns the HTREEITEM handle to it.
If zparam is NULL, does not change associated value.

*/

ZEND_FUNCTION(wb_create_treeview_item)
{
	zend_long pwbo, where = 0, img1 = -1, img2 = -1, insertiontype = 0, lparam = 0;
	char *str;
	size_t str_len;
	zval *zparam;
	BOOL setlparam = FALSE;
	TCHAR *wcs = 0;
	LONG_PTR ret;
	zend_bool where_isnull ,img1_isnull, img2_isnull, insertiontype_isnull;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls|zllll", &pwbo, &str, &str_len, &zparam, &where, &img1, &img2, &insertiontype) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 7)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_STRING(str,str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_OR_NULL(zparam)
		Z_PARAM_LONG_OR_NULL(where, where_isnull)
		Z_PARAM_LONG_OR_NULL(img1, img1_isnull)
		Z_PARAM_LONG_OR_NULL(img2, img2_isnull)
		Z_PARAM_LONG_OR_NULL(insertiontype, insertiontype_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE))
		RETURN_NULL();
	/* wb_create_items value only integer|null or adress possible, otherwise we need to allocate memory !   GYW */
	switch (Z_TYPE_P(zparam))
	{

	case IS_NULL: // Do not change lparam
		lparam = 0;
		setlparam = FALSE;
		break;

	default:
		lparam = Z_LVAL_P(zparam);
		setlparam = TRUE;
		break;
	}

	wcs = Utf82WideChar(str, str_len);
	switch (insertiontype)
	{
	case 0: // 'where' is the level of insertion
	default:
		ret = (LONG_PTR)wbAddTreeViewItemLevel((PWBOBJ)pwbo, where, wcs, lparam, setlparam, img1, img2);
		break;
	case 1: // 'where' is the sibling of the new node
		ret = (LONG_PTR)wbAddTreeViewItemSibling((PWBOBJ)pwbo, (HTREEITEM)where, wcs, lparam, setlparam, img1, img2);
		break;
	case 2: // 'where' is the parent of the new node
		ret = (LONG_PTR)wbAddTreeViewItemChild((PWBOBJ)pwbo, (HTREEITEM)where, wcs, lparam, setlparam, img1, img2);
		break;
	}
	RETURN_LONG(ret);
}

//------------------------------------------------------------------ END OF FILE
