#
#
#
#

package require yajltcl

yajl create x -beautify 1

set fp [open foo.json]
set json [read $fp]
close $fp

puts "json: $json"
puts ""

set parsedJson [x parse $json]
puts "parsed json: $parsedJson"

puts ""

eval x $parsedJson
set regenJson [x get]
puts "regenerated json: $regenJson"

