package main

type Direction uint8

const (
	Lost Direction = iota
	//
	North
	East
	South
	West
)

func (d Direction) String() string {
	switch d {
	case North: return "N"
	case East:  return "E"
	case South: return "S"
	case West:  return "W"
	}
	return "Lost"
}

func TurnCW(in Direction) Direction {
	switch in {
	case North: return East
	case East:  return South
	case South: return West
	case West:  return North
	}
	return Lost
}

func TurnCCW(in Direction) Direction {
	switch in {
	case North: return West
	case East:  return North
	case South: return East
	case West:  return South
	}
	return Lost
}

func Turn180(in Direction) Direction {
	switch in {
	case North: return South
	case East:  return West
	case South: return North
	case West:  return East
	}
	return Lost
}
