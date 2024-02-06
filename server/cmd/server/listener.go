package server

import (
	"encoding/json"
	"errors"
	"fmt"
	"os"
)

// ListenerRegister
// register a listener to the server and
// notify clients to a newly available handler.
func (t *Teamserver) ListenerRegister(name string, listener map[string]any) error {
	var (
		protocol Handler
		data     []byte
		err      error
	)

	protocol = Handler{
		Name: name,
		Data: listener,
	}

	// add the listener to the
	// available protocols lists
	t.protocols = append(t.protocols, protocol)

	// convert the object to a json
	if data, err = json.Marshal(protocol); err != nil {
		return err
	}

	// broadcast to all connected clients
	// and any future clients
	t.UserBroadcast(true, t.EventCreateJson(EventListenerRegister, data))

	return nil
}

func (t *Teamserver) ListenerExists(name string) bool {
	for _, listener := range t.listener {
		if listener.Name == name {
			return true
		}
	}

	return false
}

func (t *Teamserver) ListenerInitDir(name string) (string, error) {
	var (
		path string
		err  error
	)

	path = t.configPath + "/listeners/" + name

	// create agents directory
	if err = os.MkdirAll(path, os.ModePerm); err != nil {
		return path, err
	}

	return path, nil
}

func (t *Teamserver) ListenerStart(name, protocol string, options map[string]any) error {
	var (
		err    error
		data   map[string]string
		host   string
		port   string
		status string
		path   string
	)

	for _, prot := range t.protocols {

		if val, ok := prot.Data["protocol"]; ok {

			if val.(string) == protocol {

				if t.ListenerExists(name) {
					return errors.New("listener already exists")
				}

				if path, err = t.ListenerInitDir(name); err != nil {
					return errors.New("failed to create listener config path: " + err.Error())
				}

				if data, err = t.plugins.ListenerStart(name, protocol, options); err != nil {
					return err
				}

				host, ok = data["host"]
				port, ok = data["port"]
				status, ok = data["status"]

				t.listener = append(t.listener, Handler{
					Name: name,
					Data: map[string]any{
						"protocol":    protocol,
						"host":        host,
						"port":        port,
						"status":      status,
						"config.path": path,
					},
				})

				t.UserBroadcast(true, t.EventCreate(EventListenerStart, map[string]string{
					"name":     name,
					"protocol": protocol,
					"host":     host,
					"port":     port,
					"status":   status,
				}))

				break
			}
		}
	}

	return nil
}

func (t *Teamserver) ListenerEvent(protocol string, event map[string]any) (map[string]any, error) {
	var (
		err  error
		resp map[string]any
	)

	err = errors.New("protocol not found")

	for _, p := range t.protocols {
		if val, ok := p.Data["protocol"]; ok {
			if val.(string) == protocol {
				if resp, err = t.plugins.ListenerEvent(protocol, event); err != nil {
					return nil, err
				}

				break
			}
		}
	}

	return resp, err
}

func (t *Teamserver) ListenerLog(name string, format string, args ...any) {
	t.UserBroadcast(true, t.EventCreate(EventListenerLog, map[string]any{
		"name": name,
		"log":  fmt.Sprintf(format, args...),
	}))
}

func (t *Teamserver) ListenerStatus(name string, status string) {
	t.UserBroadcast(true, t.EventCreate(EventListenerStatus, map[string]any{
		"name":   name,
		"status": status,
	}))
}

func (t *Teamserver) ListenerConfigPath(name string) string {
	return t.configPath + "/listeners/" + name
}
