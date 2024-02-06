package plugin

import "errors"

func (s *PluginSystem) AgentGenerate(ctx map[string]any, config map[string]any) (string, []byte, error) {
	var (
		err  error
		ext  *Plugin
		bin  []byte
		name string
	)

	err = errors.New("agent not found")

	s.loaded.Range(func(key, value any) bool {
		ext = value.(*Plugin)

		if ext.Type != PluginTypeAgent {
			return true
		}

		if ctx["name"] == ext.AgentRegister()["name"] {
			name, bin, err = ext.AgentGenerate(ctx, config)
			return false
		}

		return true
	})

	return name, bin, err
}

func (s *PluginSystem) AgentProcess(ctx map[string]any, request []byte) ([]byte, error) {
	var (
		err error
		ext *Plugin
		res []byte
	)

	err = errors.New("agent not found")

	s.loaded.Range(func(key, value any) bool {
		ext = value.(*Plugin)

		if ext.Type != PluginTypeAgent {
			return true
		}

		if ctx["name"] == ext.AgentRegister()["name"] {
			res, err = ext.AgentProcess(ctx, request)
			return false
		}

		return true
	})

	return res, err
}
