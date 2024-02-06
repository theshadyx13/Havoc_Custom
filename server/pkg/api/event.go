package api

import (
	"Havoc/pkg/logger"
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
)

// event is a websocket endpoint that
// allows a client to listen for incoming
// events (registered agent, operator connect, etc.)
func (api *ServerApi) event(ctx *gin.Context) {
	var (
		upgrade websocket.Upgrader
		socket  *websocket.Conn
		err     error
	)

	// upgrade this request to a websocket
	if socket, err = upgrade.Upgrade(ctx.Writer, ctx.Request, nil); err != nil {
		logger.DebugError("Failed upgrading request: " + err.Error())
		return
	}

	// handle socket channel.
	// check if the token is available
	go api.handleEventClient(socket)
}
