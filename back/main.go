package main

import (
	"fmt"
)

func testOneBlock(cr Crossing, x,y Lane) {
	if x == y {
		return
	}

	if cr.Blocks(x, y) {
		fmt.Printf("(%s x %s): BLOCK\n", x.String(), y.String())
	} else {
		fmt.Printf("(%s x %s): Ok\n", x.String(), y.String())
	}
}

func testAllBlocks(cr Crossing) {
	for i := range cr.lanes {
		testOneBlock(cr, Lane{ SOUTH, WEST }, i)
	}
}

func main() {
	cr := NewCrossing()

	testAllBlocks(cr)
}
