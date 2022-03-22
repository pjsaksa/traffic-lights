package main

func main() {
	crossing := NewCrossing()

	go StartHttpServer(&crossing)

	<- crossing.quit
}
