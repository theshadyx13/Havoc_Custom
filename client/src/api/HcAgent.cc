#include <Havoc.h>
#include <api/HcAgent.h>

auto HcAgentRegisterInterface(
    const std::string&  type,
    const py11::object& object
) -> void {
    Havoc->AddAgentObject( type, object );
}

auto HcAgentConsoleWrite(
    const std::string& uuid,
    const std::string& content
) -> void {
    auto agent = Havoc->Agent( uuid );

    if ( agent.has_value() ) {
        emit agent.value()->emitter.ConsoleWrite( content.c_str() );
    }
}