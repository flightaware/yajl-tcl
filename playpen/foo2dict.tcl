#
#
#
#

package require yajltcl

yajl create x 

set fp [open foo.json]
set json [read $fp]
close $fp

puts "json: $json"
puts ""

set parsedJson [x parse $json]
puts "straight parse: $parsedJson"
puts ""

set parse2dictJson [x parse2dict $json]
puts "as dict: $parse2dictJson"

puts ""

