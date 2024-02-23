#ifndef HAVOCCLIENT_API_HCSCRIPTMANAGER_H
#define HAVOCCLIENT_API_HCSCRIPTMANAGER_H

#include <Common.h>
#include <api/Engine.h>

auto HcIoConsoleWriteStdOut(
    const std::string& text
) -> void;

auto HcIoScriptLoadCallback(
    const py11::object& callback
) -> void;

#endif //HAVOCCLIENT_API_HCSCRIPTMANAGER_H
