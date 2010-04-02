/*
 *
 * Include file for yajl package
 *
 * Copyright (C) 2010 by FlightAware, All Rights Reserved
 *
 * Freely redistributable under the Berkeley copyright, see license.terms
 * for details.
 */


/* NB - fix the configure script */
#include <yajl/yajl_common.h>
#include <yajl/yajl_gen.h>

extern int
yajltcl_yajlObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objvp[]);

typedef struct yajltcl_clientData
{
    Tcl_Interp *interp;
    yajl_gen handle;
    Tcl_DString dString;
    yajl_gen_config yConfig;
} yajltcl_clientData;
