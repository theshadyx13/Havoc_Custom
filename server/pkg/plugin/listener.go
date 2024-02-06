package plugin

import "errors"

func (s *PluginSystem) ListenerStart(name, protocol string, options map[string]any) (map[string]string, error) {
	var (
		data map[string]string
		err  error
		ext  *Plugin
	)

	err = errors.New("protocol not found")

	s.loaded.Range(func(key, value any) bool {
		ext = value.(*Plugin)

		if ext.Type != PluginTypeListener {
			return true
		}

		if protocol == ext.ListenerRegister()["protocol"].(string) {
			data, err = ext.ListenerStart(name, options)
			return false
		}

		return true
	})

	return data, err
}

func (s *PluginSystem) ListenerEvent(protocol string, event map[string]any) (map[string]any, error) {
	var (
		err  error
		ext  *Plugin
		resp map[string]any
	)

	err = errors.New("protocol not found")

	s.loaded.Range(func(key, value any) bool {
		ext = value.(*Plugin)

		if ext.Type != PluginTypeListener {
			return true
		}

		if protocol == ext.ListenerRegister()["protocol"].(string) {
			resp, err = ext.ListenerEvent(event)
			return false
		}

		return true
	})

	return resp, err
}
