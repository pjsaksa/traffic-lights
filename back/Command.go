package main

import (
	"encoding/json"
	"fmt"
)

// ----- Command

type Command interface {
	handle(browser *Browser)
}

func ParseCommand(byteData []byte) (output Command) {
	defer func() {
		if recover() != nil {
			output = nil
		}
	}()

	var root interface{}

	if json.Unmarshal(byteData, &root) == nil {
		rootObj := root.(map[string]interface{})

		cmd := rootObj["command"].(string)
		data := rootObj["data"].(map[string]interface{})

		switch (cmd) {
		case "message":
			// {
			//   "command": "message",
			//   "data": {
			//     "message": "Hello!"
			//   }
			// }

			return MessageCommand{
				message: data["message"].(string),
			}

		case "inccar":
			// {
			//   "command": "inccar",
			//   "data": {
			//     "lane": "N->E",
			//   }
			// }

			return IncCarCommand{
				lane: data["lane"].(string),
			}
		}
	}

	return nil
}

// ----- MessageCommand

type MessageCommand struct {
	message string
}

func (cmd MessageCommand) handle(browser *Browser) {
	fmt.Printf("Message: %s\n", cmd.message)
}

// ----- IncCarCommand

type IncCarCommand struct {
	lane string
}

func (cmd IncCarCommand) handle(browser *Browser) {
	if lane,ok := browser.crossing.Lane(cmd.lane) ; ok {
		if claim,ok := browser.crossing.claims[lane] ; ok {
			claim.IncCar()
			return
		}
	}

	fmt.Printf("invalid lane in command: %s\n", cmd.lane)
}
