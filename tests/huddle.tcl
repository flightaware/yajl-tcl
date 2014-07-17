#!/usr/bin/tclsh8.5

load ../libyajltcl1.5.so
source ../yajl.tcl

package require json
package require huddle

set failed 0

proc dotest {json} {
	set json_obj [yajl create #auto -beautify 0]
	set yajlstr [$json_obj parse $json]
	$json_obj delete
	puts -nonewline "YAJL:\t\t"
	puts $yajlstr

	puts -nonewline "HUDDLE:\t\t"
	set huddle [::yajl::json2huddle $json]
	puts $huddle 
	puts "correct huddle: [huddle isHuddle $huddle]"

	#puts -nonewline "DICT:\t\t"
	#puts [::yajl::json2dict $json]
	set json [huddle jsondump $huddle "" ""]

	set json_obj [yajl create #auto -beautify 0]
	set yajlstr2 [$json_obj parse $json]
	$json_obj delete

	if {$yajlstr2 != $yajlstr} {
		puts " *** ERROR ***"
	} else {
		puts "OK"
	}

	puts "----------"
}

#set aa [huddle create 123 577 3453]
#set aa [huddle create a b c d]
#set aa [huddle create a 123 b 123 c 12 ]
puts "--------------"
set aa [huddle list 123 123 12 ]
puts $aa
puts "--------------"

dotest {[123, 577, 3453]}
dotest {{"moo": [123, 4576, 789], "cow": "moooo"}}
dotest {{"moo": [123, 4576, 789], "cow": "moooo", "complex": {"alfa" : "a", "bravo": "b", "charlie" : "c", "delta"  : "d" }}}
dotest {{"moo": "cow", "pig": "oink", "rabbit" : null}}

return

# example from the yajl-tcl README file.
dotest {{
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
}}

# example from "man n json" (tcllib)
dotest {[
		 {
              "precision": "zip",
              "Latitude":  37.7668,
              "Longitude": -122.3959,
              "Address":   "",
              "City":      "SAN FRANCISCO",
              "State":     "CA",
              "Zip":       "94107",
              "Country":   "US"
		 },
		 {
              "precision": "zip",
              "Latitude":  37.371991,
              "Longitude": -122.026020,
              "Address":   "",
              "City":      "SUNNYVALE",
              "State":     "CA",
              "Zip":       "94085",
              "Country":   "US"
		 }
		 ]
}

# example from "man n json" (tcllib)
dotest {{
	"Image": {
               "Width":  800,
               "Height": 600,
               "Title":  "View from 15th Floor",
		"Thumbnail": {
                   "Url":    "http://www.example.com/image/481989943",
                   "Height": 125,
                   "Width":  "100"
		},
		"IDs": [116, 943, 234, 38793]
	}
}}



exit $failed
