package main

import (
	//"fmt"
	"sort"
	"time"
)

type Crossing struct {
	quit      chan bool
	claimType string
	claims    map[Lane]Claim
	rounds    int

	browsers  map[*Browser]struct{}
}

func NewCrossing() Crossing {
	cr := Crossing{
		quit:      make(chan bool, 1),
		claimType: "daytime",
		claims:    map[Lane]Claim{},
		browsers:  map[*Browser]struct{}{},
	}

	cr._initLane(Lane{ NORTH, EAST  })
	cr._initLane(Lane{ NORTH, SOUTH })
	cr._initLane(Lane{ NORTH, WEST  })
	cr._initLane(Lane{ EAST,  SOUTH })
	cr._initLane(Lane{ EAST,  WEST  })
	cr._initLane(Lane{ EAST,  NORTH })
	cr._initLane(Lane{ SOUTH, WEST  })
	cr._initLane(Lane{ SOUTH, NORTH })
	cr._initLane(Lane{ SOUTH, EAST  })
	cr._initLane(Lane{ WEST,  NORTH })
	cr._initLane(Lane{ WEST,  EAST  })
	cr._initLane(Lane{ WEST,  SOUTH })

	go cr.Run()

	return cr
}

// methods

func (cr *Crossing) Blocks(lane1,lane2 Lane) bool {
	cr.Valid(lane1)
	cr.Valid(lane2)

	if lane1.to == lane2.to {
		return true
	}

	if lane1.from == lane2.from {
		return false
	}

	return _blocks2(cr, lane1, lane2) || _blocks2(cr, lane2, lane1)
}

func (cr *Crossing) Lane(name string) (Lane, bool) {
	for lane := range cr.claims {
		if lane.String() == name {
			return lane,true
		}
	}
	return Lane{},false
}

func (cr *Crossing) Run() {
	defer func() { cr.quit <- true }()

	ticker := time.NewTicker(2 * time.Second)
	defer ticker.Stop()

	//

	for {
		select {
		case <- ticker.C:
			// if len(game.players) == 0 {
			//     return
			// }

			cr.Tick()
		}
	}
}

func sortClaims(claims []Claim) {
	sort.Slice(claims, func(i,j int) bool {
		if claims[i].Weight() != claims[j].Weight() {
			return claims[i].Weight() > claims[j].Weight()
		} else {
			if claims[i].Lane().IsStraight() != claims[j].Lane().IsStraight() {
				return claims[i].Lane().IsStraight()
			} else {
				return claims[i].Lane().from < claims[j].Lane().from
			}
		}
	})
}

func (cr *Crossing) Tick() {
	var claims []Claim

	for _,cl := range cr.claims {
		cl.IncTick()

		claims = append(claims, cl)
	}

	// sort claims primarily by weight

	sortClaims(claims)

	// do claims

	for _,cl := range claims {
		if !cl.IsClaimed() {
			if cl.Weight() > NO_WEIGHT && !cl.IsBlocked() {
				cl.BeginClaim()
			}
		} else {
			if cl.Weight() == NO_WEIGHT {
				cl.EndClaim()
			}
		}
	}

	// report

	if len(cr.browsers) > 0 {
		report := NewClaimsReport(cr)

		for browser := range cr.browsers {
			browser.output <- report
		}
	}

	// print claims (for debug)
	//@@@
	/*
	fmt.Printf("tick\n")

	for _,cl := range claims {
		fmt.Printf("- %s : %s (%4d)\n",
			cl.Lane().String(),
			func(b bool) string {
				if b { return "*" } else { return "." }
			}(cl.IsClaimed()),
			cl.Weight())
	}
	*/
}

func (cr *Crossing) Valid(lane Lane) {
	_,ok := cr.claims[lane]
	if !ok {
		panic("invalid lane: " + lane.String())
	}
}

// private

func _blocks2(cr *Crossing, lane1,lane2 Lane) bool {
	if lane1.IsLeft() {
		return !lane2.IsRight()
	}

	if lane1.IsStraight() {
		return TurnCCW(lane1.from) == lane2.from ||
			TurnCCW(lane1.from) == lane2.to
	}

	return false
}

func (cr *Crossing) _initLane(lane Lane) {
	var newClaim Claim

	switch cr.claimType {
	case "daytime":
		newClaim = &DaytimeClaim{
			ClaimBase: ClaimBase{
				crossing: cr,
				lane: lane,
			},
		}
	default:
		panic("invalid claim type: " + cr.claimType)
	}

	cr.claims[lane] = newClaim
}
