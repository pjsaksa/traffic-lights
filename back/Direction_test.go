package main

import "fmt"

func ExampleTurnCW() {
	fmt.Println(TurnCW(NORTH).String())
	fmt.Println(TurnCW(EAST).String())
	fmt.Println(TurnCW(SOUTH).String())
	fmt.Println(TurnCW(WEST).String())

	// Output:
	// E
	// S
	// W
	// N
}

func ExampleTurnCCW() {
	fmt.Println(TurnCCW(NORTH).String())
	fmt.Println(TurnCCW(EAST).String())
	fmt.Println(TurnCCW(SOUTH).String())
	fmt.Println(TurnCCW(WEST).String())

	// Output:
	// W
	// N
	// E
	// S
}

func ExampleTurn180() {
	fmt.Println(Turn180(NORTH).String())
	fmt.Println(Turn180(EAST).String())
	fmt.Println(Turn180(SOUTH).String())
	fmt.Println(Turn180(WEST).String())

	// Output:
	// S
	// W
	// N
	// E
}
