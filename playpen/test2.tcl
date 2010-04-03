

source airports.tcl

package require huddle
package require yajltcl

proc huddle_school {howMany} {
    set airport_json [huddle list]

    foreach ap [lrange [array names ::airport] 0 $howMany] {
        lassign $::airport($ap) lat lon
	huddle append airport_json [huddle create type Feature geometry [huddle create type Point coordinates [huddle list $lat $lon]] properties [huddle create label $ap]]
    }

    set json [huddle jsondump [huddle create type FeatureCollection features $airport_json]]
    #puts $json
}

proc new_school {howMany} {
    yajl create y

    y map_open string type string FeatureCollection string features array_open

    foreach ap [lrange [array names ::airport] 0 $howMany] {
        lassign $::airport($ap) lat lon
	y map_open string type string feature string geometry map_open string type string Point string coordinates array_open number $lat number $lon array_close map_close string properties map_open string label string $ap map_close map_close
    }

    y array_close map_close

    set json [y get]
    #puts $json

}

set howMany 10000

puts [time "huddle_school $howMany" 5]

puts [time "new_school $howMany" 5]
