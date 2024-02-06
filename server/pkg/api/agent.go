package api

import (
	"Havoc/pkg/logger"
	"encoding/base64"
	"encoding/json"
	"errors"
	"github.com/gin-gonic/gin"
	"io"
	"net/http"
)

func (api *ServerApi) agentBuild(ctx *gin.Context) {
	var (
		body     []byte
		context  map[string]any
		config   map[string]any
		agent    map[string]any
		name     string
		username string
		ok       bool
		err      error
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

	logger.Debug("/api/agent/build -> %v", string(body))

	// unmarshal the bytes into a map
	if err = json.Unmarshal(body, &agent); err != nil {
		logger.DebugError("Failed to unmarshal bytes to map: " + err.Error())
		err = errors.New("invalid request")
		return
	}

	//
	// get name from agent request
	//
	switch agent["name"].(type) {
	case string:
		name = agent["name"].(string)
	default:
		logger.DebugError("Failed retrieve agent name: invalid type")
		err = errors.New("invalid request")
		goto ERROR
	}

	//
	// get config from agent request
	//
	switch agent["config"].(type) {
	case map[string]any:
		config = agent["config"].(map[string]any)
	default:
		logger.DebugError("Failed retrieve agent config: invalid type")
		err = errors.New("invalid request")
		goto ERROR
	}

	//
	// get the username from the token
	//
	if username, ok = api.tokenUser(ctx.GetHeader(ApiTokenHeader)); !ok {
		logger.DebugError("Failed to authenticate token: couldn't get username from token")
		err = errors.New("failed to authenticate token")
		goto ERROR
	}

	context = map[string]any{
		"name": name,
		"user": username,
	}

	//
	// interact with the plugin to generate a payload
	//
	name, body, err = api.teamserver.AgentGenerate(context, config)
	if err != nil {
		logger.DebugError("Failed to generate agent payload: %v", err)
		goto ERROR
	}

	//
	// return base64 encoded payload
	//
	ctx.JSON(http.StatusOK, gin.H{
		"payload":  base64.StdEncoding.EncodeToString(body),
		"filename": name,
	})

	return

ERROR:
	ctx.JSON(http.StatusInternalServerError, gin.H{
		"error": err.Error(),
	})
}
