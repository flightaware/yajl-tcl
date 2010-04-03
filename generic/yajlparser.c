/*
 *
 */


#include <tcl.h>
#include "yajltcl.h"
#include <string.h>


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

     append_string (interp, "map_start");
    return 1;
}

static int
map_end_callback (void *context)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_string (interp, "map_end");
    return 1;
}

static int
array_start_callback (void *context)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_string (interp, "array_start");
    return 1;
}

static int
array_end_callback (void *context)
{
    Tcl_Interp *interp = (Tcl_Interp *)context;

     append_string (interp, "array_end");
    return 1;
}

