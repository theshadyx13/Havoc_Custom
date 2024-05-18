#ifndef HAVOCCLIENT_HCEVENTWORKER_H
#define HAVOCCLIENT_HCEVENTWORKER_H

#include <Common.h>

#include <QThread>
#include <QString>
#include <QWebSocket>

class HcEventWorker : public QThread
{
Q_OBJECT
    QWebSocket* WebSocket = nullptr;
    bool        shutdown  = false;

public:
    explicit HcEventWorker();
    ~HcEventWorker();

    /* run event thread */
    void run();

public slots:
    auto connected() -> void;
    auto closed() -> void;
    auto receivedEvent( const QByteArray &message ) -> void;

signals:
    /* dispatch json event */
    auto availableEvent( const QByteArray& event ) -> void;
    auto socketClosed() -> void;
};

#endif //HAVOCCLIENT_HCEVENTWORKER_H
