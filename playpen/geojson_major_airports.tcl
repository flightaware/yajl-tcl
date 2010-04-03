#
# generate geo-JSON for major airports - now with caching
#
proc flightaware_track_geojson_major_airports {} {
    if {[info exists ::majorAirportGeoJson]} {
        return $::majorAirportGeoJson
    }
	set result [huddle list]

	foreach majAirport $::majorAirports {
		lassign [flightaware_airportCodeToNameAndCity $majAirport] name city thisLat thisLon
		if {$thisLat != "" && $thisLon != ""} {
			huddle append result [huddle create type Feature geometry [huddle create type Point coordinates [huddle list $thisLon $thisLat]] properties [huddle create label $majAirport]]
		}
	}

	set ::majorAirportGeoJson [huddle jsondump [huddle create type FeatureCollection features $result] "" ""]
	return $::majorAirportGeoJson
}

