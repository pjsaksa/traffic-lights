package main

type Crossing struct {
	lanes map[Lane]struct{}
}

func NewCrossing() Crossing {
	return Crossing{
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
	}
}

func (cr Crossing) Valid(lane Lane) {
	_,ok := cr.lanes[lane]
	if !ok {
		panic("invalid lane: " + lane.String())
	}
}

func (cr Crossing) Blocks_(x,y Lane) bool {
	if x.IsLeft() {
		return !y.IsRight()
	}

	if x.IsStraight() {
		return TurnCCW(x.from) == y.from ||
			TurnCCW(x.from) == y.to
	}

	return false
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

	return cr.Blocks_(x, y) || cr.Blocks_(y, x)
}
