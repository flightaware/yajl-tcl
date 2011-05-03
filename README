This is yajl-tcl, a direct Tcl interface to the yajl JSON generator library.

Version 1.2

This package is a freely available open source package under the "Berkeley"
license, same as Tcl.  You can do virtually anything you like with it, such as 
modifying it, redistributing it, and selling it either in whole or in part.  
See the file "license.terms" for complete information.

yajl-tcl was written by Karl Lehenbauer of FlightAware.  This company-funded
work was generously open-sourced.

USING YAJL-TCL
==============

package require yajltcl


EXAMPLE USE
===========

Create a yajl-tcl object...

yajl create x -beautify 1

or

set x [yajl create #auto]

then generate some json

x map_open string type string FeatureCollection string features array_open 

proc json_major_airport {obj icao lat lon} {
    $obj map_open string type string Feature string geometry map_open string type string Point string coordinates array_open number $lon number $lat array_close map_close string properties map_open string label string $icao map_close map_close
}

json_major_airport x KABQ 35.0401944 -106.6091944
json_major_airport x KBUR 34.206667 -118.3586667 

x array_close map_close

puts [x get]

produces...

{
	"type": "FeatureCollection",
	"features": [
		{
			"type": "Feature",
			"geometry": {
				"type": "Point",
				"coordinates": [
					-106.6091944,
					35.0401944
				]
			},
			"properties": {
				"label": "KABQ"
			}
		},
		{
			"type": "Feature",
			"geometry": {
				"type": "Point",
				"coordinates": [
					-118.3586667,
					34.206667
				]
			},
			"properties": {
				"label": "KBUR"
			}
		}
	]
}

x reset - prepare the object for reuse

or

x delete - delete the object and all of its internal data


YAJL-TCL QUICK REF
==================

create the object as above

invoke the object with one or more methods.  They are invoked left to
right.  Some have no arguments and some have one.

The methods are:

array_open, array_close, bool, clear, double, integer, map_close, map_open, null, number, string, free, get, reset, or delete

array_open - start an array

array_close - end an array

clear - clears the buffer but doesn't reset the parser's state
    (not sure how useful this is)

bool - add a boolean, value follows

double - add a double precision floating point value, value follows

integer - add an integer value, value follows

number - add a numeric value, value follows

Note that with respect to "double" yajl internally formats that only with "%g"
providing six digits of precision and this is not currently configurable 
via YAJL.  If you need higher precision, use "format" or equivalent, coupled 
with yajl-tcl's "number" method.

map_open - open a map

map_close - close a map

null - insert a null value

free - nothing yet

get - get the JSON generated so far.  clears the generator buffer.  maintains
state so you can keep going, i.e. stream it.

reset - free, reallocate and configure the yajl generator object for reuse

delete - delete the object and all of its internal data

PARSING
=======

As of version 1.2, yajl-tcl can also parse...

set list [$object parse $json]

List will be as above.

YAJL LIBRARY ROUTINES
=====================

add_array_to_json jsonObject arrayName - append a Tcl array into the specified
   json object by doing a JSON map open and then storing the array as key-value
   pairs and then doing a map close.

example usage:

    set json [yajl create #auto]
    add_array_to_json $json array
    puts [$json get]
    rename $json ""

array_to_json arrayName - return the contents of the array as JSON text

add_pgresult_tuples_to_json - append a JSON array of JSON objects of a
     Postgres result, one array entry per tuple in the result, with
     the non-null values set in each row object.

pg_select_to_json - given a postgres connection handle and a sql select
statement, perform the select and return the results of the select as
JSON text.

BUGS
====

About for sure.  None known at this time.

CONTENTS
========

Makefile.in	Makefile template.  The configure script uses this file to
		produce the final Makefile.

README		This file

aclocal.m4	Generated file.  Do not edit.  Autoconf uses this as input
		when generating the final configure script.  See "tcl.m4"
		below.

configure	Generated file.  Do not edit.  This must be regenerated
		anytime configure.in or tclconfig/tcl.m4 changes.

configure.in	Configure script template.  Autoconf uses this file as input
		to produce the final configure script.

generic/yajltcl.c	YAJL Tcl interface routines.
generic/yajltcl.h	include file
generic/tclyajltcl.c	Init routines.


tclconfig/	This directory contains various template files that build
		the configure script.  They should not need modification.

	install-sh	Program used for copying binaries and script files
			to their install locations.

	tcl.m4		Collection of Tcl autoconf macros.  Included by
			aclocal.m4 to define SC_* macros.

UNIX BUILD
==========

Building under most UNIX systems is easy, just run the configure script
and then run make. 

	$ cd yajl-tcl
	$ ./configure
	$ make
	$ make install

WINDOWS BUILD
=============

yajl-tcl has not been built under Windows at this time.

The recommended method to build extensions under Windows is to use the
Msys + Mingw build process. This provides a Unix-style build while
generating native Windows binaries. Using the Msys + Mingw build tools
means that you can use the same configure script as per the Unix build
to create a Makefile.

If you have VC++, then you may wish to use the files in the win
subdirectory and build the extension using just VC++. 

Instructions for using the VC++ makefile are written in the first part of 
the Makefile.vc file.

INSTALLATION
============

make install

yajltcl installs like so:

         $exec_prefix
          /       \
        lib       bin
         |         |
   PACKAGEx.y   (dependent .dll files on Windows)
         |
  pkgIndex.tcl (.so|.dll files)

The main .so|.dll library file gets installed in the versioned PACKAGE
directory, which is OK on all platforms because it will be directly
referenced with by 'load' in the pkgIndex.tcl file.  Dependent DLL files on
Windows must go in the bin directory (or other directory on the user's
PATH) in order for them to be found.

Yajl-tcl has not been tested with Windows so none of the above may be true.

