#ifndef HAVOCCLIENT_HCHEARTBEATWORKER_H
#define HAVOCCLIENT_HCHEARTBEATWORKER_H

#include <Common.h>

#include <QThread>
#include <QString>

//
// heartbeat worker is a thread that handles every agent heartbeat and
// updates the last called time in seconds, minutes, hours, days, etc.
//

class HcHeartbeatWorker : public QThread {
    Q_OBJECT

    QTimer* HeartbeatTimer = nullptr;

public:
    explicit HcHeartbeatWorker();
    ~HcHeartbeatWorker();

    /* run event thread */
    void run();

private:
    auto updateHeartbeats() -> void;
};

#endif //HAVOCCLIENT_HCHEARTBEATWORKER_H
