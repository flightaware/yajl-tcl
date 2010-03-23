/*
 *
 */

#include <tcl.h>
#include "yajltcl.h"


/*
 *--------------------------------------------------------------
 *
 * yajl --
 *
 *
 * Results:
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
    yajl_gen hand = yajlData->handle;
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
	"free",
	"get",
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
	OPT_FREE,
	OPT_GET
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
	      errString = "generation complete";
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
	    Tcl_SetObjResult (interp, Tcl_NewStringObj (errString, -1));
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
    yajl_gen_config yConfig;

    yajltcl_clientData *yajlData = (yajltcl_clientData *)ckalloc (sizeof (yajltcl_clientData));
    yajlData->interp = interp;
    Tcl_DStringInit (&yajlData->dString);

    yConfig.beautify = 1;
    yConfig.indentString = "\t";

    yajlData->handle = yajl_gen_alloc2 (yajltcl_print_callback, &yConfig, NULL, yajlData);


    if (objc != 3) {
        Tcl_WrongNumArgs (interp, 1, objv, "create name");
	return TCL_ERROR;
    }


    Tcl_CreateObjCommand (interp, Tcl_GetString (objv[2]), yajltcl_yajlObjectObjCmd, yajlData, NULL);

    return TCL_OK;
}

