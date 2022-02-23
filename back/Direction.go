package main

type Direction uint8

const (
	LOST Direction = iota
	//
	NORTH
	EAST
	SOUTH
	WEST
)

func (d Direction) String() string {
	switch d {
	case NORTH: return "N"
	case EAST:  return "E"
	case SOUTH: return "S"
	case WEST:  return "W"
	}
	return "Lost"
}

func TurnCW(in Direction) Direction {
	switch in {
	case NORTH: return EAST
	case EAST:  return SOUTH
	case SOUTH: return WEST
	case WEST:  return NORTH
	}
	return LOST
}

func TurnCCW(in Direction) Direction {
	switch in {
	case NORTH: return WEST
	case EAST:  return NORTH
	case SOUTH: return EAST
	case WEST:  return SOUTH
	}
	return LOST
}

func Turn180(in Direction) Direction {
	switch in {
	case NORTH: return SOUTH
	case EAST:  return WEST
	case SOUTH: return NORTH
	case WEST:  return EAST
	}
	return LOST
}
