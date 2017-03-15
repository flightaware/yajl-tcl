#!/bin/sh

# This script simplifies the process of incrementing all version numbers for a new release.

NEWVER="1.6.2"

perl -p -i -e "s/^(AC_INIT\\(\\[[a-z_]+\\],) \\[[0-9.]+\\]/\\1 \\[$NEWVER\\]/" configure.in

perl -p -i -e "s/^(\*Version) [0-9.]+(\*)/\\1 $NEWVER\\2/" README.md

perl -p -i -e "s/^(\\[manpage_begin yajl-tcl n) [0-9.]+(\\])/\\1 $NEWVER\\2/" doc/yajl-tcl.man

autoreconf

make distclean
