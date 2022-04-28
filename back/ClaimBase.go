package main

type ClaimBase struct {
	crossing  *Crossing
	lane       Lane
	pole      *Pole
	isClaimed  bool
}

func (claim1 *ClaimBase) IsBlocked() bool {
	for lane2,claim2 := range claim1.crossing.claims {
		if claim1.lane == lane2 {
			continue
		}

		if claim2.IsClaimed() && claim1.crossing.Blocks(claim1.lane, lane2) {
			return true
		}
	}

	return false
}

func (claim1 *ClaimBase) IsClaimed() bool {
	return claim1.isClaimed
}

func (claim1 *ClaimBase) Lane() Lane {
	return claim1.lane
}
