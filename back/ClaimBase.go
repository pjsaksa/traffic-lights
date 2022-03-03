package main

type ClaimBase struct {
	crossing  *Crossing
	lane       Lane
	isClaimed  bool
}

func (cb *ClaimBase) IsBlocked() bool {
	for l2,c2 := range cb.crossing.claims {
		if cb.lane == l2 {
			continue
		}

		if c2.IsClaimed() && cb.crossing.Blocks(cb.lane, l2) {
			return true
		}
	}

	return false
}

func (cb *ClaimBase) IsClaimed() bool {
	return cb.isClaimed
}

func (cb *ClaimBase) Lane() Lane {
	return cb.lane
}
