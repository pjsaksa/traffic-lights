package main

/*
   Served entry points:

   - `/` : `/front/index.html`
   - `/ws` : WebSocket

   Entry points implemented but commented out:

   - "/css" : "/front/css"
   - "/img/*" : "/front/img/*"
*/

import (
	"context"
	"errors"
	"fmt"
	"net/http"
	"time"

	"nhooyr.io/websocket"
)

const (
	MaxContentLength = 16*1024
)

/*
func parseImgUrl(input string) string {
	if strings.HasPrefix(input, "/img/") {
		return "front" + input
	} else {
		return ""
	}
}
*/

type HttpHandler struct {
	crossing *Crossing
	serveMux  http.ServeMux
}

func NewHttpHandler(crossing *Crossing) *HttpHandler {
	handler := &HttpHandler{
		crossing: crossing,
	}

	handler.serveMux.HandleFunc(
		"/",
		func (out http.ResponseWriter, req *http.Request) {
			handler.HandleRequest(out, req)
		})

	return handler
}

func (handler *HttpHandler) newWebSocket(out http.ResponseWriter, req *http.Request) {
	webSocket,err := websocket.Accept(out, req, nil)
	if err != nil {
		fmt.Printf("websocket.Accept: %v\n", err)
		return
	}
	defer webSocket.Close(websocket.StatusInternalError, "internal error")

	//

	err = RunSocket(handler.crossing, req.Context(), webSocket)
	if err == nil ||
		errors.Is(err, context.Canceled) {
		return
	}
	if websocket.CloseStatus(err) == websocket.StatusNormalClosure ||
		websocket.CloseStatus(err) == websocket.StatusGoingAway {
		return
	}

	fmt.Printf("%v", err)
}

func (handler *HttpHandler) HandleRequest(out http.ResponseWriter, req *http.Request) {
	if req.URL.EscapedPath() == "/" {
		http.ServeFile(out, req, "front/index.html")
/*
	} else if req.URL.EscapedPath() == "/css" {
		http.ServeFile(out, req, "front/css")
	} else if newUrl := parseImgUrl(req.URL.EscapedPath())
	newUrl != "" {
		http.ServeFile(out, req, newUrl)
*/
	} else if req.URL.EscapedPath() == "/ws" {
		handler.newWebSocket(out, req)
	} else {
		http.NotFound(out, req)
	}
}

func (handler *HttpHandler) ServeHTTP(out http.ResponseWriter, req *http.Request) {
	handler.serveMux.ServeHTTP(out, req)
}

// ----- HttpServer

func StartHttpServer(cr *Crossing) {
	httpServer := &http.Server{
		Addr:           ":8000",
		Handler:        NewHttpHandler(cr),
		ReadTimeout:    10 * time.Second,
		WriteTimeout:   10 * time.Second,
		MaxHeaderBytes: 1 << 15,
	}

	fmt.Println("Listening at address", httpServer.Addr);

	if err := httpServer.ListenAndServe() ; err != http.ErrServerClosed {
		panic(err.Error())
	}
}
