/*
 *
 * Include file for yajl package
 *
 * Copyright (C) 2010 by FlightAware, All Rights Reserved
 *
 * Freely redistributable under the Berkeley copyright, see license.terms
 * for details.
 */

#include <tcl.h>

/* NB - fix the configure script */
#include <yajl/yajl_version.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_gen.h>
#include <yajl/yajl_parse.h>

#if TCL_MAJOR_VERSION == 9
#undef CONST
#define CONST const
#define yajltcl_size Tcl_Size
#else
#define yajltcl_size int
#endif

extern int
yajltcl_yajlObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objvp[]);

#define YAJLTCL_MAX_DEPTH 100

typedef struct yajltcl_clientData
{
    Tcl_Interp *interp;
    yajl_gen genHandle;
    Tcl_DString dString;
    Tcl_DString p2dString;
    int p2dDepth;

    struct my_gen_config {
      int beautify;
      const char *indentString;
    } genConfig;

    yajl_handle parseHandle;
    yajl_handle parse2dictHandle;
    yajl_handle parse2dictexHandle;
    yajl_handle parse2huddleHandle;

    struct my_parser_config {
      int checkUTF8;
      int allowComments;
    } parseConfig;

    Tcl_Command cmdToken;
	int arrayElement[YAJLTCL_MAX_DEPTH];
} yajltcl_clientData;

