#ifndef HAVOCCLIENT_API_ENGINE_H
#define HAVOCCLIENT_API_ENGINE_H

#include <Common.h>

#include <api/HcCore.h>
#include <api/HcScriptManager.h>
#include <api/HcAgent.h>

class HcPyEngine : public QThread {
    Q_OBJECT

public:
    py11::scoped_interpreter* guard{};

    std::optional<py11::object> PyEval = {};
    std::optional<py11::object> PyLoad = {};

    explicit HcPyEngine();
    ~HcPyEngine();

    auto run() -> void;

public slots:
    auto ScriptEval(
        const std::string& code
    ) -> void;

    auto ScriptLoad(
        const std::string& path
    ) -> void;
};

#endif
