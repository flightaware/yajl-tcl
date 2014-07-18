#!/usr/local/bin/tclsh8.5


package require yajltcl

set x [yajl create #auto]

$x map_open string type string FeatureCollection string features array_open

proc json_major_airport {obj icao lat lon} {
    $obj map_open string type string Feature string geometry map_open string type string Point string coordinates array_open double $lon double $lat array_close map_close string properties map_open string label string $icao map_close map_close
}

json_major_airport $x KABQ 35.0 -106.0
#json_major_airport $x KABQ 35.0401944 -106.6091944
#json_major_airport $x KBUR 34.206667 -118.3586667

$x array_close map_close

puts [$x get]

