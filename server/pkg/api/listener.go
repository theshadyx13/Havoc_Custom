package api

import (
	"Havoc/pkg/logger"
	"encoding/json"
	"fmt"
	"github.com/gin-gonic/gin"
	"io"
	"net/http"
)

func (api *ServerApi) listenerStart(ctx *gin.Context) {
	var (
		body     []byte
		err      error
		listener map[string]any
		name     string
		protocol string
		options  map[string]any
	)

	if !api.sanityCheck(ctx) {
		ctx.AbortWithStatus(http.StatusUnauthorized)
		return
	}

	// read from request the login data
	if body, err = io.ReadAll(io.LimitReader(ctx.Request.Body, ApiMaxRequestRead)); err != nil {
		logger.DebugError("Failed to read from server api login request: " + err.Error())
		goto ERROR
	}

	logger.Debug("got request on /api/listener/start:" + fmt.Sprintf("%s", string(body)))

	// unmarshal the bytes into a map
	if err = json.Unmarshal(body, &listener); err != nil {
		logger.DebugError("Failed to unmarshal bytes to map: " + err.Error())
		return
	}

	// get name from listener start request
	switch listener["name"].(type) {
	case string:
		name = listener["name"].(string)
	default:
		logger.DebugError("Failed retrieve name: invalid type")
		goto ERROR
	}

	// get protocol from listener start request
	switch listener["protocol"].(type) {
	case string:
		protocol = listener["protocol"].(string)
	default:
		logger.DebugError("Failed retrieve protocol: invalid type")
		goto ERROR
	}

	// get options from listener start request
	switch listener["data"].(type) {
	case map[string]any:
		options = listener["data"].(map[string]any)
	default:
		logger.DebugError("Failed retrieve data: invalid type")
		goto ERROR
	}

	if err = api.teamserver.ListenerStart(name, protocol, options); err != nil {
		ctx.JSON(http.StatusInternalServerError, gin.H{
			"error": err.Error(),
		})
		return
	}

	ctx.AbortWithStatus(http.StatusOK)
	return

ERROR:
	ctx.AbortWithStatus(http.StatusInternalServerError)
}

func (api *ServerApi) listenerStop(ctx *gin.Context) {

	if !api.sanityCheck(ctx) {
		ctx.AbortWithStatus(http.StatusUnauthorized)
		return
	}
}

func (api *ServerApi) listenerEdit(ctx *gin.Context) {

	if !api.sanityCheck(ctx) {
		ctx.AbortWithStatus(http.StatusUnauthorized)
		return
	}
}

func (api *ServerApi) listenerEvent(ctx *gin.Context) {
	var (
		body     []byte
		err      error
		event    map[string]any
		protocol string
	)

	if !api.sanityCheck(ctx) {
		ctx.AbortWithStatus(http.StatusUnauthorized)
		return
	}

	// read from request the event data
	if body, err = io.ReadAll(io.LimitReader(ctx.Request.Body, ApiMaxRequestRead)); err != nil {
		logger.DebugError("Failed to read from server api login request: " + err.Error())
		goto ERROR
	}

	logger.Debug("got request on /api/listener/event:" + fmt.Sprintf("%s", string(body)))

	// unmarshal the bytes into a map
	if err = json.Unmarshal(body, &event); err != nil {
		logger.DebugError("Failed to unmarshal bytes to map: " + err.Error())
		return
	}

	// get protocol from listener start request
	switch event["protocol"].(type) {
	case string:
		protocol = event["protocol"].(string)
	default:
		logger.DebugError("Failed retrieve protocol: invalid type")
		goto ERROR
	}

	// process listener event
	if event, err = api.teamserver.ListenerEvent(protocol, event); err != nil {
		ctx.JSON(http.StatusInternalServerError, gin.H{
			"error": err.Error(),
		})
		return
	}

	ctx.JSON(http.StatusOK, event)
	return

ERROR:
	ctx.AbortWithStatus(http.StatusInternalServerError)
}
