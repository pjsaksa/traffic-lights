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

// ----- URL parsing

/*
func parseImgUrl(input string) string {
	if strings.HasPrefix(input, "/img/") {
		return "front" + input
	} else {
		return ""
	}
}
*/

// ----- GameServer

type HttpHandler struct {
	crossing *Crossing
	serveMux  http.ServeMux
}

func NewHttpHandler(crossing *Crossing) *HttpHandler {
	hh := &HttpHandler{
		crossing: crossing,
	}

	hh.serveMux.HandleFunc(
		"/",
		func (out http.ResponseWriter, req *http.Request) {
			hh.HandleRequest(out, req)
		})

	return hh
}

func (hh *HttpHandler) newWebSocket(out http.ResponseWriter, req *http.Request) {
	ws, err := websocket.Accept(out, req, nil)
	if err != nil {
		fmt.Printf("websocket.Accept: %v\n", err)
		return
	}
	defer ws.Close(websocket.StatusInternalError, "internal error")

	//

	err = RunSocket(hh.crossing, req.Context(), ws)
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

func (gs *HttpHandler) HandleRequest(out http.ResponseWriter, req *http.Request) {
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
		gs.newWebSocket(out, req)
	} else {
		http.NotFound(out, req)
	}
}

func (gs *HttpHandler) ServeHTTP(out http.ResponseWriter, req *http.Request) {
	gs.serveMux.ServeHTTP(out, req)
}

// ----- HttpServer

func StartHttpServer(crossing *Crossing) {
	httpSrv := &http.Server{
		Addr:           ":8000",
		Handler:        NewHttpHandler(crossing),
		ReadTimeout:    10 * time.Second,
		WriteTimeout:   10 * time.Second,
		MaxHeaderBytes: 1 << 15,
	}

	fmt.Println("Listening at address", httpSrv.Addr);

	if err := httpSrv.ListenAndServe() ; err != http.ErrServerClosed {
		panic(err.Error())
	}
}
