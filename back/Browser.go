package main

import (
	"context"
	"fmt"

	"nhooyr.io/websocket"
)

type Browser struct {
	crossing *Crossing
	input     chan Command
	output    chan Report
}

func NewBrowser(crossing *Crossing) Browser {
	return Browser{
		crossing: crossing,
		input:    make(chan Command, 10),
		output:   make(chan Report, 10),
	}
}

func RunSocket(crossing *Crossing, ctx context.Context, conn *websocket.Conn) error {
	browser := NewBrowser(crossing)

	fmt.Println("new browser connection")

	crossing.browsers[&browser] = struct{}{}
	defer func() {
		delete(crossing.browsers, &browser)
		fmt.Println("browser disconnected")
	}()

	go browser.ParseSocket(ctx, conn)

	browser.output <- NewMessageReport("connected to backend")

	for {
		select {
		case cmd := <- browser.input:
			if cmd != nil {
				cmd.handle(&browser)
			} else {
				return nil
			}
		case report := <- browser.output:
			if err := conn.Write(ctx, websocket.MessageText, []byte(report.toJson())) ; err != nil {
				return err
			}
		case <- ctx.Done():
			return ctx.Err()
		}
	}
}

func (browser *Browser) ParseSocket(ctx context.Context, conn *websocket.Conn) {
	defer func() {
		browser.input <- nil
	}()

	for {
		dataType,data,err := conn.Read(ctx)
		if err == nil && dataType == websocket.MessageText {
			if cmd := ParseCommand(data) ; cmd != nil {
				browser.input <- cmd
			}
		} else {
			break
		}
	}
}
