#
# yajl support functions
#
#
#

package require yajltcl

namespace eval ::yajl {

# helper method used by json2dict
proc yajl_array_to_list {_yajl} {
	upvar $_yajl yajl

	set result {}
	while {$yajl != ""} {
		set first [lindex $yajl 0]
		set yajl [lrange $yajl 1 end]
		switch -exact $first {
			"array_close" {
				return $result
			}
			"integer" -
			"bool" -
			"double" -
			"number" -
			"string" {
				set val [lindex $yajl 0]
				set yajl [lrange $yajl 1 end]
				lappend result $val
			}
			"null" {
				lappend result "null"
			}
			"array_open" {
				# nested array
				lappend result [yajl_array_to_list yajl]
			}
			"map_open" {
				# nested map
				lappend result [yajl_map_to_list yajl]
			}
			default {
				error "unexpected yajl tag: $first"
			}
		}
	}
	error "reached end of yajl without finding array_close"
}

# helper method used by json2dict
proc yajl_map_to_list {_yajl} {
	upvar $_yajl yajl
	set result {}
	while {$yajl != ""} {
		set first [lindex $yajl 0]
		set yajl [lrange $yajl 1 end]
		switch -exact $first {
			"map_close" {
				if {[llength $result] % 2 != 0} {
					error "yajl map was not an even number of elements"
				}
				return $result
			}
			"map_key" -
			"integer" -
			"bool" -
			"double" -
			"number" -
			"string" {
				set val [lindex $yajl 0]
				set yajl [lrange $yajl 1 end]
				lappend result $val
			}
			"null" {
				lappend result "null"
			}
			"array_open" {
				# nested array
				lappend result [yajl_array_to_list yajl]
			}
			"map_open" {
				# nested map
				lappend result [yajl_map_to_list yajl]
			}
			default {
				error "unexpected yajl tag: $first"
			}
		}
	}
	error "reached end of yajl without finding map_close"
}

# helper method used by json2dict
proc yajl_atom_to_list {_yajl} {
	upvar $_yajl yajl

	set result {}
	set level 0
	
	if {$yajl != ""} {
		set first [lindex $yajl 0]
		set yajl [lrange $yajl 1 end]
		switch -exact $first {
			"integer" -
			"bool" -
			"double" -
			"number" -
			"string" {
				set val [lindex $yajl 0]
				set yajl [lrange $yajl 1 end]
				lappend result $val
			}
			"null" {
				lappend result "null"
			}
			"map_open" {
				lappend result [yajl_map_to_list yajl]
			}
			"array_open" {
				lappend result [yajl_array_to_list yajl]
			}
			default {
				error "unexpected yajl tag: $first"
			}
		}
		if {$yajl != ""} {
			error "leftover yajl tags: $yajl"
		}
	}

	return $result
}

#
# json2dict - parse json and return a key-value list suitable for
# loading into a dict or an array.  This is usually a friendlier
# format to parse than the direct output of the "get" method.
# (inspired and named after the tcllib proc ::json::json2dict)
#
proc json2dict {jsonText} {
	set obj [yajl create #auto]
	set yajl [$obj parse $jsonText]
	set result [yajl_atom_to_list yajl]
	$obj delete
	return {*}$result
}


#
# add_array_to_json - get the key-value pairs out of an array and add them into
#  a yajltcl object
#
proc add_array_to_json {json _array} {
    upvar $_array array

    $json map_open

    foreach key [lsort [array names array]] {
        $json string $key string $array($key)
    }

    $json map_close
}

#
# array_to_json - convert an array to json
#
proc array_to_json {_array} {
    upvar $_array array

    set json [yajl create #auto -beautify 1]
    add_array_to_json $json array
    set result [$json get]
    rename $json ""
    return $result
}

#
# add_pgresult_tuples_to_json - get the tuples out of a postgresql result object
#  and generate them into a yajl object as an array of objects of key-value
#  pairs, one object per tuple in the result
#
proc add_pgresult_tuples_to_json {json res} {
    $json array_open
    set numTuples [pg_result $res -numTuples]
    for {set tuple 0} {$tuple < $numTuples} {incr tuple} {
        unset -nocomplain row
        pg_result $res -tupleArrayWithoutNulls $tuple row
        add_array_to_json $json row
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
    add_pgresult_tuples_to_json $json $res
    pg_result $res -clear
    set result [$json get]
    rename $json ""
    return $result
}

} ;# namespace ::yajl
