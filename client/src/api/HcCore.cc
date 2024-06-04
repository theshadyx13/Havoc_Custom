#include <Common.h>
#include <Havoc.h>
#include <api/HcCore.h>

auto HcServerApiSend(
    const std::string& endpoint,
    const json&        data
) -> json {
    auto result = httplib::Result();

    result = Havoc->ApiSend(
        endpoint,
        data
    );

    return json {
        { "version", result->version },
        { "status",  result->status  },
        { "reason",  result->reason  },
        { "body",    result->body    },
    };
}

auto HcListenerProtocolData(
    const std::string& protocol
) -> json {

    for ( auto& p : Havoc->Gui->PageListener->Protocols ) {
        if ( p.contains( "data" ) ) {
            if ( p[ "data" ].contains( "protocol" ) ) {
                if ( p[ "data" ][ "protocol" ] == protocol ) {
                    if ( p[ "data" ].contains( "data" ) ) {
                        return p[ "data" ][ "data" ];
                    }
                }
            }
        }
    }

    return {};
}

auto HcListenerAll() -> std::vector<std::string>
{
    return Havoc->Listeners();
}

auto HcListenerQueryType(
    const std::string& name
) -> std::string {
    for ( auto& n : Havoc->Listeners() ) {
        if ( auto obj = Havoc->ListenerObject( n ) ) {
            if ( obj.has_value() && obj.value()[ "name" ] == name ) {
                return obj.value()[ "protocol" ].get<std::string>();
            }
        }
    }

    return std::string();
}