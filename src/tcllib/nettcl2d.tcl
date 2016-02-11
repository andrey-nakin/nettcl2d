###############################################################################
# nettcl2d.tcl --
#
#    Basic nettcl2d operations
#
###############################################################################

package provide nettcl2d 1.0.0

package require Tcl 8.5
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
		{tagExpr1.arg	""		"Tag expression #1"}
		{tagExpr2.arg	""		"Tag expression #2"}
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
	            $options(tagExpr)  \
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
	            $options(tagExpr)  \
            ]
        }
        
        phase {
	        return [nettcl2d::tracer create phase \
	            $options(fileName)  \
	            $options(interval)  \
	            $options(startTime)  \
	            $options(precision)  \
	            $options(tagExpr)  \
            ]
        }

        phase-diff {
	        return [nettcl2d::tracer create phase-diff \
	            $options(fileName)  \
	            $options(interval)  \
	            $options(startTime)  \
	            $options(precision)  \
	            $options(tagExpr1)  \
	            $options(tagExpr2)  \
            ]
        }

        default {
            error "Invalid tracer type $type"
        }
    }
}

proc nettcl2d::foreachContact { varName network tagExpr expression } {
	upvar $varName var
	foreach var [nettcl2d::network get $network contacts $tagExpr] {
		uplevel $expression
	}
}

proc nettcl2d::foreachCircuit { varName network tagExpr expression } {
	upvar $varName var
	foreach var [nettcl2d::network get $network circuits $tagExpr] {
		uplevel $expression
	}
}

proc nettcl2d::single-circuit { network tagExpr { raiseError 1 } } {
    set cs [nettcl2d::network get $network circuits $tagExpr]
    if { [llength $cs] == 0 } {
        if { $raiseError } {
            error "No circuits with tag expression {$tagExpr}"
        } else {
            return ""
        }
    }
    if { [llength $cs] > 1 } {
        if { $raiseError } {
            error "More than one circuit with tag expression {$tagExpr}"
        } else {
            return ""
        }
    }
    return [lindex $cs 0]
}

# Assigns pseudo-random numeric value to a given property of contacts
# Values are uniformly distributed in range [mean - scattering/2, mean + scattering/2)
# Arguments
#   network - network to affect
#   propName - name of contact's property to set, e.g. v
#   tagExpr - contact search expression
#   mean - mean value
#   scattering - value scattering 
#   seed - optional seed value forrandom number generator
proc nettcl2d::setRandomContactProp { network propName tagExpr mean scattering { seed 12345 } } {
	set rng [nettcl2d::rng create uniform $mean $scattering]
	nettcl2d::rng seed $rng [expr { int($seed) }]
	nettcl2d::foreachContact c $network $tagExpr {
		nettcl2d::contact set $c $propName [nettcl2d::rng next $rng]
	}
}

# Calculates summary flux in network
# Arguments
#   network - network
#   tagExpr - optional tag expression to query circuits
# Return
#   summary flux or "" of none circuits match tag expression
proc nettcl2d::summaryFlux { network { tagExpr "" } } {
    set sum ""
    
    foreachCircuit c $network $tagExpr {
        set flux [nettcl2d::circuit get $c flux]
        if { $sum == "" } {
            set sum 0.0
        }
        set sum [expr { $sum + $flux }]
    }

    return $sum
}

# Calculates rectangular circuit dimensions
# Arguments
#   network - network
# Return
#  list of following values:
#    minX maxX minY maxY
proc nettcl2d::circuit-xy-dimensions { network } {

    proc get-min { accum value } {
        if { $value != "" } {
            if { $accum == "" || $accum > $value } {
                set accum $value
            }
        }
        return $accum
    }

    proc get-max { accum value } {
        if { $value != "" } {
            if { $accum == "" || $accum < $value } {
                set accum $value
            }
        }
        return $accum
    }

    set minX ""
    set maxX ""
    set minY ""
    set maxY ""

    foreachCircuit circuit $network {} {
        set x [nettcl2d::circuit get-prop $circuit x]
        set minX [get-min $minX $x]
        set maxX [get-max $maxX $x]

        set y [nettcl2d::circuit get-prop $circuit y]
        set minY [get-min $minY $y]
        set maxY [get-max $maxY $y]
    }

    return [list $minX $maxX $minY $maxY]
}

# Writes circuit fluxes to file
# Arguments
#   network - network
#   fileName - file name
#   fileformat - file format. POssible values:
#     - map (default) - gnuplot's splot compatible
#     - slices - gnuplot's sliced splot compatible
proc nettcl2d::write-xy-fluxes { network fileName { fileFormat map } } {

    # validate format
    if { $fileFormat != "slices" } {
        set fileFormat map
    }

    # determine dimensions
    lassign [circuit-xy-dimensions $network] minx maxx miny maxy

    set f [open $fileName w]
    puts $f "# X\tY\tFlux"

    for { set y $miny } { $y <= $maxy } { incr y } {

        for { set x $minx } { $x <= $maxx } { incr x } {
            set circuit [nettcl2d::single-circuit $network "x=$x & y=$y" 0]
            if { ![nettcl2d::circuit exists $circuit] } {
                set flux 0.0
            } else {
                set flux [nettcl2d::circuit get $circuit flux]
            }

            switch $fileFormat {
                map {
                    puts $f "$x\t$y\t$flux"
                }
                slices {
                    puts $f "$x\t$y\t$flux"
                    puts $f "$x\t$y\t0.0"
                    puts $f ""
                }
            }
        }

        switch $fileFormat {
            map {
                puts $f ""
            }
            slices {
                puts $f "\n"
            }
        }
    }

    close $f
}
