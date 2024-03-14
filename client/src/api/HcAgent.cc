#include <Havoc.h>
#include <api/HcAgent.h>

/*!
 * @brief
 *  register an agent interface to the havoc client
 *
 * @param type
 *  type of the agent (name)
 *
 * @param object
 *  object to register
 */
auto HcAgentRegisterInterface(
    const std::string&  type,
    const py11::object& object
) -> void {
    Havoc->AddAgentObject( type, object );
}

/*!
 * @brief
 *  writes content to specified agent console
 *
 * @param uuid
 *  uuid of the agent
 *
 * @param content
 *  content to write to the console
 */
auto HcAgentConsoleWrite(
    const std::string& uuid,
    const std::string& content
) -> void {
    auto agent = Havoc->Agent( uuid );

    if ( agent.has_value() ) {
        emit agent.value()->emitter.ConsoleWrite( uuid.c_str(), content.c_str() );
    }
}

/*!
 * @brief
 *  send agent command to the server implant plugin handler
 *
 * @param uuid
 *  uuid of the agent
 *
 * @param data
 *  data to send to the handler
 *
 * @param wait
 *  wait for a response
 *
 * @return
 *  response from the server implant handler
 */
auto HcAgentExecute(
    const std::string& uuid,
    const json&        data,
    const bool         wait
) -> json {
    auto future   = QFuture<json>();
    auto request  = json();
    auto result   = httplib::Result();
    auto response = json();

    //
    // build request that is going to be
    // sent to the server implant handler
    //
    request = {
        { "uuid", uuid },
        { "wait", wait },
        { "data", data }
    };

    //
    // send api request
    //
    if ( ( result = Havoc->ApiSend( "/api/agent/execute", request, true ) ) ) {
        //
        // check for valid status response
        //
        if ( result->status != 200 ) {
            spdlog::debug( "failed to send request: status code {}", result->status );
            return json {
                    { "error", "failed to send request" }
            };
        }
        //
        // check for emtpy request
        //
        if ( ! result->body.empty() ) {
            if ( ( response = json::parse( result->body ) ).is_discarded() ) {
                response[ "error" ] = "failed to parse response";
            }
        }
    }

    return response;
}