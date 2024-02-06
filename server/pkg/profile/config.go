package profile

import (
	"Havoc/pkg/logger"
	"errors"
	"reflect"
	"strconv"
)

type Server struct {
	Host    string
	Port    string
	Plugins []string
}

type Operator struct {
	Username string
	Password string
}

func (p *Profile) sanityCheck() error {
	var (
		data      map[string]any
		operators []any
		val       any
		ok        bool
	)

	if data, ok = p.config["server"].(map[string]any); ok {

		if val, ok = data["host"]; ok {
			switch val.(type) {
			case string:
				break
			default:
				return errors.New("invalid type: server host is not string")
			}
		} else {
			return errors.New("no server host defined")
		}

		if val, ok = data["port"]; ok {

			switch val.(type) {
			case int64:
				break
			default:
				return errors.New("invalid type: server port is not integer")
			}
		} else {
			return errors.New("no server port defined")
		}

		if val, ok = data["plugins"]; ok {
			logger.Debug("%v", reflect.TypeOf(val))
			switch val.(type) {
			case []any:
				for i := range val.([]any) {
					switch val.([]any)[i].(type) {
					case string:
						continue
					default:
						return errors.New("invalid type: server plugins is not string array")
					}
				}
				break
			default:
				return errors.New("invalid type: server plugins is not string array")
			}
		}

	} else {
		return errors.New("no server defined")
	}

	if operators, ok = p.config["operator"].([]any); ok {
		for i := range operators {
			data = operators[i].(map[string]any)

			if val, ok = data["username"]; ok {
				switch val.(type) {
				case string:
					break
				default:
					return errors.New("invalid type: operator username is not string")
				}
			} else {
				return errors.New("invalid type: operator username is not set")
			}

			if val, ok = data["password"]; ok {
				switch val.(type) {
				case string:
					break
				default:
					return errors.New("invalid type: operator password is not string")
				}
			} else {
				return errors.New("invalid type: operator password is not set")
			}
		}
	}

	return nil
}

func (p *Profile) Server() (Server, error) {
	var (
		server map[string]any
		val    any
		ok     bool

		host    string
		port    string
		plugins []string
	)

	if server, ok = p.config["server"].(map[string]any); ok {

		if val, ok = server["host"]; ok {
			switch val.(type) {
			case string:
				host = val.(string)
				break
			default:
				return Server{}, errors.New("invalid type: server host is not string")
			}
		} else {
			return Server{}, errors.New("no server host defined")
		}

		if val, ok = server["port"]; ok {

			switch val.(type) {
			case int64:
				port = strconv.Itoa(int(val.(int64)))
				break
			default:
				return Server{}, errors.New("invalid type: server port is not integer")
			}
		} else {
			return Server{}, errors.New("no server port defined")
		}

		if val, ok = server["plugins"]; ok {
			logger.Debug("%v", reflect.TypeOf(val))
			switch val.(type) {
			case []any:
				for i := range val.([]any) {
					switch val.([]any)[i].(type) {
					case string:
						plugins = append(plugins, val.([]any)[i].(string))
						continue
					default:
						return Server{}, errors.New("invalid type: server plugins is not string array")
					}
				}
				break
			default:
				return Server{}, errors.New("invalid type: server plugins is not string array")
			}
		}

	} else {
		return Server{}, errors.New("no server defined")
	}

	return Server{
		Host:    host,
		Port:    port,
		Plugins: plugins,
	}, nil
}

func (p *Profile) Operators() ([]Operator, error) {
	var (
		operators []Operator
		operator  Operator
		data      map[string]any
		array     []any
		val       any
		ok        bool
	)

	if array, ok = p.config["operator"].([]any); ok {
		for i := range array {
			data = array[i].(map[string]any)

			if val, ok = data["username"]; ok {
				operator.Username = val.(string)
			} else {
				return nil, errors.New("invalid type: operator username is not set")
			}

			if val, ok = data["password"]; ok {
				operator.Password = val.(string)
			} else {
				return nil, errors.New("invalid type: operator password is not set")
			}

			operators = append(operators, operator)
		}
	}

	return operators, nil
}
