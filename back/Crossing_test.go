package main

import (
	"fmt"
	"sort"
)

func reportLanePairBlock(cr Crossing, x,y Lane) (string, bool) {
	if x == y {
		return "", false
	}

	if cr.Blocks(x, y) {
		return fmt.Sprintf("(%s x %s): Block", x.String(), y.String()), true
	} else {
		return fmt.Sprintf("(%s x %s): Pass", x.String(), y.String()), true
	}
}

func reportLaneSourceBlock(cr Crossing, src Lane) {

	var lines []string

	for i := range cr.lanes {
		line, ok := reportLanePairBlock(cr, src, i)
		if ok {
			lines = append(lines, line)
		}
	}

	sort.Strings(lines)

	for i := range lines {
		fmt.Println(lines[i])
	}
}

func ExampleLaneSouthWestBlocks() {
	reportLaneSourceBlock(NewCrossing(), Lane{ SOUTH, WEST })

	// Output:
	// (S->W x E->N): Pass
	// (S->W x E->S): Block
	// (S->W x E->W): Block
	// (S->W x N->E): Block
	// (S->W x N->S): Block
	// (S->W x N->W): Block
	// (S->W x S->E): Pass
	// (S->W x S->N): Pass
	// (S->W x W->E): Block
	// (S->W x W->N): Block
	// (S->W x W->S): Pass
}

func ExampleLaneSouthEastBlocks() {
	reportLaneSourceBlock(NewCrossing(), Lane{ SOUTH, EAST })

	// Output:
	// (S->E x E->N): Pass
	// (S->E x E->S): Pass
	// (S->E x E->W): Pass
	// (S->E x N->E): Block
	// (S->E x N->S): Pass
	// (S->E x N->W): Pass
	// (S->E x S->N): Pass
	// (S->E x S->W): Pass
	// (S->E x W->E): Block
	// (S->E x W->N): Pass
	// (S->E x W->S): Pass
}

func ExampleLaneSouthNorthBlocks() {
	reportLaneSourceBlock(NewCrossing(), Lane{ SOUTH, NORTH })

	// Output:
	// (S->N x E->N): Block
	// (S->N x E->S): Block
	// (S->N x E->W): Block
	// (S->N x N->E): Block
	// (S->N x N->S): Pass
	// (S->N x N->W): Pass
	// (S->N x S->E): Pass
	// (S->N x S->W): Pass
	// (S->N x W->E): Block
	// (S->N x W->N): Block
	// (S->N x W->S): Pass
}
