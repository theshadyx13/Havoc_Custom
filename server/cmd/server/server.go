package server

import "errors"

func (t *Teamserver) ServerAgentRegister(uuid, name string, agent map[string]any) error {

	// check if the given uuid already exists
	if _, ok := t.agents.Load(uuid); ok {
		return errors.New("agent with given uuid already exists")
	}

	// store the agent data with the given uuid
	t.agents.Store(uuid, Agent{
		uuid:  uuid,
		_type: name,
		data:  agent,
	})

	t.UserBroadcast(true, t.EventCreate(EventAgentInitialize, map[string]any{
		"uuid": uuid,
		"type": name,
		"meta": agent,
	}))

	return nil
}

func (t *Teamserver) ServerAgentExist(uuid string) bool {
	_, found := t.agents.Load(uuid)

	return found
}

func (t *Teamserver) ServerAgent(uuid string) (map[string]any, error) {
	var (
		agent any
		ok    bool
	)

	// check if the given uuid already exists
	if agent, ok = t.agents.Load(uuid); !ok {
		return nil, errors.New("agent with given uuid doesn't exists")
	}

	return agent.(Agent).data, nil
}

func (t *Teamserver) ServerAgentType(uuid string) (string, error) {
	var (
		agent any
		ok    bool
	)

	// check if the given uuid already exists
	if agent, ok = t.agents.Load(uuid); !ok {
		return "", errors.New("agent with given uuid doesn't exists")
	}

	return agent.(Agent)._type, nil
}
