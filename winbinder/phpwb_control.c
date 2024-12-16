/*******************************************************************************

 WINBINDER - The native Windows binding for PHP for PHP

 Copyright  Hypervisual - see LICENSE.TXT for details
 Author: Rubem Pechansky (https://github.com/crispy-computing-machine/Winbinder)

 ZEND wrapper for window controls

*******************************************************************************/

//----------------------------------------------------------------- DEPENDENCIES

#include "phpwb.h"

//----------------------------------------------------------- PRIVATE PROTOTYPES

// External

extern BOOL DisplayHTMLPage(PWBOBJ pwbo, LPCTSTR pszWebPageName);
extern BOOL SetProxyForWebBrowser(PWBOBJ pwbo, const char* proxyAddress);


//----------------------------------------------------------- EXPORTED FUNCTIONS

/* Creates a window control, menu, toolbar, status bar or accelerator. */

ZEND_FUNCTION(wb_create_control)
{
	zend_long pwboparent;
	zend_long wbclass, x = WBC_CENTER, y = WBC_CENTER;
	zend_long w = CW_USEDEFAULT, h = CW_USEDEFAULT, id = 32767, style = 0, param = 0, ntab = 0;
	int nargs;
	zval *zcaption;
	char *caption = "";
	char *tooltip = "";

	TCHAR *wcsCaption = 0;
	TCHAR *wcsTooltip = 0;

	nargs = ZEND_NUM_ARGS();
	zend_bool x_isnull, y_isnull, w_isnull, h_isnull, id_isnull, style_isnull, param_isnull, ntab_isnull;

	//if (zend_parse_parameters(nargs TSRMLS_CC, "ll|zllllllll", &pwboparent, &wbclass, &zcaption, &x, &y, &w, &h, &id, &style, &param, &ntab) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 11)
		Z_PARAM_LONG(pwboparent)
		Z_PARAM_LONG(wbclass)
		Z_PARAM_OPTIONAL // Everything after optional
		Z_PARAM_ZVAL_OR_NULL(zcaption)
		Z_PARAM_LONG_OR_NULL(x, x_isnull)
		Z_PARAM_LONG_OR_NULL(y, y_isnull)
		Z_PARAM_LONG_OR_NULL(w, w_isnull)
		Z_PARAM_LONG_OR_NULL(h, h_isnull)
		Z_PARAM_LONG_OR_NULL(id, id_isnull)
		Z_PARAM_LONG_OR_NULL(style, style_isnull)
		Z_PARAM_LONG_OR_NULL(param, param_isnull)
		Z_PARAM_LONG_OR_NULL(ntab, ntab_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (nargs == 5)
	{
		w = x;
		h = y;
		x = WBC_CENTER;
		y = WBC_CENTER;
	}

	if (!wbIsWBObj((void *)pwboparent, TRUE)){
		RETURN_NULL();
	}
	// 2016_08_12 PHP 7 no longer has the same zval struct, let's not be complicated and use *macros*
	// switch(zcaption->type) {
	switch (Z_TYPE_P(zcaption))
	{

	case IS_ARRAY:
		parse_array(zcaption, "ss", &caption, &tooltip);
		wcsCaption = Utf82WideChar(caption, 0);
		wcsTooltip = Utf82WideChar(tooltip, 0);
		break;

	case IS_STRING:
		// 2016_08_12 - Jared Allard: why be so complicated when we can use the Z_STRVAL and Z_STRLEN macros?
		wcsCaption = Utf82WideChar(Z_STRVAL_P(zcaption), Z_STRLEN_P(zcaption));
		break;

	case IS_NULL:
		// 2011_11_24 - Stefan Loewe: line below commented out, because if it fails with Windows 7 32 bit, with PHP 5.4 RC1
		//*caption = '\0';
		break;
	}

	// Convert line breaks for the caption and tooltip
	RETURN_LONG((LONG_PTR)wbCreateControl((PWBOBJ)pwboparent, wbclass, wcsCaption, wcsTooltip, x, y, w, h, id, style, param, ntab));
}

ZEND_FUNCTION(wb_destroy_control)
{
	zend_long pwbo;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pwbo) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}else{
		RETURN_BOOL(wbDestroyControl((PWBOBJ)pwbo));
	}
}

ZEND_FUNCTION(wb_get_visible)
{
	zend_long pwbo;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pwbo) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	} else{
		RETURN_BOOL(wbGetVisible((PWBOBJ)pwbo));
	}
}

ZEND_FUNCTION(wb_set_visible)
{
	zend_long pwbo;
	zend_bool b;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pwbo, &b) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_BOOL(b)
	ZEND_PARSE_PARAMETERS_END();


	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}else{
		RETURN_BOOL(wbSetVisible((PWBOBJ)pwbo, b));
	}
}

ZEND_FUNCTION(wb_set_focus)
{
	zend_long pwbo;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pwbo) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(TRUE);
	}else{
		RETURN_BOOL(wbSetFocus((PWBOBJ)pwbo));
	}
}

/* Sets the state of a control item */

ZEND_FUNCTION(wb_set_state)
{
	zend_long pwbo, item;
	zend_bool state = TRUE;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &pwbo, &item, &state) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(item)
		Z_PARAM_BOOL(state)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}

	if (((PWBOBJ)pwbo)->uClass == TreeView)
	{ // Set expanded / collapsed state
		RETURN_BOOL(wbSetTreeViewItemState((PWBOBJ)pwbo, (HTREEITEM)item, state));
	}
	
	RETURN_BOOL(FALSE);
}

/* Gets the state of a control item */

ZEND_FUNCTION(wb_get_state)
{
	zend_long pwbo, item;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pwbo, &item) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(item)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}

	if (((PWBOBJ)pwbo)->uClass == TreeView)
	{ // Get expanded / collapsed state
		RETURN_BOOL(wbGetTreeViewItemState((PWBOBJ)pwbo, (HTREEITEM)item));
	}
	else {
		RETURN_NULL();
	}
}

/* Gets the parent of a control or control item */

ZEND_FUNCTION(wb_get_parent)
{
	zend_long pwbo, item = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"l|l", &pwbo, &item) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 2)
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
			RETURN_LONG((LONG_PTR)wbGetTreeViewItemParent((PWBOBJ)pwbo, (HTREEITEM)item));
		}
		else
		{
			RETURN_LONG((LONG_PTR)((PWBOBJ)pwbo)->parent);
		}
	}
	else
	{
		RETURN_LONG((LONG_PTR)((PWBOBJ)pwbo)->parent);
	}
}

ZEND_FUNCTION(wb_get_focus){
	
	RETURN_LONG((LONG_PTR)wbGetFocus());

}

ZEND_FUNCTION(wb_set_style)
{
	zend_long pwbo, style;
	zend_long value = TRUE;
	zend_bool value_isnull;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|l", &pwbo, &style, &value) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(style)
		Z_PARAM_OPTIONAL // Everything after optional
		Z_PARAM_LONG_OR_NULL(value, value_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	RETURN_BOOL(wbSetStyle((PWBOBJ)pwbo, style, value));
}

ZEND_FUNCTION(wb_get_class)
{
	zend_long pwbo;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, return;
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	}
	RETURN_LONG(((PWBOBJ)pwbo)->uClass);
}

ZEND_FUNCTION(wb_get_id)
{
	zend_long pwbo;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pwbo) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	}
	RETURN_LONG(((PWBOBJ)pwbo)->id);
}

ZEND_FUNCTION(wb_get_value)
{
	zend_long pwbo, item = -1, subitem = -1;
	zend_bool item_isnull, subitem_isnull;
	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|ll", &pwbo, &item, &subitem) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL // Everything after optional
		Z_PARAM_LONG_OR_NULL(item, item_isnull)
		Z_PARAM_LONG_OR_NULL(subitem, subitem_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	}
	((PWBOBJ)pwbo)->item = item;
	((PWBOBJ)pwbo)->subitem = subitem;

	if (((PWBOBJ)pwbo)->uClass == ListView)
	{ // ListView: returns checked items

		int i, nitems;
		int *items;

		// How many items are checked?

		nitems = wbGetListViewCheckedItems((PWBOBJ)pwbo, NULL);
		if (!nitems){ // No items checked
			RETURN_NULL();
		}
		// Call the function again, this time to fill up the item array

		items = emalloc(sizeof(int) * nitems);
		wbGetListViewCheckedItems((PWBOBJ)pwbo, items);

		// Build the return array

		array_init(return_value);
		for (i = 0; i < nitems; i++)
			add_next_index_long(return_value, items[i]);

		efree(items);
		/* wb_create_items value only integer|null or adress possible, otherwise we need to allocate memory !   GYW

	} else if(((PWBOBJ)pwbo)->uClass == TreeView) {	// TreeView: returns whatever zval is

		zval *zparam;

		zparam = (zval *)wbGetValue((PWBOBJ)pwbo);
		if(!zparam)
			RETURN_NULL();

		// 2016_08_12 - Jared Allard: Use macros.
		switch(Z_TYPE_P(zparam)) {
			case IS_LONG:
			// 2016_08_12 - Jared Allard: no longer IS_BOOL, check for IS_TRUE or IS_FALSE
			case IS_TRUE:
				RETURN_LONG(Z_LVAL_P(zparam));
				break;
			case IS_FALSE:
				RETURN_LONG(Z_LVAL_P(zparam));
				break;
			case IS_STRING:
				// 2016_08_12 - Jared Allard: use macros.
				RETURN_STRINGL(Z_STRVAL_P(zparam), Z_STRLEN_P(zparam));
				break;
			case IS_DOUBLE:
				RETURN_DOUBLE(Z_DVAL_P(zparam));
				break;
			default:
				RETURN_NULL();
		}
*/
	}
	else
	{

		RETURN_LONG(wbGetValue((PWBOBJ)pwbo));
	}
}

ZEND_FUNCTION(wb_get_selected)
{
	zend_long pwbo;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pwbo) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	}
	if (((PWBOBJ)pwbo)->uClass == TabControl)
	{ // TabControl

		RETURN_LONG(TabCtrl_GetCurSel(((PWBOBJ)pwbo)->hwnd));
	}
	else if (((PWBOBJ)pwbo)->uClass == ListView)
	{ // ListView: returns selected items

		int i, nitems;
		int *items;

		// How many items are selected?

		nitems = wbGetListViewSelectedItems((PWBOBJ)pwbo, NULL);
		if (!nitems){ // No items selected
			RETURN_NULL();
		}
		// Call the function again, this time to fill up the item array

		items = emalloc(sizeof(int) * nitems);
		wbGetListViewSelectedItems((PWBOBJ)pwbo, items);

		// Build the return array

		array_init(return_value);
		for (i = 0; i < nitems; i++)
			add_next_index_long(return_value, items[i]);

		efree(items);
	}
	else if (((PWBOBJ)pwbo)->uClass == TreeView)
	{ // TreeView: returns the handle of the selected node

		RETURN_LONG((LONG_PTR)wbGetTreeViewItemSelected((PWBOBJ)pwbo));
	}
	else{

		RETURN_LONG(wbGetSelected((PWBOBJ)pwbo));
	}
}

/*
  bool wb_set_image (int wbobject, mixed source [, int transparentcolor [, int index [, int param]]])
*/

ZEND_FUNCTION(wb_set_image)
{
	zend_long pwbo, trcolor = NOCOLOR, index = 0, param = 0;
	zval *source = NULL;
	HANDLE hImage = NULL;
	TCHAR *wcs = 0;
	zend_bool trcolor_isnull, index_isnull, param_isnull;
	
	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz!|lll", &pwbo, &source, &trcolor, &index, &param) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_ZVAL(source)
		Z_PARAM_OPTIONAL // Everything after optional
		Z_PARAM_LONG_OR_NULL(trcolor, trcolor_isnull)
		Z_PARAM_LONG_OR_NULL(index, index_isnull)
		Z_PARAM_LONG_OR_NULL(param, param_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	// Get the image handle from source

	zend_uchar sourcetype = Z_TYPE_P(source);

	if (!source)
	{

		hImage = NULL;
	}

	if (source)
	{
		if (sourcetype == IS_LONG)
		{

			hImage = (HANDLE)source->value.lval;
		}
		else if (sourcetype == IS_STRING)
		{

			// Here param is the icon size: set it to 1 for a small icon
			wcs = Utf82WideChar(Z_STRVAL_P(source), Z_STRLEN_P(source));
			hImage = wbLoadImage(wcs, MAX(0, index), param);
			//wbFree(wcs);

			if (!hImage)
			{
				wbError(TEXT("wb_set_image"), MB_ICONWARNING, TEXT("Invalid image file %s or image index %d"), Z_STRVAL_P(source), index);
				RETURN_BOOL(FALSE);
			}
		}
		else
		{
			wbError(TEXT("wb_set_image"), MB_ICONWARNING, TEXT("Invalid parameter type passed to function"));
			RETURN_BOOL(FALSE);
		}
	}

	//	printf("%d %d %d %d\n", pwbo, trcolor, param, hImage);

	// Here param is the number of images for the ImageList

	RETURN_BOOL(wbSetImage((PWBOBJ)pwbo, hImage, trcolor, param));
}

/*
	Selects an icon from an imagelist and sets a control item/subitem to it

	bool wb_set_item_image (int wbobject, mixed index [, int item [, int subitem]])
*/

ZEND_FUNCTION(wb_set_item_image)
{
	zend_long pwbo, item = 0, subitem = 0;
	zval *zindex = NULL;
	int nclass, index1 = 0, index2 = 0;
	zend_bool item_isnull, subitem_isnull;
	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz|ll", &pwbo, &zindex, &item, &subitem) == FAILURE)
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_ZVAL(zindex)
		Z_PARAM_OPTIONAL // Everything after optional
		Z_PARAM_LONG_OR_NULL(item, item_isnull)
		Z_PARAM_LONG_OR_NULL(subitem, subitem_isnull)
	ZEND_PARSE_PARAMETERS_END();

	nclass = ((PWBOBJ)pwbo)->uClass;

	zend_uchar zindextype = Z_TYPE_P(zindex);

	switch (nclass)
	{

	case ListView:

		index1 = Z_LVAL_P(zindex);

		if (zindextype != IS_NULL && zindextype != IS_LONG)
		{
			wbError(TEXT("wb_set_item_image"), MB_ICONWARNING, TEXT("Parameter 2 expected to be an integer or NULL in function"));
			RETURN_BOOL(FALSE);
		}
		else if (zindextype == IS_NULL || (zindextype == IS_LONG && Z_LVAL_P(zindex) < 0))
		{
			RETURN_BOOL(wbSetListViewItemImage((PWBOBJ)pwbo, item, subitem, -1));
		}
		else
		{
			RETURN_BOOL(wbSetListViewItemImage((PWBOBJ)pwbo, item, subitem, index1));
		}
		break;

	case TabControl:

		//https://stackoverflow.com/questions/20081032/tabctrl-getitem-macro-not-working-as-expected
		index1 = zindex->value.lval;

		RETURN_BOOL(wbSetTabControlItemImages((PWBOBJ)pwbo, item, index1));
		break;

	case TreeView:

		if (zindextype == IS_ARRAY){
			parse_array(zindex, "ll", &index1, &index2);
		}else{
			index1 = zindex->value.lval;
		}
		RETURN_BOOL(wbSetTreeViewItemImages((PWBOBJ)pwbo, (HTREEITEM)item, index1, index2));
		break;

	case ListBox:
		// Future implementation goes here
	case ComboBox:
		// Future implementation goes here
	default:
		wbError(TEXT("wb_set_item_image"), MB_ICONWARNING, TEXT("Function is not implemented for class #%d"), nclass);
	}
}

ZEND_FUNCTION(wb_get_control)
{
	zend_long id = 0;
	zend_long pwboparent;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &pwboparent, &id) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(pwboparent)
		Z_PARAM_LONG(id)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwboparent, TRUE)){
		RETURN_NULL();
	}else{
		RETURN_LONG((LONG_PTR)wbGetControl((PWBOBJ)pwboparent, id));
	}
}

ZEND_FUNCTION(wb_set_enabled)
{
	zend_long pwbo;
	zend_bool state;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pwbo, &state) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_BOOL(state)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}else{
		RETURN_BOOL(wbSetEnabled((PWBOBJ)pwbo, state));
	}
}

ZEND_FUNCTION(wb_get_enabled)
{
	zend_long pwbo;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"l", &pwbo) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}else{
		RETURN_BOOL(wbGetEnabled((PWBOBJ)pwbo));
	}
}

/* bool wb_refresh (int control [, bool now]) */

ZEND_FUNCTION(wb_refresh)
{
	zend_long pwbo;
	zend_bool now = TRUE;
	zend_long x = 0, y = 0, width = 0, height = 0;
	zend_bool now_isnull, x_isnull, y_isnull, width_isnull, height_isnull;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|lllll", &pwbo, &now, &x, &y, &width, &height) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 6)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL // Everything after optional
		Z_PARAM_BOOL_OR_NULL(now, now_isnull)
		Z_PARAM_LONG_OR_NULL(x, x_isnull)
		Z_PARAM_LONG_OR_NULL(y, y_isnull)
		Z_PARAM_LONG_OR_NULL(width, width_isnull)
		Z_PARAM_LONG_OR_NULL(height, height_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}else{
		RETURN_BOOL(wbRefreshControl((PWBOBJ)pwbo, x, y, width, height, now));
	}
}

ZEND_FUNCTION(wb_begin_refresh)
{
	zend_long pwbo;
	zend_long x = 0, y = 0, width = 0, height = 0, fps = 0;
	zend_bool now_isnull, x_isnull, y_isnull, width_isnull, height_isnull, fps_isnull;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|lllll", &pwbo, &now, &x, &y, &width, &height) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 6)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL // Everything after optional
		Z_PARAM_LONG_OR_NULL(fps, fps_isnull)
		Z_PARAM_LONG_OR_NULL(x, x_isnull)
		Z_PARAM_LONG_OR_NULL(y, y_isnull)
		Z_PARAM_LONG_OR_NULL(width, width_isnull)
		Z_PARAM_LONG_OR_NULL(height, height_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}else{
		RETURN_BOOL(wbRefreshControlFPS((PWBOBJ)pwbo, x, y, width, height, fps));
	}
}

ZEND_FUNCTION(wb_get_item_count)
{
	zend_long pwbo;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pwbo) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(pwbo)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	}else{
		RETURN_LONG(wbGetItemCount((PWBOBJ)pwbo));

	}
}

ZEND_FUNCTION(wb_delete_items)
{
	zend_long pwbo;
	zval *zitems = NULL;
	BOOL bRet = TRUE;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|z!", &pwbo, &zitems) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_OR_NULL(zitems)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	}

	if (!zitems){ 
		// Delete all items
		RETURN_LONG(wbDeleteItems((PWBOBJ)pwbo, TRUE));
	}

	switch (Z_TYPE_P(zitems))
	{

	case IS_NULL: // Delete all items
		RETURN_LONG(wbDeleteItems((PWBOBJ)pwbo, TRUE));
		break;

	case IS_LONG: // Delete an item
	case IS_TRUE:
	case IS_FALSE:
		((PWBOBJ)pwbo)->item = Z_LVAL_P(zitems);
		RETURN_LONG(wbDeleteItems((PWBOBJ)pwbo, FALSE));
		break;

	case IS_ARRAY:
	{
		zval *zitem;

		while ((zitem = process_array(zitems)) != NULL)
		{
			((PWBOBJ)pwbo)->item = Z_LVAL_P(zitem);
			if (!wbDeleteItems((PWBOBJ)pwbo, FALSE)){
				bRet = FALSE;
				}
		}
	}

		RETURN_BOOL(bRet);
	default:
		wbError(TEXT("wb_delete_items"), MB_ICONWARNING, TEXT("Parameter 2 expected to be an integer or array in function"));
		RETURN_NULL();
	}
}

ZEND_FUNCTION(wb_sort)
{
	zend_long pwbo, ascending = TRUE, subitem = 0;
	zend_bool ascending_isnull, subitem_isnull;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|ll", &pwbo, &ascending, &subitem) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL // Everything after optional
		Z_PARAM_LONG_OR_NULL(ascending, ascending_isnull)
		Z_PARAM_LONG_OR_NULL(subitem, subitem_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	} else
	{
		if (((PWBOBJ)pwbo)->uClass == ListView){
			RETURN_BOOL(wbSortLVColumn((PWBOBJ)pwbo, subitem, ascending));
		}else{
			RETURN_BOOL(FALSE);

		}
	}
}

ZEND_FUNCTION(wb_set_location)
{
	char *location;
	size_t location_len;
	zend_long pwbo;

	TCHAR *wcs = 0;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &pwbo, &location, &location_len) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_STRING(location,location_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	if (((PWBOBJ)pwbo)->uClass == HTMLControl)
	{
		wcs = Utf82WideChar(location, location_len);
		RETURN_BOOL(DisplayHTMLPage((PWBOBJ)pwbo, wcs));
	}
	else{
		RETURN_BOOL(FALSE);
	}
}

// Function to set the proxy for the web browser instance
PHP_FUNCTION(wb_set_browser_proxy) {

    char* proxyAddress = NULL;
    size_t proxyAddressLen;
	zend_long pwbo;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_STRING(proxyAddress, proxyAddressLen)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}

    RETURN_BOOL(SetProxyForWebBrowser((PWBOBJ)pwbo, proxyAddress));

}
//------------------------------------------------- AUXILIARY EXPORTED FUNCTIONS

ZEND_FUNCTION(wb_select_tab)
{
	zend_long pwbo;
	zend_long ntab;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pwbo, &ntab) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(ntab)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	RETURN_BOOL(wbSelectTab((PWBOBJ)pwbo, ntab));
}

ZEND_FUNCTION(wb_set_value)
{
	zend_long pwbo, value, item = 0, subitem = 0;
	zend_bool item_isnull, subitem_isnull;
	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|ll", &pwbo, &value, &item, &subitem) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(value)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(item, item_isnull)
		Z_PARAM_LONG_OR_NULL(subitem, subitem_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	((PWBOBJ)pwbo)->item = item;
	((PWBOBJ)pwbo)->subitem = subitem;

	RETURN_BOOL(wbSetValue((PWBOBJ)pwbo, value));
}

ZEND_FUNCTION(wb_set_range)
{
	zend_long pwbo, min = 0, max = 0;
	zend_bool max_isnull;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|l", &pwbo, &min, &max) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_LONG(min)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(max, max_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	RETURN_BOOL(wbSetRange((PWBOBJ)pwbo, min, max));
}

ZEND_FUNCTION(wb_create_item)
{
	char *s;
	size_t s_len;
	zend_long pwbo, param = 0;
	TCHAR *wcs = 0;
	zend_bool param_isnull;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls|l", &pwbo, &s, &s_len, &param) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_STRING(s, s_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(param, param_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}else
	{
		wcs = Utf82WideChar(s, s_len);
		RETURN_BOOL(wbCreateItem((PWBOBJ)pwbo, wcs));
	}
}

ZEND_FUNCTION(wb_create_statusbar_items)
{
	zend_long pwbo, clear, param;
	zval *zitems = NULL;
	BOOL bRet = TRUE;
	zend_bool clear_isnull, param_isnull;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz!|ll", &pwbo, &zitems, &clear, &param) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_ZVAL(zitems) // Has to be an array of arrays!
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(clear, clear_isnull)
		Z_PARAM_LONG_OR_NULL(param, param_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL();
	}

	switch (Z_TYPE_P(zitems))
	{

	case IS_ARRAY:
	{
		zval *zitem;
		int i;
		int nParts = 0;
		int aWidths[255];
		LPTSTR pszCaption;
		LONG_PTR nWidth;

		// Count array elements
		while ((zitem = process_array(zitems)) != NULL)
		{
			parse_array(zitem, "");
			nParts++;
		}

		// Create the array of widths

		i = 0;
		while ((zitem = process_array(zitems)) != NULL)
		{
			parse_array(zitem, "sl", &pszCaption, &nWidth);

			if ((i == nParts - 1) && (nWidth <= 0))
			{
				aWidths[i] = -1;
			}
			else
			{
				if (nWidth <= 4)
				{

					if (*pszCaption)
					{
						SIZE siz;

						if (wbGetTextSize(&siz, pszCaption, 0)){
							nWidth = siz.cx + 10; // This number is and arbitrary
						}else{
							nWidth = 10;
						}
					}
					else
					{
						nWidth = 4;
					}
				}
				aWidths[i] = (i ? aWidths[i - 1] : 0) + nWidth;
			}

			i++;
		}
		wbSetStatusBarParts((PWBOBJ)pwbo, nParts, aWidths);

		// Set the text of the various parts
		i = 0;
		while ((zitem = process_array(zitems)) != NULL)
		{
			parse_array(zitem, "sl", &pszCaption, NULL);
			
			if (!wbSetText((PWBOBJ)pwbo, pszCaption, i, FALSE)){
				bRet = FALSE;
			}
			i++;
		}
	}
	
	RETURN_BOOL(bRet);

	default:
		wbError(TEXT("wb_create_statusbar_items"), MB_ICONWARNING, TEXT("Parameter 2 expected to be an array in wb_create_statusbar_items"));
		RETURN_NULL();
	}
}

ZEND_FUNCTION(wb_set_text)
{
	zend_long pwbo, item = 0;
	BOOL ret = TRUE;
	zval *zcaption;
	char *caption = "";
	char *tooltip = "";
	TCHAR *wcsCaption = 0;
	TCHAR *wcsTooltip = 0;
	zend_bool item_isnull;

	//if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz|l", &pwbo, &zcaption, &item) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_ZVAL(zcaption)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(item, item_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_BOOL(FALSE);
	}
	switch (Z_TYPE_P(zcaption))
	{

	case IS_ARRAY:
		parse_array(zcaption, "ss", &caption, &tooltip);
		if (caption)
		{
			wcsCaption = Utf82WideChar(caption, 0);
			ret = wbSetText((PWBOBJ)pwbo, wcsCaption, item, FALSE);
		}
		if (tooltip)
		{
			wcsTooltip = Utf82WideChar(tooltip, 0);
			wbSetText((PWBOBJ)pwbo, wcsTooltip, item, TRUE);
		}
		break;

	case IS_STRING:
		//caption = zcaption->value.str.val;
		wcsCaption = Utf82WideChar(Z_STRVAL_P(zcaption), Z_STRLEN_P(zcaption));
		ret = wbSetText((PWBOBJ)pwbo, wcsCaption, item, FALSE);
		wbFree(wcsCaption);
		break;

	case IS_NULL:
		*caption = '\0';
		break;
	}

	RETURN_BOOL(ret);
}

/*
ZEND_FUNCTION(wb_set_text)
{
	LONG_PTR pwbo, item = 0;
	BOOL ret = TRUE;
    zval *zcaption;
	char *caption = "";
	char *tooltip = "";

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	 "lz|l", &pwbo, &zcaption, &item) == FAILURE)
		return;

	if(!wbIsWBObj((void *)pwbo, TRUE))
		RETURN_NULL()

	switch(zcaption->type) {

		case IS_ARRAY:
			parse_array(zcaption, "ss", &caption, &tooltip);
			break;

		case IS_STRING:
			caption = zcaption->value.str.val;
			break;

		case IS_NULL:
		case IS_LONG:
		case IS_DOUBLE:
		case IS_BOOL:
		case IS_CONSTANT:
//		default:
//		case IS_LONG:
//		case IS_BOOL:
//		case IS_FLOAT:
			printf("%d   ", zcaption->value.lval);
			convert_to_string_ex(&zcaption);
			printf("[%s]\n", zcaption->value.str.val);
			if(zcaption->value.str.val)
				caption = zcaption->value.str.val;
			else
				*caption = '\0';
			break;

//		case IS_NULL:
		default:
			*caption = '\0';
			break;
	}

	// ????????????<<<<<<<     Convert line breaks for the caption and tooltip

	if((!caption || !*caption) && zcaption->type == IS_STRING) {
		ret = wbSetText((PWBOBJ)pwbo, "", item, FALSE);
	} else if(caption && *caption) {
		ret = wbSetText((PWBOBJ)pwbo, caption, item, FALSE);
	}

	// Set tooltip

	if(tooltip && *tooltip) {
		ret = wbSetText((PWBOBJ)pwbo, tooltip, item, TRUE);
	}

	RETURN_BOOL(ret);
}
*/

ZEND_FUNCTION(wb_get_text)
{
	TCHAR *ptext = NULL;
	zend_long pwbo;
	zend_long len, index = -1;

	char *str = 0;
	int str_len = 0;
	zend_bool index_isnull;

	// NOTE: I don't quite understand why do I need all these
	// len + 1 and len - 1 stuff below, but it works

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &pwbo, &index) == FAILURE)
	// ZEND_PARSE_PARAMETERS_START() takes two arguments minimal and maximal parameters count.
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(pwbo)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(index, index_isnull)
	ZEND_PARSE_PARAMETERS_END();

	if (!wbIsWBObj((void *)pwbo, TRUE)){
		RETURN_NULL(); // This is an error, so return NULL
	}
	//Get rtf text
	if (((PWBOBJ)pwbo)->uClass == RTFEditBox && index == WBC_RTF_TEXT)
	{

		if (wbGetRtfText((PWBOBJ)pwbo, &str))
		{
			//if you use SF_TEXT|SF_UNICODE
			//str = WideChar2Utf8(ptext, &str_len);
			RETURN_STRINGL(str, strlen(str));
		}
		RETURN_STRING("");
	}

	len = wbGetTextLength((PWBOBJ)pwbo, index) + 1;
	if (len)
	{
		ptext = emalloc(sizeof(TCHAR) * (len + 1));
		if (!ptext)
		{
			RETURN_NULL(); // This is an error, so return NULL
		}
		else
		{
			wbGetText((PWBOBJ)pwbo, ptext, len, index);
			
			if (*ptext)
			{
				str = WideChar2Utf8(ptext, &str_len);
				efree(ptext);
				RETURN_STRINGL(str, max(0, str_len));
			}
			else
			{
				efree(ptext);
				RETURN_STRING(""); // This is a valid empty string
			}
		}
	}
	else
	{
		RETURN_STRING(""); // This is a valid empty string
	}
}

//------------------------------------------------------------------ END OF FILE