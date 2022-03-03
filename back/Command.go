package main

import (
	"encoding/json"
	"fmt"
)

// ----- Command

type Command interface {
	handle(user *User)
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

func (cmd MessageCommand) handle(user *User) {
	fmt.Printf("Message: %s\n", cmd.message)
}

// ----- IncCarCommand

type IncCarCommand struct {
	lane string
}

func (cmd IncCarCommand) handle(user *User) {
	if lane := user.crossing.Lane(cmd.lane)
	lane != nil {
		cl := user.crossing.AccessClaim(*lane)
		cl.IncCar()
	} else {
		fmt.Printf("invalid lane in command: %s\n", cmd.lane)
	}
}
