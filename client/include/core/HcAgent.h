#ifndef HAVOCCLIENT_HCAGENT_H
#define HAVOCCLIENT_HCAGENT_H

class HcAgentConsole;
struct HcAgent;

#include <Common.h>
#include <ui/HcPageAgent.h>

class HcAgentEmit : public QWidget {
    Q_OBJECT

signals:
    auto ConsoleWrite(
        const QString& text
    ) -> void;
};

struct HcAgent {
    std::string                 uuid;
    std::string                 type;
    json                        data;
    std::optional<py11::object> interface;
    HcAgentConsole*             console;

    struct {
        QTableWidgetItem* Uuid;
        QTableWidgetItem* Internal;
        QTableWidgetItem* Username;
        QTableWidgetItem* Hostname;
        QTableWidgetItem* ProcessPath;
        QTableWidgetItem* ProcessName;
        QTableWidgetItem* ProcessId;
        QTableWidgetItem* ThreadId;
        QTableWidgetItem* Arch;
        QTableWidgetItem* System;
        QTableWidgetItem* Note;
        QTableWidgetItem* Last;
    } ui;

    //
    // signals
    //
    HcAgentEmit emitter = {};
};

#endif //HAVOCCLIENT_HCAGENT_H
