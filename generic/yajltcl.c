/*
 *
 */

#include <tcl.h>
#include "yajltcl.h"
#include <string.h>


/* PARSER STUFF */

static int
append_result_list (Tcl_Interp *interp, char *type, Tcl_Obj *object) 
{
    Tcl_Obj *resultObj = Tcl_GetObjResult (interp);
    Tcl_ListObjAppendElement (interp, resultObj, Tcl_NewStringObj (type, -1));
    Tcl_ListObjAppendElement (interp, resultObj, object);

    return 1;
}

static int
append_string (Tcl_Interp *interp, char *string)
{
     Tcl_ListObjAppendElement (interp, Tcl_GetObjResult (interp), Tcl_NewStringObj (string, -1));
    return 1;
}

static int
null_callback (void *context)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

    append_string (interp, "null");
    return 1;
}

static int
boolean_callback (void *context, int boolean)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_result_list (interp, "boolean", Tcl_NewBooleanObj(boolean));
    return 1;
}

static int
integer_callback (void *context, long integerVal)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_result_list (interp, "integer", Tcl_NewLongObj(integerVal));
    return 1;
}

static int
double_callback (void *context, double doubleVal)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_result_list (interp, "double", Tcl_NewDoubleObj(doubleVal));
    return 1;
}

static int
number_callback (void *context, const char *s, unsigned int l)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_result_list (interp, "number", Tcl_NewStringObj(s, l));
    return 1;
}

static int
string_callback (void *context, const unsigned char *stringVal, unsigned int stringLen)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_result_list (interp, "string", Tcl_NewStringObj((char *)stringVal, stringLen));
    return 1;
}

static int
map_key_callback (void *context, const unsigned char *stringVal, unsigned int stringLen)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_result_list (interp, "map_key", Tcl_NewStringObj((char *)stringVal, stringLen));
    return 1;
}

static int
map_start_callback (void *context)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_string (interp, "map_open");
    return 1;
}

static int
map_end_callback (void *context)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_string (interp, "map_close");
    return 1;
}

static int
array_start_callback (void *context)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_string (interp, "array_open");
    return 1;
}

static int
array_end_callback (void *context)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_string (interp, "array_close");
    return 1;
}

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


/*
 *--------------------------------------------------------------
 *
 * yajltcl_free_parser -- free the YAJL parser and associated
 *  data.
 *
 * Results:
 *      frees the YAJL parser handle if it exists.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
void
yajltcl_free_parser (yajltcl_clientData *yajlData)
{
    if (yajlData->parseHandle != NULL) {
	yajl_free (yajlData->parseHandle);
    }
}


/*
 *--------------------------------------------------------------
 *
 * yajltcl_recreate_parser -- create or recreate the YAJL parser 
 * and associated data.
 *
 * Results:
 *      ...frees the YAJL parser handle if it exists.
 *      ...creates a new YAJL parser object.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
void
yajltcl_recreate_parser (yajltcl_clientData *yajlData)
{
    yajltcl_free_parser (yajlData);

    yajlData->parseHandle = yajl_alloc (&callbacks, &yajlData->parseConfig, NULL, yajlData->interp);
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
 *	None.
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
 *	None.
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
 *	None.
 *
 *--------------------------------------------------------------
 */
void
yajltcl_recreate_generator (yajltcl_clientData *yajlData)
{
    yajltcl_free_generator (yajlData);

    yajlData->genHandle = yajl_gen_alloc2 (yajltcl_print_callback, &yajlData->genConfig, NULL, yajlData);
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
 *	None.
 *
 *--------------------------------------------------------------
 */
void
yajltcl_yajlObjectDelete (ClientData clientData)
{
    yajltcl_clientData *yajlData = (yajltcl_clientData *)clientData;

    yajltcl_free_generator (yajlData);
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
    yajl_gen_status status = yajl_gen_status_ok;
    char *errString = NULL;

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
	"parse",
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
	OPT_PARSE,
	OPT_PARSE_COMPLETE
    };

    if (objc < 2) {
        Tcl_WrongNumArgs (interp, 1, objv, "option ?value? ?option ?value?...?");
	return TCL_ERROR;
    }

    for (arg = 1; arg < objc; arg++) {

	if (Tcl_GetIndexFromObj (interp, objv[arg], options, "option",
	    TCL_EXACT, &optIndex) != TCL_OK) {
	    return TCL_ERROR;
	}

	switch ((enum options) optIndex) {
	  case OPT_ARRAY_OPEN: {
	      status = yajl_gen_array_open (hand);
	      break;
	  }

	  case OPT_ARRAY_CLOSE: {
	      status = yajl_gen_array_close (hand);
	      break;
	  }

	  case OPT_MAP_OPEN: {
	      status = yajl_gen_map_open (hand);
	      break;
	  }

	  case OPT_MAP_CLOSE: {
	      status = yajl_gen_map_close (hand);
	      break;
	  }

	  case OPT_NULL: {
	      status = yajl_gen_null (hand);
	      break;
	  }

	  case OPT_GET: {
	      Tcl_DStringResult (interp, &yajlData->dString);
	      Tcl_DStringFree (&yajlData->dString);
	      return TCL_OK;
	  }

	  case OPT_RESET: {
	      yajltcl_recreate_generator (yajlData);
	      yajltcl_recreate_parser (yajlData);
	      return TCL_OK;
	  }

	  case OPT_CLEAR: {
	      yajl_gen_clear (hand);
	      status = yajl_gen_status_ok;
	      Tcl_DStringFree (&yajlData->dString);
	      break;
	  }

	  case OPT_BOOL: {
	      int bool;

	      if (arg + 1 >= objc) {
		Tcl_WrongNumArgs (interp, 1, objv, "bool value");
		return TCL_ERROR;
	      }

	      if (Tcl_GetBooleanFromObj (interp, objv[++arg], &bool) == TCL_ERROR) {
		  return TCL_ERROR;
	      }

	      status = yajl_gen_bool (hand, bool);
	      break;
	  }

	  case OPT_DOUBLE: {
	      double doub;

	      if (arg + 1 >= objc) {
		Tcl_WrongNumArgs (interp, 1, objv, "double value");
		return TCL_ERROR;
	      }

	      if (Tcl_GetDoubleFromObj (interp, objv[++arg], &doub) == TCL_ERROR) {
		  return TCL_ERROR;
	      }

	      status = yajl_gen_double (hand, doub);
	      break;
	  }

	  case OPT_INTEGER: {
	      long lon;

	      if (arg + 1 >= objc) {
		Tcl_WrongNumArgs (interp, 1, objv, "integer value");
		return TCL_ERROR;
	      }

	      if (Tcl_GetLongFromObj (interp, objv[++arg], &lon) == TCL_ERROR) {
		  return TCL_ERROR;
	      }

	      status = yajl_gen_integer (hand, lon);
	      break;
	  }

	  case OPT_NUMBER: {
	      char *number;
	      int   len;

	      if (arg + 1 >= objc) {
		Tcl_WrongNumArgs (interp, 1, objv, "number value");
		return TCL_ERROR;
	      }

	      number = Tcl_GetStringFromObj (objv[++arg], &len);
	      status = yajl_gen_number (hand, number, len);
	      break;
	  }

	  case OPT_MAP_KEY:
	  case OPT_STRING: {
	      char *string;
	      int   len;

	      if (arg + 1 >= objc) {
		Tcl_WrongNumArgs (interp, 1, objv, "string value");
		return TCL_ERROR;
	      }

	      string = Tcl_GetStringFromObj (objv[++arg], &len);
	      status = yajl_gen_string (hand, (unsigned char *)string, len);
	      break;
	  }

	  case OPT_PARSE: {
	      char *string;
	      int   len;

	      if (arg + 1 >= objc) {
		Tcl_WrongNumArgs (interp, 1, objv, "parse jsonText");
		return TCL_ERROR;
	      }

	      string = Tcl_GetStringFromObj (objv[++arg], &len);
	      status = yajl_parse (yajlData->parseHandle, (unsigned char *)string, len);

	      if (status != yajl_status_ok && status != yajl_status_insufficient_data) {
	          unsigned char *str = yajl_get_error (yajlData->parseHandle, 1, (unsigned char *)string, len);
		  Tcl_ResetResult (interp);
		  Tcl_SetObjResult (interp, Tcl_NewStringObj ((char *)str, -1));
		  return TCL_ERROR;
	      }
	      break;
	  }

	  case OPT_PARSE_COMPLETE: {
	      yajl_parse_complete (yajlData->parseHandle);
	      break;
	  }

	  case OPT_FREE: {
	  }

	}

	switch (status) {
	  case yajl_gen_status_ok: {
	      break;
	  }

	  case yajl_gen_keys_must_be_strings: {
	      errString = "map key needed but string not called";
	      break;
	  }

	  case yajl_max_depth_exceeded: {
	      errString = "maximum generation depth exceeded";
	      break;
	  }

	  case yajl_gen_in_error_state: {
	      errString = "generator option called while in error state";
	      break;
	  }

	  case yajl_gen_generation_complete: {
	      errString = "generation complete, reset the object before reuse";
	      break;
	  }

	  case yajl_gen_invalid_number: {
	      errString = "invalid floating point value";
	      break;
	  }

	  case yajl_gen_no_buf: {
	      errString = "no internal buffer";
	      break;
	  }
	}

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
 *	This procedure is invoked to process the "yajl" command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

    /* ARGSUSED */
int
yajltcl_yajlObjCmd(clientData, interp, objc, objv)
    ClientData clientData;		/* registered proc hashtable ptr. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int objc;				/* Number of arguments. */
    Tcl_Obj   *CONST objv[];
{
    yajltcl_clientData *yajlData;
    int                 optIndex;
    int                 suboptIndex;
    int                 i;
    char               *commandName;

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

    if (objc < 3 || (objc & 1) == 0) {
        Tcl_WrongNumArgs (interp, 1, objv, "create name ?-beautify 0|1? ?-indent string?");
	return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj (interp, objv[1], options, "option",
	TCL_EXACT, &optIndex) != TCL_OK) {
	return TCL_ERROR;
    }

    yajlData = (yajltcl_clientData *)ckalloc (sizeof (yajltcl_clientData));

    yajlData->genConfig.beautify = 0;
    yajlData->genConfig.indentString = "\t";

    yajlData->parseConfig.checkUTF8 = 0;
    yajlData->parseConfig.allowComments = 0;

    yajlData->interp = interp;
    yajlData->genHandle = NULL;
    yajlData->parseHandle = NULL;
    Tcl_DStringInit (&yajlData->dString);

    for (i = 3; i < objc; i += 2) {
        if (Tcl_GetIndexFromObj (interp, objv[i], subOptions, "suboption",
	    TCL_EXACT, &suboptIndex) != TCL_OK) {
	    return TCL_ERROR;
	}

	switch ((enum suboptions)suboptIndex ) {
	    case SUBOPT_BEAUTIFY: {
	        int beautify;

	        if (Tcl_GetBooleanFromObj (interp, objv[i+1], &beautify) == TCL_ERROR) {
		    return TCL_ERROR;
		}
		yajlData->genConfig.beautify = beautify;
	        break;
	    }

	    case SUBOPT_INDENT: {
	        yajlData->genConfig.indentString = Tcl_GetString (objv[i+1]);
	        break;
	    }

	    case SUBOPT_ALLOWCOMMENTS: {
	        int allowComments;

	        if (Tcl_GetBooleanFromObj (interp, objv[i+1], &allowComments) == TCL_ERROR) {
		    return TCL_ERROR;
		}
	        yajlData->parseConfig.allowComments = allowComments;
	        break;
	    }

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
    yajltcl_recreate_parser (yajlData);

    commandName = Tcl_GetString (objv[2]);

    // if commandName is #auto, generate a name
    if (strcmp (commandName, "#auto") == 0) {
        unsigned long nextAutoCounter = 0;
	char *objName;
	int    baseNameLength;

	objName = Tcl_GetStringFromObj (objv[0], &baseNameLength);
	baseNameLength += 42;
	commandName = ckalloc (baseNameLength);
	snprintf (commandName, baseNameLength, "%s%lu", objName, nextAutoCounter++);
    }


    Tcl_CreateObjCommand (interp, commandName, yajltcl_yajlObjectObjCmd, yajlData, yajltcl_yajlObjectDelete);
    Tcl_SetObjResult (interp, Tcl_NewStringObj (commandName, -1));
    return TCL_OK;
}

