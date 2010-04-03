
package require flightaware
package require flightaware-track
package require flightaware-db
package require yajltcl

#
# generate geo-JSON for major airports - now with caching
#
proc flightaware_track_geojson_major_airports {} {
    yajl create yajo

    yajo map_open string type string FeatureCollection string features array_open

    foreach majAirport $::majorAirports {
	lassign [flightaware_airportCodeToNameAndCity $majAirport] name city thisLat thisLon
	if {$thisLat != "" && $thisLon != ""} {
	    yajo map_open string type string Feature string geometry map_open string type string Point string coordinates array_open number $thisLon number $thisLat array_close map_close string properties map_open string label string $majAirport map_close map_close
	}
    }

    yajo array_close map_close

    set ::majorAirportGeoJson [yajo get]
    catch {rename yajo ""}
    return $::majorAirportGeoJson
}


flightaware_track_geojson_major_airports
