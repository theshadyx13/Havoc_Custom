#include <Havoc.h>
#include <core/EventWorker.h>

EventWorker::EventWorker()  = default;
EventWorker::~EventWorker() = default;

void EventWorker::run() {
    WebSocket    = new QWebSocket;
    auto Host    = QString( Havoc->Server().c_str() );
    auto SslConf = WebSocket->sslConfiguration();

    /* ignore annoying SSL errors */
    SslConf.setPeerVerifyMode( QSslSocket::VerifyNone );
    WebSocket->setSslConfiguration( SslConf );
    WebSocket->ignoreSslErrors();

    QObject::connect( WebSocket, &QWebSocket::connected,             this, &EventWorker::connected );
    QObject::connect( WebSocket, &QWebSocket::disconnected,          this, &EventWorker::closed );
    QObject::connect( WebSocket, &QWebSocket::binaryMessageReceived, this, &EventWorker::receivedEvent );

    WebSocket->open( "wss://" + Host + "/api/event" );
}

auto EventWorker::connected() -> void {
    /* show event progress dialog or something */
    auto login = json {
        { "token", Havoc->Token() }
    };

    WebSocket->sendBinaryMessage( login.dump().c_str() );
}

auto EventWorker::closed() -> void {
    /* this is it. clean everything up and close the Nocturn client */
    emit socketClosed( );
}

auto EventWorker::receivedEvent(
    const QByteArray &message
) -> void {
    /* send json event */
    emit availableEvent( message );
}