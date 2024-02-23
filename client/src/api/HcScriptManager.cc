#include <Havoc.h>
#include <api/HcScriptManager.h>

auto HcIoConsoleWriteStdOut(
    const std::string& text
) -> void {
    emit Havoc->Gui->PageScripts->SignalConsoleWrite( text.c_str() );
}

auto HcIoScriptLoadCallback(
    const py11::object& callback
) -> void {
    // Havoc->Gui->PageScripts->LoadCallback = callback;
}