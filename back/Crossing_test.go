package main

import (
	"fmt"
	"sort"
)

func reportLanePairBlock(cr Crossing, lane1,lane2 Lane) (string, bool) {
	if lane1 == lane2 {
		return "", false
	}

	if cr.Blocks(lane1, lane2) {
		return fmt.Sprintf("(%s x %s): Block", lane1.String(), lane2.String()), true
	} else {
		return fmt.Sprintf("(%s x %s): Pass", lane1.String(), lane2.String()), true
	}
}

func reportLaneSourceBlock(cr Crossing, lane1 Lane) {

	var outputList []string

	for lane2 := range cr.lanes {
		if output, ok := reportLanePairBlock(cr, lane1, lane2) ; ok {
			outputList = append(outputList, output)
		}
	}

	sort.Strings(outputList)

	for idx := range outputList {
		fmt.Println(outputList[idx])
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
