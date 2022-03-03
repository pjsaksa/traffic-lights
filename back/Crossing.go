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

	users     map[*User]struct{}
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
		users: map[*User]struct{}{},
	}

	go cr.Run()

	return cr
}

// methods

func (cr *Crossing) AccessClaim(l Lane) Claim {
	if c,ok := cr.claims[l] ; ok {
		return c
	} else {
		var c2 Claim

		switch cr.claimType {
		case "daytime":
			c2 = &DaytimeClaim{
				ClaimBase: ClaimBase{
					crossing: cr,
					lane: l,
				},
			}
		default:
			panic("invalid claim type: " + cr.claimType)
		}

		cr.claims[l] = c2

		return c2
	}
}

func (cr *Crossing) Blocks(x,y Lane) bool {
	cr.Valid(x)
	cr.Valid(y)

	if x.to == y.to {
		return true
	}

	if x.from == y.from {
		return false
	}

	return _blocks2(cr, x, y) || _blocks2(cr, y, x)
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

	if len(cr.users) > 0 {
		report := NewClaimsReport(cr)

		for user := range cr.users {
			user.output <- report
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

func _blocks2(cr *Crossing, x,y Lane) bool {
	if x.IsLeft() {
		return !y.IsRight()
	}

	if x.IsStraight() {
		return TurnCCW(x.from) == y.from ||
			TurnCCW(x.from) == y.to
	}

	return false
}
