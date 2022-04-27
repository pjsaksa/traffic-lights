package main

import (
	//"fmt"
	"sort"
	"time"
)

type Crossing struct {
	quit      chan bool
	claimType string
	lanes     map[Lane]struct{}
	claims    map[Lane]Claim
	rounds    int

	browsers  map[*Browser]struct{}
}

func NewCrossing() Crossing {
	cr := Crossing{
		quit: make(chan bool, 1),
		claimType: "daytime",
		lanes: map[Lane]struct{}{
			{ NORTH, EAST  }: struct{}{},
			{ NORTH, SOUTH }: struct{}{},
			{ NORTH, WEST  }: struct{}{},
			{ EAST,  SOUTH }: struct{}{},
			{ EAST,  WEST  }: struct{}{},
			{ EAST,  NORTH }: struct{}{},
			{ SOUTH, WEST  }: struct{}{},
			{ SOUTH, NORTH }: struct{}{},
			{ SOUTH, EAST  }: struct{}{},
			{ WEST,  NORTH }: struct{}{},
			{ WEST,  EAST  }: struct{}{},
			{ WEST,  SOUTH }: struct{}{},
		},
		claims: map[Lane]Claim{},
		browsers: map[*Browser]struct{}{},
	}

	go cr.Run()

	return cr
}

// methods

func (cr *Crossing) AccessClaim(lane Lane) Claim {
	if existingClaim,ok := cr.claims[lane] ; ok {
		return existingClaim
	} else {
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

		return newClaim
	}
}

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

func (cr *Crossing) Lane(name string) *Lane {
	for lane := range cr.lanes {
		if lane.String() == name {
			return &lane
		}
	}
	return nil
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

	for lane := range cr.lanes {
		cl := cr.AccessClaim(lane)
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
	_,ok := cr.lanes[lane]
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
