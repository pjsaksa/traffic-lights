package main

import (
	"context"
	"fmt"

	"nhooyr.io/websocket"
)

type User struct {
	crossing *Crossing
	input     chan Command
	output    chan Report
}

func NewUser(crossing *Crossing) User {
	return User{
		crossing: crossing,
		input: make(chan Command, 10),
		output: make(chan Report, 10),
	}
}

func RunSocket(crossing *Crossing, ctx context.Context, conn *websocket.Conn) error {
	user := NewUser(crossing)

	fmt.Println("new browser connection")

	crossing.users[&user] = struct{}{}
	defer func() {
		delete(crossing.users, &user)
		fmt.Println("browser disconnected")
	}()

	go user.ParseSocket(ctx, conn)

	user.output <- NewMessageReport("connected to backend")

	for {
		select {
		case cmd := <- user.input:
			if cmd != nil {
				cmd.handle(&user)
			} else {
				return nil
			}
		case rep := <- user.output:
			if err := conn.Write(ctx, websocket.MessageText, []byte(rep.toJson())) ; err != nil {
				return err
			}
		case <- ctx.Done():
			return ctx.Err()
		}
	}
}

func (user *User) ParseSocket(ctx context.Context, conn *websocket.Conn) {
	defer func() {
		user.input <- nil
	}()

	for {
		typ,buf,err := conn.Read(ctx)
		if err == nil && typ == websocket.MessageText {
			if cmd := ParseCommand(buf) ; cmd != nil {
				user.input <- cmd
			}
		} else {
			break
		}
	}
}
