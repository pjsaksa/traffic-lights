package main

const (
	NO_WEIGHT  = 0
	MAX_WEIGHT = 1000
)

type Claim interface {
	IncTick()
	IncCar()
	Weight() int32

	IsClaimed() bool
	IsBlocked() bool
	Lane() Lane

	BeginClaim()
	EndClaim()
}
