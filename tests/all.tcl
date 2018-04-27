#!/usr/local/bin/tclsh8.5

package require tcltest


# Hook to determine if any of the tests failed. Then we can exit with
# proper exit code: 0=all passed, 1=one or more failed
proc tcltest::cleanupTestsHook {} {
	variable numTests
	set ::exitCode [expr {$numTests(Failed) > 0}]
}

::tcltest::configure -testdir [file dirname [file normalize [info script]]]
::tcltest::configure {*}$argv
::tcltest::runAllTests


if {$exitCode == 1} {
	puts "====== FAIL ====="
	exit $exitCode
} else {
	puts "====== SUCCESS ====="
}
