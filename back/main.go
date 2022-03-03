package main

import (
	"fmt"
)

func main() {
	crossing := NewCrossing()

	/*
	claim1 := cr.AccessClaim(Lane{ SOUTH, WEST })
	claim2 := cr.AccessClaim(Lane{ EAST,  WEST })
	claim2.BeginClaim()

	if claim1.IsBlocked() {
		fmt.Printf("claim IS blocked\n")
	} else {
		fmt.Printf("claim NOT blocked\n")
	}
	*/

	httpSrv := NewHttpServer(&crossing)
	fmt.Println("Listening now at address", httpSrv.Addr);
	httpSrv.ListenAndServe()

	<- crossing.quit

	fmt.Printf("exiting\n")
}
