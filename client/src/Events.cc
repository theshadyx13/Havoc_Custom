#include <Havoc.h>

auto HavocClient::eventDispatch(
    const json& event
) -> void {
    auto type = std::string();
    auto data = json();

    if ( ! event.contains( "type" ) ) {
        spdlog::debug( "invalid event: {}", event.dump() );
        return;
    }

    if ( ! event.contains( "data" ) ) {
        spdlog::debug( "invalid event: {}", event.dump() );
        return;
    }

    type = event[ "type" ].get<std::string>();
    data = event[ "data" ].get<json>();

    if ( type == Event::user::login )
    {
        if ( data.empty() ) {
            spdlog::error( "user::login: invalid package (data emtpy)" );
            return;
        }
    }
    else if ( type == Event::user::logout )
    {
        if ( data.empty() ) {
            spdlog::error( "user::logout: invalid package (data emtpy)" );
            return;
        }
    }
    else if ( type == Event::user::message )
    {

    }
    else if ( type == Event::listener::add )
    {
        if ( data.empty() ) {
            spdlog::error( "listener::register: invalid package (data emtpy)" );
            return;
        }

        Gui->PageListener->Protocols.push_back( data );
        Gui->PagePayload->RefreshBuilders();
    }
    else if ( type == Event::listener::start )
    {
        if ( data.empty() ) {
            spdlog::error( "Event::listener::start: invalid package (data emtpy)" );
            return;
        }

        AddListener( data );
    }
    else if ( type == Event::listener::edit )
    {

    }
    else if ( type == Event::listener::stop )
    {

    }
    else if ( type == Event::listener::status )
    {
        auto name = std::string();
        auto log  = std::string();

        if ( data.empty() ) {
            spdlog::error( "Event::listener::status: invalid package (data emtpy)" );
            return;
        }

        if ( data.contains( "name" ) ) {
            if ( data[ "name" ].is_string() ) {
                name = data[ "name" ].get<std::string>();
            } else {
                spdlog::error( "invalid listener status: \"name\" is not string" );
                return;
            }
        } else {
            spdlog::error( "invalid listener status: \"name\" is not found" );
            return;
        }

        if ( data.contains( "status" ) ) {
            if ( data[ "status" ].is_string() ) {
                log = data[ "status" ].get<std::string>();
            } else {
                spdlog::error( "invalid listener status: \"status\" is not string" );
                return;
            }
        } else {
            spdlog::error( "invalid listener status: \"status\" is not found" );
            return;
        }

        Gui->PageListener->setListenerStatus( name, log );
        Gui->PagePayload->RefreshBuilders();
    }
    else if ( type == Event::listener::log )
    {
        auto name = std::string();
        auto log  = std::string();

        if ( data.empty() ) {
            spdlog::error( "Event::listener::log: invalid package (data emtpy)" );
            return;
        }

        if ( data.contains( "name" ) ) {
            if ( data[ "name" ].is_string() ) {
                name = data[ "name" ].get<std::string>();
            } else {
                spdlog::error( "invalid listener log: \"name\" is not string" );
                return;
            }
        } else {
            spdlog::error( "invalid listener log: \"name\" is not found" );
            return;
        }

        if ( data.contains( "log" ) ) {
            if ( data[ "log" ].is_string() ) {
                log = data[ "log" ].get<std::string>();
            } else {
                spdlog::error( "invalid listener log: \"log\" is not string" );
                return;
            }
        } else {
            spdlog::error( "invalid listener log: \"log\" is not found" );
            return;
        }

        Gui->PageListener->addListenerLog( name, log );
    }
    else if ( type == Event::agent::add )
    {
        Gui->PagePayload->RefreshBuilders();
    }
    else if ( type == Event::agent::initialize )
    {
        if ( data.empty() ) {
            spdlog::error( "Event::agent::initialize: invalid package (data emtpy)" );
            return;
        }

        spdlog::debug( "Agent: {}", data.dump() );

        Gui->PagePayload->RefreshBuilders();
        Gui->PageAgent->addAgent( data );
    }
    else if ( type == Event::agent::callback )
    {
        auto uuid = std::string();
        auto typ  = std::string();
        auto fmt  = json();
        auto pat  = std::string();
        auto out  = std::string();

        if ( data.empty() ) {
            spdlog::error( "Event::agent::callback: invalid package (data emtpy)" );
            return;
        }

        if ( data.contains( "uuid" ) ) {
            if ( data[ "uuid" ].is_string() ) {
                uuid = data[ "uuid" ].get<std::string>();
            } else {
                spdlog::error( "invalid agent callback: \"uuid\" is not string" );
                return;
            }
        } else {
            spdlog::error( "invalid agent callback: \"uuid\" is not found" );
            return;
        }

        if ( data.contains( "type" ) ) {
            if ( data[ "type" ].is_string() ) {
                typ = data[ "type" ].get<std::string>();
            } else {
                spdlog::error( "invalid agent callback: \"type\" is not string" );
                return;
            }
        } else {
            spdlog::error( "invalid agent callback: \"type\" is not found" );
            return;
        }

        if ( typ == "console" ) {
            if ( data.contains( "data" ) ) {
                if ( data[ "data" ].is_object() ) {
                    fmt = data[ "data" ].get<json>();
                } else {
                    spdlog::error( "invalid agent callback: \"data\" is not an object" );
                    return;
                }
            } else {
                spdlog::error( "invalid agent callback: \"data\" is not found" );
                return;
            }

            if ( fmt.contains( "format" ) ) {
                if ( fmt[ "format" ].is_string() ) {
                    pat = fmt[ "format" ].get<std::string>();
                } else {
                    spdlog::error( "invalid agent callback: \"format\" is not string" );
                    return;
                }
            } else {
                spdlog::error( "invalid agent callback: \"format\" is not found" );
                return;
            }

            if ( fmt.contains( "output" ) ) {
                if ( fmt[ "output" ].is_string() ) {
                    out = fmt[ "output" ].get<std::string>();
                } else {
                    spdlog::error( "invalid agent callback: \"output\" is not string" );
                    return;
                }
            } else {
                spdlog::error( "invalid agent callback: \"output\" is not found" );
                return;
            }

            Gui->PageAgent->AgentConsole( uuid, pat, out );
        }

    }
    else if ( type == Event::agent::console )
    {

    }
    else if ( type == Event::agent::input )
    {

    }
    else if ( type == Event::agent::task )
    {

    }
    else if ( type == Event::agent::status )
    {

    }
    else if ( type == Event::agent::remove )
    {

    }
    else if ( type == Event::agent::buildlog )
    {
        auto log = std::string();

        if ( data.empty() ) {
            spdlog::error( "Event::agent::buildlog: invalid package (data emtpy)" );
            return;
        }

        if ( data.contains( "log" ) ) {
            if ( data[ "log" ].is_string() ) {
                log = data[ "log" ].get<std::string>();
            } else {
                spdlog::error( "invalid agent build log: \"log\" is not string" );
                return;
            }
        } else {
            spdlog::error( "invalid agent build log: \"log\" is not found" );
            return;
        }

        Gui->PagePayload->TextBuildLog->append( log.c_str() );
    } else {
        spdlog::debug( "invalid event: {} not found", type );
    }
}


