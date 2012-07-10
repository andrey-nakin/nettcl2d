###############################################################################
# nettcl2d.tcl --
#
#    Basic nettcl2d operations
#
###############################################################################

package provide nettcl2d 1.0.0

package require math::statistics 0.5
package require cmdline

namespace eval nettcl2d {
}

# Checks whether program version is greater or equal to given one
# If version checking fails, error is thrown
# Arguments
#   verRequired - version expected, parts are separated by dots, e.g. 1.2.3
proc nettcl2d::matchVersion { verRequired } {
  # ensure nettcl2d commands are available
  if { [info command ::nettcl2d::version] == "" } {
    error "nettcl2d commands are not available"
  }

  # retrieve current vdiff1d version
  set ver [ nettcl2d::version as-list]

  # split given version by parts
  set expected [ split "$verRequired" . ]
  for { set i 0 } { $i < [ llength $expected ] } { incr i } {
    set n [ lindex $expected $i ]
    if { $n > 0 && ( [ llength $ver ] < $i || [ lindex $ver $i ] < $n ) } {
      error "Version mismatch: current version is [nettcl2d::version] but $verRequired expected"
    }
    if { [ lindex $ver $i ] > $n } {
      break
    }
  }
}

# Creates and returns network instance
proc nettcl2d::makeGrid2d { args } {
	set opts {
		{rows.arg		25		"number of rows"}
		{columns.arg	25		"number of columns"}
		{betaAvg.arg	""		"Beta average value"}
		{tauAvg.arg		""		"Tau average value"}
		{vAvg.arg		""		"V average value"}
		{delta.arg		""		"Parameter scattering"}
		{xyDelta.arg	""		"Grid scattering"}
		{xRng.arg		""		"Grid x rng"}
		{yRng.arg		""		"Grid y rng"}
		{seed.arg		12345	"Random generator seed"}
	}

	set usage ": make2dGrid \[options]\noptions:"
	array set options [::cmdline::getoptions args $opts $usage]

	set seedGen [nettcl2d::rng create uniform 10000.0 20000.0]
	nettcl2d::rng seed $seedGen [expr int($options(seed))]
	
	if { $options(xRng) != "" } {
		set xRng $options(xRng)
	} else {
		set xRng [nettcl2d::rng create uniform 1.0 $options(xyDelta)]
		nettcl2d::rng seed $xRng [expr int([nettcl2d::rng next $seedGen])]
	}
	if { $options(yRng) != "" } {
		set yRng $options(yRng)
	} else {
		set yRng [nettcl2d::rng create uniform 1.0 $options(xyDelta)]
		nettcl2d::rng seed $yRng [expr int([nettcl2d::rng next $seedGen])]
	}

	set betaRng [nettcl2d::rng create uniform $options(betaAvg) [expr $options(delta) * $options(betaAvg)]]
	nettcl2d::rng seed $betaRng [expr int([nettcl2d::rng next $seedGen])]
	set tauRng [nettcl2d::rng create uniform $options(tauAvg) [expr $options(delta) * $options(tauAvg)]]
	nettcl2d::rng seed $tauRng [expr int([nettcl2d::rng next $seedGen])]
	set vRng [nettcl2d::rng create uniform $options(vAvg) [expr $options(delta) * $options(vAvg)]]
	nettcl2d::rng seed $vRng [expr int([nettcl2d::rng next $seedGen])]
	
	set populator [::nettcl2d::populator create grid2d	\
		$options(columns) $options(rows) \
		$xRng $yRng	\
		$betaRng $tauRng $vRng	\
	]
	return [::nettcl2d::network create $populator]
}

# Creates and returns tracer instance
proc nettcl2d::makeTracer { args } {
	set opts {
		{interval.arg	0.0		"Tracing interval"}
		{startTime.arg	0.0		"Start time for tracing"}
		{precision.arg	6		"Number precision"}
		{fileName.arg	""		"Trace file name"}
		{tagExpr.arg	""		"Tag expression"}
		{indices.arg	""		"Element indices"}
	}

	set usage ": makeTracer \[options] type\noptions:"
	array set options [::cmdline::getoptions args $opts $usage]
	lassign $args type
	
	switch -exact -- $type {
	    null {
	        return [nettcl2d::tracer create null]
        }
       
	    avg-voltage {
	        return [nettcl2d::tracer create avg-voltage \
	            $options(fileName)  \
	            $options(interval)  \
	            $options(startTime)  \
	            $options(precision)  \
	            $options(tagExpr)  \
            ]
        }
       
        voltage {
	        return [nettcl2d::tracer create voltage \
	            $options(fileName)  \
	            $options(interval)  \
	            $options(startTime)  \
	            $options(precision)  \
	            $options(indices)  \
            ]
        }
       
	    avg-flux {
	        return [nettcl2d::tracer create avg-flux \
	            $options(fileName)  \
	            $options(interval)  \
	            $options(startTime)  \
	            $options(precision)  \
	            $options(tagExpr)  \
            ]
        }
        
        flux {
	        return [nettcl2d::tracer create flux \
	            $options(fileName)  \
	            $options(interval)  \
	            $options(startTime)  \
	            $options(precision)  \
	            $options(indices)  \
            ]
        }
        
        default {
            error "Invalid tracer type $type"
        }
    }
}
