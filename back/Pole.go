package main

import (
	"fmt"
)

type Pole struct {
	direction Direction
	claims    map[Lane]Claim
	wire      Wire
}

func NewPole(dir Direction) *Pole {
	return &Pole{
		direction: dir,
		claims:    map[Lane]Claim{},
		wire:      nil,
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

	//

	if pole.wire == nil && dir == NORTH {
		pole.wire = NewWireSpi(1)
	}

	if pole.wire != nil {
		pole.wire.Transfer()
	}
}
