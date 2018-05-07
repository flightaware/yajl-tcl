#
# yajl support functions
#
#
#

package require yajltcl

namespace eval ::yajl {


#
# json2dict - parse json and return a key-value list suitable for
# loading into a dict or an array. This is usually a friendlier
# format to parse than the direct output of the "get" method.
# (inspired and named after the tcllib proc ::json::json2dict)
#
proc json2dict {jsonText args} {
	set obj [yajl create #auto {*}$args]
	set result [$obj parse2dict $jsonText]
	$obj delete
	return $result
}

#
# json2dict_ex - parse json and return a key-value list suitable for
# loading into a dict or an array. This is usually a friendlier
# format to parse than the direct output of the "get" method.
# (inspired and named after the tcllib proc ::json::json2dict)
#
# This version enumerates all arrays as key-value pairs with the
# array index being an incrementing integer starting from 0 for
# the first element.
#
proc json2dict_ex {jsonText args} {
	set obj [yajl create #auto {*}$args]
	set result [$obj parse2dict_ex $jsonText]
	$obj delete
	return $result
}


proc json2huddle {jsonText args} {
	set obj [yajl create #auto {*}$args]
	set result [$obj parse2huddle $jsonText]
	$obj delete
	return "HUDDLE $result"
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
	$json delete
	return $result
}

#
# json_to_array - convert json to an array,
#   json must basically be one layer of map,
#   like '{"type":"user_typing","channel":"C15S6609K","user":"U15S0297G"}'
#   or if there's more json hierarchy beneath
#   a top-level map, that's what you get as
#   values in your array
#
proc json_to_array {json _array} {
	upvar $_array array

	set yajl [yajl create #auto]
	array set array [$yajl parse2dict $json]
	$yajl delete
	return
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
	$json delete
	return $result
}

} ;# namespace ::yajl
