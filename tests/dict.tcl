#!/usr/local/bin/tclsh8.5

package require json
package require yajltcl

set failed 0

proc dotest {raw} {
	#set count 10000
	set count 1

	# do the tcllib method
	set start [clock clicks]
	for {set i 0} {$i < $count} {incr i} {
		set expected [::json::json2dict $raw]
	}
	set end [clock clicks]
	if {$count > 1} {
		puts "tcllib took [expr $end-$start] clicks"
	}

	# do the yajl method.
	set start [clock clicks]
	for {set i 0} {$i < $count} {incr i} {
		set actual [::yajl::json2dict $raw]
	}
	set end [clock clicks]
	if {$count > 1} {
		puts "yajl took [expr $end-$start] clicks"
	}

	# compare the results.
	puts "Expected: $expected"
	puts "Actual: $actual"
	if {$actual != $expected} {
		puts "Input: $raw"
		puts "FAILED"
		incr ::failed
	} else {
		puts "OK"
	}

	puts ""
}

dotest {[123, 577, 3453]}
dotest {{"moo": [123, 4576, 789], "cow": "moooo"}}
dotest {{"moo": [123, 4576, 789], "cow": "moooo", "complex": {"alfa" : "a", "bravo": "b", "charlie" : "c", "delta"  : "d" }}}
dotest {{"moo": "cow", "pig": "oink", "rabbit" : null}}

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
