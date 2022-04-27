package main

const (
	claim_interval int32 = 5
	pause_interval int32 = 3
)

type DaytimeClaim struct {
	ClaimBase

	ticks int32
	cars  int32
}

func (claim *DaytimeClaim) IncTick() {
	claim.ticks++

	if claim.isClaimed {
		claim.cars = 0
	}
}

func (claim *DaytimeClaim) IncCar() {
	claim.cars++
}

func (claim *DaytimeClaim) Weight() int32 {
	if claim.isClaimed {
		if claim.ticks < claim_interval || claim.cars > 0 {
			return MAX_WEIGHT
		} else {
			return NO_WEIGHT
		}
	} else {
		if claim.ticks < pause_interval {
			return NO_WEIGHT
		} else {
			return (claim.ticks - pause_interval) + (2 * claim.cars)
		}
	}
}

func (claim *DaytimeClaim) BeginClaim() {
	claim.isClaimed = true
	claim.ticks = 0
}

func (claim *DaytimeClaim) EndClaim() {
	claim.isClaimed = false
	claim.ticks = 0
}
