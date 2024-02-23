#ifndef HAVOCCLIENT_API_HCAGENT_H
#define HAVOCCLIENT_API_HCAGENT_H

#include <Common.h>
#include <api/Engine.h>

auto HcAgentRegisterInterface(
    const std::string&  type,
    const py11::object& object
) -> void;

auto HcAgentConsoleWrite(
    const std::string& uuid,
    const std::string& content
) -> void;

#endif //HAVOCCLIENT_API_HCAGENT_H
