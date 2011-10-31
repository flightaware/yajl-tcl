#!/usr/local/bin/tclsh8.5

package require json
package require yajltcl

set failed 0

proc dotest {raw} {
	set expected [::json::json2dict $raw]
	set actual [::yajl::json2dict $raw]
	puts "Input: $raw"
	puts "Expected: $expected"
	puts "Actual: $actual"
	if {$actual != $expected} {
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


exit $failed
