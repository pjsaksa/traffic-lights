package main

import "fmt"

func ExampleTurnCW() {
	fmt.Println(TurnCW(North).String())
	fmt.Println(TurnCW(East).String())
	fmt.Println(TurnCW(South).String())
	fmt.Println(TurnCW(West).String())

	// Output:
	// E
	// S
	// W
	// N
}

func ExampleTurnCCW() {
	fmt.Println(TurnCCW(North).String())
	fmt.Println(TurnCCW(East).String())
	fmt.Println(TurnCCW(South).String())
	fmt.Println(TurnCCW(West).String())

	// Output:
	// W
	// N
	// E
	// S
}

func ExampleTurn180() {
	fmt.Println(Turn180(North).String())
	fmt.Println(Turn180(East).String())
	fmt.Println(Turn180(South).String())
	fmt.Println(Turn180(West).String())

	// Output:
	// S
	// W
	// N
	// E
}
