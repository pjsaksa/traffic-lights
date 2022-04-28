package main

import (
	"fmt"
)

type Pole struct {
	direction Direction
	claims    map[Lane]Claim
}

func NewPole(dir Direction) *Pole {
	return &Pole{
		direction: dir,
		claims:    map[Lane]Claim{},
	}
}

func (pole *Pole) Transfer(dir Direction) {
	var anyClaims bool = false

	line := fmt.Sprintf("Pole %s -> ", dir.String())

	for lane,claim := range pole.claims {
		if claim.IsClaimed() {
			line += fmt.Sprintf("%s", lane.to.String())
			anyClaims = true
		}
	}

	if anyClaims {
		fmt.Println(line)
	}
}
