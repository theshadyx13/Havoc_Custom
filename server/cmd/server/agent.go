package server

import (
	"Havoc/pkg/logger"
	"encoding/json"
	"errors"
	"fmt"
)

func (t *Teamserver) AgentRegister(name string, agent map[string]any) error {
	var (
		payload Handler
		data    []byte
		err     error
	)

	payload = Handler{
		Name: name,
		Data: agent,
	}

	// add the payload to the
	// available payloads lists
	t.payloads = append(t.payloads, payload)

	// convert the object to a json
	if data, err = json.Marshal(payload); err != nil {
		return err
	}

	// broadcast to all connected clients
	// and any future clients
	t.UserBroadcast(true, t.EventCreateJson(EventAgentRegister, data))

	return nil
}

func (t *Teamserver) AgentProcess(ctx map[string]any, request []byte) ([]byte, error) {
	for _, agent := range t.payloads {
		if agent.Data["name"].(string) == ctx["name"].(string) {
			return t.plugins.AgentProcess(ctx, request)
		}
	}

	return nil, errors.New("agent to process request not found")
}

func (t *Teamserver) AgentGenerate(ctx map[string]any, config map[string]any) (string, []byte, error) {
	for _, agent := range t.payloads {
		if agent.Data["name"].(string) == ctx["name"].(string) {
			return t.plugins.AgentGenerate(ctx, config)
		}
	}

	return "", nil, errors.New("agent to generate not found")
}

func (t *Teamserver) AgentBuildLog(context map[string]any, format string, args ...any) {
	var (
		user string
		err  error
	)

	user = context["user"].(string)

	//
	// send the client the build log message
	//
	err = t.UserSend(user, t.EventCreate(EventAgentBuildLog, map[string]any{
		"log": fmt.Sprintf(format, args...),
	}))

	if err != nil {
		logger.DebugError("failed to send build log message to %v: %v", user, err)
		return
	}
}
