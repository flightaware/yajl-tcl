/*
 * yajltcl
 */

#include <tcl.h>
#include "yajltcl.h"
#include "yajltcllex.h"
#include <string.h>


/* PARSER STUFF */

/* append_result_list - append to the Tcl result object a new string object
 * of the contents of the type string followed by the passed-in Tcl object
 */
static int
append_result_list (Tcl_Interp *interp, const char *type, int len, Tcl_Obj *object)
{
    Tcl_Obj *resultObj = Tcl_GetObjResult (interp);
    Tcl_ListObjAppendElement (interp, resultObj, Tcl_NewStringObj (type, len));
    Tcl_ListObjAppendElement (interp, resultObj, object);

    return 1;
}

/* append_string - append the passed-in string to the list being formed
 * on the result object
 */
static int
append_string (Tcl_Interp *interp, const char *string, int len)
{
    Tcl_ListObjAppendElement (interp, Tcl_GetObjResult (interp), Tcl_NewStringObj (string, len));
    return 1;
}

/* null_callback - append the string "null" to the list being formed
 * on the result object
 */
static int
null_callback (void *context)
{
    static const char nullStr[] = "null";
    //yajltcl_clientData *yajlData = (yajltcl_clientData *)context;
    yajltcl_clientData *yajlData = context;
    Tcl_Interp *interp = yajlData->interp;

    append_string (interp, nullStr, sizeof nullStr - 1);
    return 1;
}

/* boolean_callback - append the string "bool" to the list being formed
 * on the result object followed by a boolean value
 */
static int
boolean_callback (void *context, int boolean)
{
    static const char boolStr[] = "bool";
    yajltcl_clientData *yajlData = context;
    Tcl_Interp *interp = yajlData->interp;

    append_result_list (interp, boolStr, sizeof boolStr - 1, Tcl_NewBooleanObj(boolean));
    return 1;
}

/* integer_callback - append the string "integer" to the list being formed
 * on the result object followed by an integer value
 *
 * note that for newer yajl it's a long but for older, a long long
 */
static int
integer_callback (void *context, long long integerVal)
{
    static const char integerStr[] = "integer";
    yajltcl_clientData *yajlData = context;
    Tcl_Interp *interp = yajlData->interp;

    append_result_list (interp, integerStr, sizeof integerStr - 1, Tcl_NewLongObj(integerVal));
    return 1;
}

/* double_callback - append the string "double" to the list being formed
 * on the result object followed by an double-precision floating point value
 */
static int
double_callback (void *context, double doubleVal)
{
    static const char doubleStr[] = "double";
    yajltcl_clientData *yajlData = context;
    Tcl_Interp *interp = yajlData->interp;

    append_result_list (interp, doubleStr, sizeof doubleStr - 1, Tcl_NewDoubleObj(doubleVal));
    return 1;
}

/* number_callback - append the string "number" to the list being formed
 * on the result object followed by a string (!) containing a number of some
 * kind or another
 */
static int
number_callback (void *context, const char *s, size_t l)
{
    static const char numberStr[] = "number";
    yajltcl_clientData *yajlData = context;
    Tcl_Interp *interp = yajlData->interp;

    append_result_list (interp, numberStr, sizeof numberStr - 1, Tcl_NewStringObj(s, l));
    return 1;
}

/* string_callback - append the string "string" to the list being formed
 * on the result object followed by the passed-in string
 */
static int
string_callback (void *context, const unsigned char *stringVal, size_t stringLen)
{
    static const char stringStr[] = "string";
    yajltcl_clientData *yajlData = context;
    Tcl_Interp *interp = yajlData->interp;

    append_result_list (interp, stringStr, sizeof stringStr - 1, Tcl_NewStringObj((char *)stringVal, stringLen));
    return 1;
}

/* map_key_callback - append the string "map_key" to the list being formed
 * on the result object followed by the passed-in string
 */
static int
map_key_callback (void *context, const unsigned char *stringVal, size_t stringLen)
{
    static const char mapKeyStr[] = "map_key";
    yajltcl_clientData *yajlData = context;
    Tcl_Interp *interp = yajlData->interp;

    append_result_list (interp, mapKeyStr, sizeof mapKeyStr - 1, Tcl_NewStringObj((char *)stringVal, stringLen));
    return 1;
}

/* map_start_callback - append the string "map_open" to the list being formed
 * on the result object
 */
static int
map_start_callback (void *context)
{
    static const char mapOpenStr[] = "map_open";
    yajltcl_clientData *yajlData = context;
    Tcl_Interp *interp = yajlData->interp;

    append_string (interp, mapOpenStr, sizeof mapOpenStr - 1);
    return 1;
}

/* map_end_callback - append the string "map_close" to the list being formed
 * on the result object
 */
static int
map_end_callback (void *context)
{
    static const char mapCloseStr[] = "map_close";
    yajltcl_clientData *yajlData = context;
    Tcl_Interp *interp = yajlData->interp;

    append_string (interp, mapCloseStr, sizeof mapCloseStr - 1);
    return 1;
}

/* array_start_callback - append the string "array_open" to the list being
 * formed on the result object
 */
static int
array_start_callback (void *context)
{
    static const char arrayOpenStr[] = "array_open";
    yajltcl_clientData *yajlData = context;
    Tcl_Interp *interp = yajlData->interp;

    append_string (interp, arrayOpenStr, sizeof arrayOpenStr - 1);
    return 1;
}

/* array_end_callback - append the string "array_close" to the list being
 * formed on the result object
 */
static int
array_end_callback (void *context)
{
    static const char arrayCloseStr[] = "array_close";
    yajltcl_clientData *yajlData = context;
    Tcl_Interp *interp = yajlData->interp;

    append_string (interp, arrayCloseStr, sizeof arrayCloseStr - 1);
    return 1;
}

/* define the yajl callbacks table */
static yajl_callbacks callbacks = {
    null_callback,
    boolean_callback,
    integer_callback,
    double_callback,
    number_callback,
    string_callback,
    map_start_callback,
    map_key_callback,
    map_end_callback,
    array_start_callback,
    array_end_callback
};

inline void
parse2dict_possibly_insert_array_index (yajltcl_clientData *yajlData) {
    if (yajlData->arrayElement[yajlData->p2dDepth] == -1) return;

    int elementNum = yajlData->arrayElement[yajlData->p2dDepth]++;
    char str[16];
    snprintf (str, 16, "%d", elementNum);
    Tcl_DStringSetLength (&yajlData->dString, 0);
    Tcl_DStringAppend (&yajlData->dString, str, -1);
    Tcl_DStringAppendElement (&yajlData->p2dString, Tcl_DStringValue (&yajlData->dString));
}

/* parse2dict_null_callback - append a null element to the dynamic string
 */
static int
parse2dict_null_callback (void *context)
{
    yajltcl_clientData *yajlData = context;

    parse2dict_possibly_insert_array_index (yajlData);
    Tcl_DStringAppendElement (&yajlData->p2dString, "null");
    return 1;
}

/* parse2dict_boolean_callback - append a boolean element to the dynamic string
 */
static int
parse2dict_boolean_callback (void *context, int boolean)
{
    yajltcl_clientData *yajlData = context;

    parse2dict_possibly_insert_array_index (yajlData);
    Tcl_DStringAppendElement (&yajlData->p2dString, boolean ? "1" : "0");
    return 1;
}

/* parse2dict_number_callback - append a number element to the dynamic string
 */
static int
parse2dict_number_callback (void *context, const char *s, size_t l)
{
    yajltcl_clientData *yajlData = context;

    parse2dict_possibly_insert_array_index (yajlData);
    Tcl_DStringSetLength (&yajlData->dString, 0);
    Tcl_DStringAppend (&yajlData->dString, s, l);
    Tcl_DStringAppendElement (&yajlData->p2dString, Tcl_DStringValue (&yajlData->dString));
    return 1;
}

/* parse2dict_string_callback - append a element to the dynamic string
 */
static int
parse2dict_string_callback (void *context, const unsigned char *stringVal, size_t stringLen)
{
    yajltcl_clientData *yajlData = context;

    parse2dict_possibly_insert_array_index (yajlData);
    Tcl_DStringSetLength (&yajlData->dString, 0);
    Tcl_DStringAppend (&yajlData->dString, (const char *)stringVal, stringLen);
    Tcl_DStringAppendElement (&yajlData->p2dString, Tcl_DStringValue (&yajlData->dString));
    return 1;
}

/* parse2dict_start_sublist_callback - start a sublist
 */
static int
parse2dict_start_sublist_callback (void *context)
{
    yajltcl_clientData *yajlData = context;

    // start a sublist unless we're at the top level
    if (yajlData->p2dDepth++ > 0) {
	yajlData->arrayElement[yajlData->p2dDepth] = -1;
	Tcl_DStringStartSublist (&yajlData->p2dString);
    }
    return 1;
}

/* parse2dict_start_array_callback - start an array
 */
static int
parse2dict_start_array_callback (void *context)
{
    yajltcl_clientData *yajlData = context;

    // start a sublist unless we're at the top level
    if (yajlData->p2dDepth++ > 0) {
	yajlData->arrayElement[yajlData->p2dDepth] = 0;
	Tcl_DStringStartSublist (&yajlData->p2dString);
    }
    return 1;
}

/* parse2dict_end_sublist_callback - finish a sublist
 */
static int
parse2dict_end_sublist_callback (void *context)
{
    yajltcl_clientData *yajlData = context;

    if (--yajlData->p2dDepth > 0) {
	Tcl_DStringEndSublist (&yajlData->p2dString);
    }
    return 1;
}

/* parse2dict_end_array_callback - finish an array
 */
static int
parse2dict_end_array_callback (void *context)
{
    yajltcl_clientData *yajlData = context;

    if (--yajlData->p2dDepth > 0) {
	Tcl_DStringEndSublist (&yajlData->p2dString);
    }
    return 1;
}

/* define the yajl callbacks table */
static yajl_callbacks parse2dict_callbacks = {
    parse2dict_null_callback,
    parse2dict_boolean_callback,
    NULL,
    NULL,
    parse2dict_number_callback,
    parse2dict_string_callback,
    parse2dict_start_sublist_callback,
    parse2dict_string_callback,
    parse2dict_end_sublist_callback,
    parse2dict_start_array_callback,
    parse2dict_end_array_callback
};

// }}}

/* {{{ parse2huddle_null_callback - append a null element to the dynamic string */
static int
parse2huddle_null_callback (void *context)
{
    yajltcl_clientData *yajlData = context;

    Tcl_DStringAppendElement (&yajlData->p2dString, "s null");
    return 1;
}

/* parse2huddle_boolean_callback - append a boolean element to the dynamic string
 */
static int
parse2huddle_boolean_callback (void *context, int boolean)
{
    yajltcl_clientData *yajlData = context;

    Tcl_DStringAppendElement (&yajlData->p2dString, boolean ? "1" : "0");
    return 1;
}

/* parse2huddle_number_callback - append a number element to the dynamic string
 */
static int
parse2huddle_number_callback (void *context, const char *s, size_t l)
{
    yajltcl_clientData *yajlData = context;

    Tcl_DStringAppend (&yajlData->p2dString, "{s ",3);

    Tcl_DStringSetLength (&yajlData->dString, 0);
    Tcl_DStringAppend (&yajlData->dString, s, l);
    Tcl_DStringAppendElement (&yajlData->p2dString, Tcl_DStringValue (&yajlData->dString));
    Tcl_DStringAppend (&yajlData->p2dString, "} ",2);
    return 1;
}

/* parse2huddle_string_callback - append a element to the dynamic string
 */
static int
parse2huddle_string_callback (void *context, const unsigned char *stringVal, size_t stringLen)
{
    yajltcl_clientData *yajlData = context;

    Tcl_DStringAppend (&yajlData->p2dString, "{s ",3);

    Tcl_DStringSetLength (&yajlData->dString, 0);
    Tcl_DStringAppend (&yajlData->dString, (const char *)stringVal, stringLen);
    Tcl_DStringAppendElement (&yajlData->p2dString, Tcl_DStringValue (&yajlData->dString));
    Tcl_DStringAppend (&yajlData->p2dString, "} ",2);
    return 1;
}

/* parse2huddle_start_map_callback - start a map */
static int
parse2huddle_start_map_callback (void *context)
{
    yajltcl_clientData *yajlData = context;

    Tcl_DStringAppend (&yajlData->p2dString, " {D {",5);
    return 1;
}

/* parse2huddle_string_callback - append a element to the dynamic string
 */
static int
parse2huddle_map_key_callback (void *context, const unsigned char *stringVal, size_t stringLen)
{
    yajltcl_clientData *yajlData = context;

    Tcl_DStringSetLength (&yajlData->dString, 0);
    Tcl_DStringAppend (&yajlData->dString, (const char *)stringVal, stringLen);
    Tcl_DStringAppendElement (&yajlData->p2dString, Tcl_DStringValue (&yajlData->dString));
    Tcl_DStringAppend (&yajlData->p2dString, " ",1);
    return 1;
}

/* parse2huddle_end_map_callback - finish a map
 */
static int
parse2huddle_end_map_callback (void *context)
{
    yajltcl_clientData *yajlData = context;

    Tcl_DStringAppend (&yajlData->p2dString, "}}",2);
    return 1;
}

/* parse2huddle_start_map_callback - start an array */
static int
parse2huddle_start_array_callback (void *context)
{
    yajltcl_clientData *yajlData = context;

    Tcl_DStringAppend (&yajlData->p2dString, "{L {",4);
    return 1;
}
/* parse2huddle_end_array_callback - end an array */
static int
parse2huddle_end_array_callback (void *context)
{
    yajltcl_clientData *yajlData = context;

    Tcl_DStringAppend (&yajlData->p2dString, "}}",2);
    return 1;
}

/* define the yajl callbacks table */
static yajl_callbacks parse2huddle_callbacks = {
    parse2huddle_null_callback,                   //   null_callback,
    parse2huddle_boolean_callback,                //   boolean_callback,
    NULL,                                         //   integer_callback,
    NULL,                                         //   double_callback,
    parse2huddle_number_callback,                 //   number_callback,
    parse2huddle_string_callback,                 //   string_callback,
    parse2huddle_start_map_callback,              //   map_start_callback,
    parse2huddle_map_key_callback,                 //   map_key_callback,
    parse2huddle_end_map_callback,                //   map_end_callback,
    parse2huddle_start_array_callback,            //   array_start_callback,
    parse2huddle_end_array_callback               //   array_end_callback
};


// }}}

// {{{

/*
 *--------------------------------------------------------------
 *
 * yajltcl_free_parsers -- free the YAJL parsers and associated
 *  data.
 *
 * Results:
 *      frees the YAJL parser handles for all that exist.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */
void
yajltcl_free_parsers (yajltcl_clientData *yajlData)
{
    Tcl_DStringFree (&yajlData->p2dString);

    if (yajlData->parseHandle != NULL) {
        yajl_free (yajlData->parseHandle);
    }

    if (yajlData->parse2dictHandle != NULL) {
        yajl_free (yajlData->parse2dictHandle);
    }

    if (yajlData->parse2huddleHandle != NULL) {
        yajl_free (yajlData->parse2huddleHandle);
    }
}


/*
 *--------------------------------------------------------------
 *
 * yajltcl_make_parser -- create one instance of the YAJL
 * parser and associated data.
 *
 * Results:
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */
yajl_handle
yajltcl_make_parser (yajltcl_clientData *yajlData, yajl_callbacks *callbacks)
{
    yajl_handle parseHandle;

    parseHandle = yajl_alloc (callbacks, NULL, yajlData);

    yajl_config(parseHandle, yajl_allow_comments, yajlData->parseConfig.allowComments);
    yajl_config(parseHandle, yajl_dont_validate_strings, !yajlData->parseConfig.checkUTF8);

    return parseHandle;
}

/*
 *--------------------------------------------------------------
 *
 * yajltcl_recreate_parsers -- create or recreate the YAJL parsers
 * and associated data.
 *
 * Results:
 *      ...frees the YAJL parser's handles for ones that exists.
 *      ...creates new YAJL parser objects.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */
void
yajltcl_recreate_parsers (yajltcl_clientData *yajlData)
{
    yajltcl_free_parsers (yajlData);

    yajlData->parseHandle = yajltcl_make_parser (yajlData, &callbacks);
    yajlData->parse2dictHandle = yajltcl_make_parser (yajlData, &parse2dict_callbacks);
    yajlData->parse2huddleHandle = yajltcl_make_parser (yajlData, &parse2huddle_callbacks);
}


/* GENERATOR STUFF */



/*
 *--------------------------------------------------------------
 *
 * yajltcl_print_callback -- callback routine for when YAJL wants to "print"
 * something -- we grab it and append it to a Tcl dynamic string in the
 * yajltcl clientData that we maintain.
 *
 * Results:
 *      yajlData->dString is appended to.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */
void
yajltcl_print_callback (void *context, const char *str, unsigned int len)
{
    yajltcl_clientData *yajlData = (yajltcl_clientData *)context;
    Tcl_DStringAppend (&yajlData->dString, str, len);
}


/*
 *--------------------------------------------------------------
 *
 * yajltcl_free_generator -- free the YAJL generator and associated
 *  data.
 *
 * Results:
 *      frees the YAJL generator handle if it exists.
 *      frees the Tcl Dynamic string we use to build up the JSON.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */
void
yajltcl_free_generator (yajltcl_clientData *yajlData)
{
    if (yajlData->genHandle != NULL) {
        yajl_gen_free (yajlData->genHandle);
    }

    Tcl_DStringFree (&yajlData->dString);
}


/*
 *--------------------------------------------------------------
 *
 * yajltcl_recreate_generator -- create or recreate the YAJL generator
 * and associated data.
 *
 * Results:
 *      ...frees the YAJL generator handle if it exists.
 *      ...frees the Tcl Dynamic string we use to build up the JSON.
 *      ...creates a new YAJL generator object.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */
void
yajltcl_recreate_generator (yajltcl_clientData *yajlData)
{
    yajltcl_free_generator (yajlData);
    yajlData->genHandle = yajl_gen_alloc(NULL);
    yajl_gen_config(yajlData->genHandle, yajl_gen_print_callback, yajltcl_print_callback, yajlData);
    yajl_gen_config(yajlData->genHandle, yajl_gen_beautify, yajlData->genConfig.beautify);
    yajl_gen_config(yajlData->genHandle, yajl_gen_indent_string, yajlData->genConfig.indentString);
}


/*
 *--------------------------------------------------------------
 *
 * yajltcl_yajlObjectDelete -- command deletion callback routine.
 *
 * Results:
 *      ...frees the YAJL generator handle if it exists.
 *      ...frees the Tcl Dynamic string we use to build up the JSON.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */
void
yajltcl_yajlObjectDelete (ClientData clientData)
{
    yajltcl_clientData *yajlData = (yajltcl_clientData *)clientData;

    yajltcl_free_generator (yajlData);
    yajltcl_free_parsers (yajlData);

    ckfree(clientData);
}


/*
 *----------------------------------------------------------------------
 *
 * yajltcl_yajlObjectObjCmd --
 *
 *    dispatches the subcommands of a yajl object command
 *
 * Results:
 *    stuff
 *
 *----------------------------------------------------------------------
 */
int
yajltcl_yajlObjectObjCmd(ClientData cData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int         optIndex;
    int         arg;
    yajltcl_clientData *yajlData = (yajltcl_clientData *)cData;
    yajl_gen hand = yajlData->genHandle;
    yajl_gen_status gstatus = yajl_gen_status_ok;
    yajl_status pstatus = yajl_status_ok;
    const char *errString = NULL;

    static CONST char *options[] = {
        "array_open",
        "array_close",
        "bool",
        "clear",
        "double",
        "integer",
        "map_close",
        "map_open",
        "null",
        "number",
        "string",
        "map_key",
        "free",
        "get",
        "reset",
        "delete",
        "parse",
        "parse2dict",
        "parse2huddle",
        "parse_complete",
        NULL
    };

    enum options {
        OPT_ARRAY_OPEN,
        OPT_ARRAY_CLOSE,
        OPT_BOOL,
        OPT_CLEAR,
        OPT_DOUBLE,
        OPT_INTEGER,
        OPT_MAP_CLOSE,
        OPT_MAP_OPEN,
        OPT_NULL,
        OPT_NUMBER,
        OPT_STRING,
        OPT_MAP_KEY,
        OPT_FREE,
        OPT_GET,
        OPT_RESET,
        OPT_DELETE,
        OPT_PARSE,
        OPT_PARSE2DICT,
        OPT_PARSE2HUDDLE,
        OPT_PARSE_COMPLETE
    };

    /* basic validation of command line arguments */
    if (objc < 2) {
        Tcl_WrongNumArgs (interp, 1, objv, "option ?value? ?option ?value?...?");
        return TCL_ERROR;
    }

    /* for each argument, see if it's array_open, array_close, bool, clear,
     * double, integer, map_close, map_open, null, number, string, map_key,
     * free, get, reset, delete, parse, or parse_complete and handle
     * accordingly
     */
    for (arg = 1; arg < objc; arg++) {

	/* convert the option name to an enum index -- if we get an error,
	 * we're done
	 */
        if (Tcl_GetIndexFromObj (interp, objv[arg], options, "option",
            TCL_EXACT, &optIndex) != TCL_OK) {
            return TCL_ERROR;
        }

        switch ((enum options) optIndex) {
	  // array_open?  generate an array_open
          case OPT_ARRAY_OPEN: {
              gstatus = yajl_gen_array_open (hand);
              break;
          }

	  // array_close?  generate an array_close
          case OPT_ARRAY_CLOSE: {
              gstatus = yajl_gen_array_close (hand);
              break;
          }

	  // map_open?  generate a map_open
          case OPT_MAP_OPEN: {
              gstatus = yajl_gen_map_open (hand);
              break;
          }

	  // map_close?  generate a map_close
          case OPT_MAP_CLOSE: {
              gstatus = yajl_gen_map_close (hand);
              break;
          }

	  // null?  generate a null
          case OPT_NULL: {
              gstatus = yajl_gen_null (hand);
              break;
          }

	  // get?  generate the string into the result
          case OPT_GET: {
              Tcl_DStringResult (interp, &yajlData->dString);
              Tcl_DStringFree (&yajlData->dString);
              return TCL_OK;
          }

	  // reset?  reset the generator and the parser
          case OPT_RESET: {
              yajltcl_recreate_generator (yajlData);
              yajltcl_recreate_parsers (yajlData);
              return TCL_OK;
          }

	  // delete?  delete the yajl-tcl command we created earlier
          case OPT_DELETE: {
              Tcl_DeleteCommandFromToken(interp, yajlData->cmdToken);
              return TCL_OK;
          }

	  // clear? - clear the yajl result
          case OPT_CLEAR: {
              yajl_gen_clear (hand);
              gstatus = yajl_gen_status_ok;
              Tcl_DStringFree (&yajlData->dString);
              break;
          }

	  // bool? generate a boolean string and value
          case OPT_BOOL: {
              int bool;

              if (arg + 1 >= objc) {
                  Tcl_WrongNumArgs (interp, 1, objv, "bool value");
                  return TCL_ERROR;
              }

              if (Tcl_GetBooleanFromObj (interp, objv[++arg], &bool) == TCL_ERROR) {
                  return TCL_ERROR;
              }

              gstatus = yajl_gen_bool (hand, bool);
              break;
          }

	  // double? generate a double string and value
          case OPT_DOUBLE: {
              double doub;

              if (arg + 1 >= objc) {
                  Tcl_WrongNumArgs (interp, 1, objv, "double value");
                  return TCL_ERROR;
              }

              if (Tcl_GetDoubleFromObj (interp, objv[++arg], &doub) == TCL_ERROR) {
                  return TCL_ERROR;
              }

              gstatus = yajl_gen_double (hand, doub);
              break;
          }

	  // integer? generate an integer string and value
          case OPT_INTEGER: {
              long lon;

              if (arg + 1 >= objc) {
                  Tcl_WrongNumArgs (interp, 1, objv, "integer value");
                  return TCL_ERROR;
              }

              if (Tcl_GetLongFromObj (interp, objv[++arg], &lon) == TCL_ERROR) {
                  return TCL_ERROR;
              }

              gstatus = yajl_gen_integer (hand, lon);
              break;
          }

	  // number? generate a number string and value
          case OPT_NUMBER: {
              char *number;
              int   len;

              if (arg + 1 >= objc) {
                  Tcl_WrongNumArgs (interp, 1, objv, "number value");
                  return TCL_ERROR;
              }

              number = Tcl_GetStringFromObj (objv[++arg], &len);
              if (!numberValidator(number)) {
                Tcl_AppendResult(interp, "Invalid value \"", number ,"\" for number input.", NULL);
                return TCL_ERROR;
              }
              gstatus = yajl_gen_number (hand, number, len);
              break;
          }

	  // map_key or string? generate a "string" string and value
          case OPT_MAP_KEY:
          case OPT_STRING: {
              char *string;
              int   len;

              if (arg + 1 >= objc) {
                  Tcl_WrongNumArgs (interp, 1, objv, "string value");
                  return TCL_ERROR;
              }

              string = Tcl_GetStringFromObj (objv[++arg], &len);
              gstatus = yajl_gen_string (hand, (unsigned char *)string, len);
              break;
          }

	  // parse? generate a parse of the following json text
	  // and return
          case OPT_PARSE2DICT:
          case OPT_PARSE2HUDDLE:
          case OPT_PARSE: {
              char *string;
              int   len;
	      yajl_handle parseHandle = NULL;

              if (arg + 1 >= objc) {
                  Tcl_WrongNumArgs (interp, 1, objv, "parse jsonText");
                  return TCL_ERROR;
              } 

              switch ((enum options) optIndex) {
          	case OPT_PARSE: 	parseHandle = yajlData->parseHandle; break;
          	case OPT_PARSE2DICT: 	parseHandle = yajlData->parse2dictHandle; break;
          	case OPT_PARSE2HUDDLE: 	parseHandle = yajlData->parse2huddleHandle; break;
		default: break;	
	      }

              string = Tcl_GetStringFromObj (objv[++arg], &len);
              pstatus = yajl_parse (parseHandle, (unsigned char *)string, len);

              if (pstatus != yajl_status_ok) {
                  unsigned char *str = yajl_get_error (parseHandle, 1, (unsigned char *)string, len);
                  Tcl_ResetResult (interp);
                  Tcl_SetObjResult (interp, Tcl_NewStringObj ((char *)str, -1));
                  yajl_free_error (parseHandle, str);
                  return TCL_ERROR;
              }

	      // parse2dict? set the Tcl result to the dynamic string
	      // we've been building
	      if ((enum options) optIndex == OPT_PARSE2DICT) {
	          Tcl_DStringResult (interp, &yajlData->p2dString);
	      }

	      if ((enum options) optIndex == OPT_PARSE2HUDDLE) {
	          Tcl_DStringResult (interp, &yajlData->p2dString);
	      }
              break;
          }

	  // parse_complete? mark that the parse is complete
          case OPT_PARSE_COMPLETE: {
              yajl_complete_parse (yajlData->parseHandle);
              yajl_complete_parse (yajlData->parse2dictHandle);
              yajl_complete_parse (yajlData->parse2huddleHandle);
              break;
          }

	  // free? no-op
          case OPT_FREE: {
          }

        }

	// dispatch based on the status we got back from yajl
        switch (gstatus) {
          case yajl_gen_status_ok: {
	      // ok - cool
              break;
          }

          case yajl_gen_keys_must_be_strings: {
	      // map without a map key, error
              errString = "map key needed but string not called";
              break;
          }

          case yajl_max_depth_exceeded: {
	      // max depth exceeded, error
              errString = "maximum generation depth exceeded";
              break;
          }

          case yajl_gen_in_error_state: {
	      // can't generate without clearing the error
              errString = "generator option called while in error state";
              break;
          }

          case yajl_gen_generation_complete: {
	      // can't do anything after generation is complete without 
	      // resetting
              errString = "generation complete, reset the object before reuse";
              break;
          }

          case yajl_gen_invalid_number: {
	      // invalid number error
              errString = "invalid floating point value";
              break;
          }

          case yajl_gen_no_buf: {
              errString = "no internal buffer";
              break;
          }

          case yajl_gen_invalid_string: {
	      // invalid string error
              errString = "invalid string";
              break;
          }
        }

        if (pstatus != yajl_status_ok) {
            errString = yajl_status_to_string (pstatus);
        }

	// if errString is set, pass the error back to Tcl with as much
	// info as possible and in a standard way
        if (errString != NULL) {
            char argString[32];

            Tcl_SetObjResult (interp, Tcl_NewStringObj (errString, -1));
            Tcl_AddErrorInfo (interp, " while processing argument ");
            sprintf (argString, "%d", arg);
            Tcl_AddErrorInfo (interp, argString);
            Tcl_AddErrorInfo (interp," \"");
            Tcl_AddErrorInfo (interp, Tcl_GetString (objv[arg]));
            Tcl_AddErrorInfo (interp, "\"");
            return TCL_ERROR;
        }
    }

    return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * yajltcl_yajlObjCmd --
 *
 *      This procedure is invoked to process the "yajl" command.
 *      See the user documentation for details on what it does.
 *
 * Results:
 *      A standard Tcl result.
 *
 * Side effects:
 *      See the user documentation.
 *
 *----------------------------------------------------------------------
 */

    /* ARGSUSED */
int
yajltcl_yajlObjCmd(clientData, interp, objc, objv)
    ClientData clientData;              /* registered proc hashtable ptr. */
    Tcl_Interp *interp;                 /* Current interpreter. */
    int objc;                           /* Number of arguments. */
    Tcl_Obj   *CONST objv[];
{
    yajltcl_clientData *yajlData;
    int                 optIndex;
    int                 suboptIndex;
    int                 i;
    char               *commandName;
    int                 autoGeneratedName;

    static CONST char *options[] = {
        "create",
        NULL
    };

    enum options {
        OPT_CREATE
    };

    static CONST char *subOptions[] = {
        "-beautify",
        "-indent",
        "-allowComments",
        "-checkUTF8",
        NULL
    };

    enum suboptions {
        SUBOPT_BEAUTIFY,
        SUBOPT_INDENT,
        SUBOPT_ALLOWCOMMENTS,
        SUBOPT_CHECKUTF8
    };

    // basic command line processing
    if (objc < 3 || (objc & 1) == 0) {
        Tcl_WrongNumArgs (interp, 1, objv, "create name ?-beautify 0|1? ?-indent string?");
        return TCL_ERROR;
    }

    // argument must be one of the subOptions defined above
    if (Tcl_GetIndexFromObj (interp, objv[1], options, "option",
        TCL_EXACT, &optIndex) != TCL_OK) {
        return TCL_ERROR;
    }

    // allocate one of our yajl client data objects for Tcl and configure it
    yajlData = (yajltcl_clientData *)ckalloc (sizeof (yajltcl_clientData));

    yajlData->genConfig.beautify = 0;
    yajlData->genConfig.indentString = "\t";

    yajlData->parseConfig.checkUTF8 = 0;
    yajlData->parseConfig.allowComments = 0;

    yajlData->interp = interp;
    yajlData->genHandle = NULL;
    yajlData->parseHandle = NULL;
    yajlData->parse2dictHandle = NULL;
    yajlData->parse2huddleHandle = NULL;
    yajlData->p2dDepth = 0;
    yajlData->arrayElement[0] = -1;
    Tcl_DStringInit (&yajlData->dString);
    Tcl_DStringInit (&yajlData->p2dString);

    // process the remaining arguments as key-value pairs
    for (i = 3; i < objc; i += 2) {
        if (Tcl_GetIndexFromObj (interp, objv[i], subOptions, "suboption",
            TCL_EXACT, &suboptIndex) != TCL_OK) {
            return TCL_ERROR;
        }

        switch ((enum suboptions)suboptIndex ) {
	    // set the beautify option (nested versus flat appearance)
            case SUBOPT_BEAUTIFY: {
                int beautify;

                if (Tcl_GetBooleanFromObj (interp, objv[i+1], &beautify) == TCL_ERROR) {
                    return TCL_ERROR;
                }
                yajlData->genConfig.beautify = beautify;
                break;
            }

	    // set the indent option
            case SUBOPT_INDENT: {
                yajlData->genConfig.indentString = Tcl_GetString (objv[i+1]);
                break;
            }

	    // set the allow comments option
            case SUBOPT_ALLOWCOMMENTS: {
                int allowComments;

                if (Tcl_GetBooleanFromObj (interp, objv[i+1], &allowComments) == TCL_ERROR) {
                    return TCL_ERROR;
                }
                yajlData->parseConfig.allowComments = allowComments;
                break;
            }

	    // set the "check UTF8" option
            case SUBOPT_CHECKUTF8: {
                int checkUTF8;

                if (Tcl_GetBooleanFromObj (interp, objv[i+1], &checkUTF8) == TCL_ERROR) {
                    return TCL_ERROR;
                }
                yajlData->parseConfig.checkUTF8 = checkUTF8;
                break;
            }
        }
    }

    yajltcl_recreate_generator (yajlData);
    yajltcl_recreate_parsers (yajlData);

    commandName = Tcl_GetString (objv[2]);

    // if commandName is #auto, generate a unique name for the object
    autoGeneratedName = 0;
    if (strcmp (commandName, "#auto") == 0) {
        static unsigned long nextAutoCounter = 0;
        char *objName;
        int    baseNameLength;

        objName = Tcl_GetStringFromObj (objv[0], &baseNameLength);
        baseNameLength += snprintf (NULL, 0, "%lu", nextAutoCounter) + 1;
        commandName = ckalloc (baseNameLength);
        snprintf (commandName, baseNameLength, "%s%lu", objName, nextAutoCounter++);
        autoGeneratedName = 1;
    }

    // create a Tcl command to interface to yajl
    yajlData->cmdToken = Tcl_CreateObjCommand (interp, commandName, yajltcl_yajlObjectObjCmd, yajlData, yajltcl_yajlObjectDelete);
    Tcl_SetObjResult (interp, Tcl_NewStringObj (commandName, -1));
    if (autoGeneratedName == 1) {
        ckfree(commandName);
    }
    return TCL_OK;
}
// }}}
//
// # vim: set ts=8 sw=4 sts=4 noet :
