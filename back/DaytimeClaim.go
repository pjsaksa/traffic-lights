package main

const (
	claim_interval int32 =  5
	pause_interval int32 = 10
)

type DaytimeClaim struct {
	ClaimBase

	ticks int32
	cars  int32
}

func (c *DaytimeClaim) IncTick() {
	c.ticks++

	if c.isClaimed {
		c.cars = 0
	}
}

func (c *DaytimeClaim) IncCar() {
	c.cars++
}

func (c *DaytimeClaim) Weight() int32 {
	if c.isClaimed {
		if c.ticks < claim_interval || c.cars > 0 {
			return MAX_WEIGHT
		} else {
			return NO_WEIGHT
		}
	} else {
		if c.ticks < pause_interval {
			return NO_WEIGHT
		} else {
			return (c.ticks - pause_interval) + (2 * c.cars)
		}
	}
}

func (c *DaytimeClaim) BeginClaim() {
	c.isClaimed = true
}

func (c *DaytimeClaim) EndClaim() {
	c.isClaimed = false
}
