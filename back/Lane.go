package main

import (
	"strings"
)

type Lane struct {
	from,to Direction
}

func (lane Lane) String() string {
	return lane.from.String() + "->" + lane.to.String()
}

func (lane Lane) WebString() string {
	return strings.ToLower( lane.from.String() + "-" + lane.to.String() )
}

func (lane Lane) IsStraight() bool {
	return Turn180(lane.from) == lane.to
}

func (lane Lane) IsRight() bool {
	return TurnCCW(lane.from) == lane.to
}

func (lane Lane) IsLeft() bool {
	return TurnCW(lane.from) == lane.to
}
