#
# yajl support functions
#
#
#

package require yajltcl

namespace eval ::yajl {

#
# array_to_json - get the key-value pairs out of an array and emit them into
#  a yajl object
#
proc array_to_json {json _array} {
    upvar $_array array

    $json map_open

    foreach key [lsort [array names array]] {
        $json string $key string $array($key)
    }

    $json map_close
}

#
# pgresult_tuples_to_json - get the tuples out of a postgresql result object
#  and generate them into a yajl object as an array of objects of key-value
#  pairs, one object per tuple in the result
#
proc pgresult_tuples_to_json {json res} {
    $json array_open
    set numTuples [pg_result $res -numTuples]
    for {set tuple 0} {$tuple < $numTuples} {incr tuple} {
        unset -nocomplain row
        pg_result $res -tupleArrayWithoutNulls $tuple row
	array_to_json $json row
    }
    $json array_close
}

#
# pg_select_to_json - given a connection handle and a sql select statement,
#  return the corresponding json as an array of objects of tuples
#
proc pg_select_to_json {db sql} {
    set json [yajl create #auto -beautify 1]
    set res [pg_exec $db $sql]
    pgresult_tuples_to_json $json $res
    pg_result $res -clear
    set result [$json get]
    rename $json ""
    return $result
}

} ;# namespace ::yajl
