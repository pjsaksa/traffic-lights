package main

type Crossing struct {
	claimType string
	lanes     map[Lane]struct{}
	claims    map[Lane]Claim
}

func NewCrossing() Crossing {
	return Crossing{
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
	}
}

// methods

func (cr Crossing) AccessClaim(l Lane) Claim {
	if c,ok := cr.claims[l] ; ok {
		return c
	} else {
		var c2 Claim

		switch cr.claimType {
		case "daytime":
			c2 = &DaytimeClaim{
				ClaimBase: ClaimBase{
					crossing: &cr,
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

func (cr Crossing) Blocks(x,y Lane) bool {
	cr.Valid(x)
	cr.Valid(y)

	if x.to == y.to {
		return true
	}

	if x.from == y.from {
		return false
	}

	return blocks2(cr, x, y) || blocks2(cr, y, x)
}

func (cr Crossing) Valid(lane Lane) {
	_,ok := cr.lanes[lane]
	if !ok {
		panic("invalid lane: " + lane.String())
	}
}

// private functions

func blocks2(cr Crossing, x,y Lane) bool {
	if x.IsLeft() {
		return !y.IsRight()
	}

	if x.IsStraight() {
		return TurnCCW(x.from) == y.from ||
			TurnCCW(x.from) == y.to
	}

	return false
}
