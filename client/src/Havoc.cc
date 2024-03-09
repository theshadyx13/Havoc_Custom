#include <Havoc.h>
#include <QTimer>
#include <QtCore5Compat/QTextCodec>

auto HttpErrorToString(
    const httplib::Error& error
) -> std::optional<std::string> {
    switch ( error ) {
        case httplib::Error::Unknown:
            return "Unknown";

        case httplib::Error::Connection:
            return ( "Connection" );

        case httplib::Error::BindIPAddress:
            return ( "BindIPAddress" );

        case httplib::Error::Read:
            return ( "Read" );

        case httplib::Error::Write:
            return ( "Write" );

        case httplib::Error::ExceedRedirectCount:
            return ( "ExceedRedirectCount" );

        case httplib::Error::Canceled:
            return ( "Canceled" );

        case httplib::Error::SSLConnection:
            return ( "SSLConnection" );

        case httplib::Error::SSLLoadingCerts:
            return ( "SSLLoadingCerts" );

        case httplib::Error::SSLServerVerification:
            return ( "SSLServerVerification" );

        case httplib::Error::UnsupportedMultipartBoundaryChars:
            return ( "UnsupportedMultipartBoundaryChars" );

        case httplib::Error::Compression:
            return ( "Compression" );

        case httplib::Error::ConnectionTimeout:
            return ( "ConnectionTimeout" );

        case httplib::Error::ProxyConnection:
            return ( "ProxyConnection" );

        case httplib::Error::SSLPeerCouldBeClosed_:
            return ( "SSLPeerCouldBeClosed_" );

        default: break;
    }

    return std::nullopt;
}

HavocClient::HavocClient() {
    /* initialize logger */
    spdlog::set_pattern( "[%T %^%l%$] %v" );
    spdlog::info( "Havoc Framework [{} :: {}]", HAVOC_VERSION, HAVOC_CODENAME );

    /* enabled debug messages */
    spdlog::set_level( spdlog::level::debug );

    /* TODO: read this from the config file */
    const auto family = "Monospace";
    const auto size   = 9;

    QTextCodec::setCodecForLocale( QTextCodec::codecForName( "UTF-8" ) );
    QApplication::setFont( QFont( family, size ) );
    QTimer::singleShot( 10, [&]() {
        QApplication::setFont( QFont( family, size ) );
    } );
}

HavocClient::~HavocClient() = default;

/*!
 * @brief
 *  this entrypoint executes the connector dialog
 *  and tries to connect and login to the teamserver
 *
 *  after connecting it is going to start an event thread
 *  and starting the Havoc MainWindow.
 */
auto HavocClient::Main(
    int    argc,
    char** argv
) -> void {
    auto Connector = new HcConnectDialog();
    auto Result    = httplib::Result();
    auto Response  = json{};
    auto Error     = std::string( "Failed to send login request: " );

    /* get provided creds */
    auto data = Connector->start();
    if ( data.empty() || ! Connector->pressedConnect() ) {
        return;
    }

    /* create http client */
    auto Http = httplib::Client( "https://" + data[ "host" ].get<std::string>() + ":" + data[ "port" ].get<std::string>() );
    Http.enable_server_certificate_verification( false );

    /* send request */
    Result = Http.Post( "/api/login", data.dump(), "application/json" );

    if ( HttpErrorToString( Result.error() ).has_value() ) {
        spdlog::error( "Failed to send login request: {}", HttpErrorToString( Result.error() ).value() );
        return;
    }

    /* 401 Unauthorized: Failed to log in */
    if ( Result->status == 401 ) {

        if ( Result->body.empty() ) {
            Helper::MessageBox(
                QMessageBox::Critical,
                "Login failure",
                "Failed to login: Unauthorized"
            );

            return;
        } else {
            if ( ( data = json::parse( Result->body ) ).is_discarded() ) {
                goto InvalidServerResponseError;
            }

            if ( ! data.contains( "error" ) ) {
                goto InvalidServerResponseError;
            }

            if ( ! data[ "error" ].is_string() ) {
                goto InvalidServerResponseError;
            }

            Helper::MessageBox(
                QMessageBox::Critical,
                "Login failure",
                QString( "Failed to login: %1" ).arg( data[ "error" ].get<std::string>().c_str() ).toStdString()
            );

            return;
        }

    } else if ( Result->status != 200 ) {
        Helper::MessageBox(
            QMessageBox::Critical,
            "Login failure",
            QString( "Unexpected response: Http status code %1" ).arg( Result->status ).toStdString()
        );
        return;
    }

    spdlog::debug( "Result: {}", Result->body );

    Profile.Name = data[ "name" ].get<std::string>();
    Profile.Host = data[ "host" ].get<std::string>();
    Profile.Port = data[ "port" ].get<std::string>();
    Profile.User = data[ "username" ].get<std::string>();
    Profile.Pass = data[ "password" ].get<std::string>();

    if ( Result->body.empty() ) {
        goto InvalidServerResponseError;
    }

    if ( ( data = json::parse( Result->body ) ).is_discarded() ) {
        goto InvalidServerResponseError;
    }

    if ( ! data.contains( "token" ) ) {
        goto InvalidServerResponseError;
    }

    if ( ! data[ "token" ].is_string() ) {
        goto InvalidServerResponseError;
    }

    Profile.Token = data[ "token" ].get<std::string>();

    //
    // create main window
    //
    Gui = new HcMainWindow;
    Gui->renderWindow();
    Gui->setStyleSheet( getStyleSheet() );

    //
    // setup Python thread
    //
    // Python.Thread = new QThread;
    Python.Engine = new HcPyEngine();
    // Python.Engine->moveToThread( Python.Thread );
    // QObject::connect( Python.Thread, & QThread::started, Python.Engine, & HcPyEngine::run );
    Python.Engine->run();

    //
    // merely debug purpose loading the scripts at startup
    //
    // TODO: remove this in future or
    //       move it to the config file
    //
    Gui->PageScripts->LoadScript( "tests/python/kaine_payload.py" );
    Gui->PageScripts->LoadScript( "tests/python/listener_http.py" );

    Gui->PageScripts->LoadScript( "tests/python/kaine_commands.py" );

    //
    // set up the event thread and connect to the
    // server and dispatch all the incoming events
    //
    setupThreads();

    QApplication::exec();

    return;

InvalidServerResponseError:
    Helper::MessageBox(
        QMessageBox::Critical,
        "Login failure",
        "Failed to login: Invalid response from the server"
    );
}

auto HavocClient::Exit() -> void {
    QApplication::exit( 0 );
}

auto HavocClient::ApiSend(
    const std::string& endpoint,
    const json&        body,
    const bool         keep_alive
) const -> httplib::Result {
    auto Http   = httplib::Client( "https://" + Profile.Host + ":" + Profile.Port );
    auto Result = httplib::Result();
    auto Error  = std::string( "Failed to send api request: " );

    //
    // only way to keep the connection alive even while we have
    // "keep-alive" enabled it will shut down after 5 seconds
    //
    if ( keep_alive ) {
        Http.set_read_timeout( INT32_MAX );
        Http.set_connection_timeout( INT32_MAX );
        Http.set_write_timeout( INT32_MAX );
    }

    //
    // configure the client
    //
    Http.set_keep_alive( keep_alive );
    Http.enable_server_certificate_verification( false );
    Http.set_default_headers( {
        { "x-havoc-token", Havoc->Profile.Token }
    } );

    //
    // send the request to our endpoint
    //
    Result = Http.Post( endpoint, body.dump(), "application/json" );

    if ( HttpErrorToString( Result.error() ).has_value() ) {
        spdlog::error( "Failed to send login request: {}", HttpErrorToString( Result.error() ).value() );
    }

    return Result;
}

auto HavocClient::eventClosed() -> void {
    spdlog::error( "websocket closed" );
    Exit();
}

auto HavocClient::Server() const -> std::string { return Profile.Host + ":" + Profile.Port; }
auto HavocClient::Token()  const -> std::string { return Profile.Token; }

auto HavocClient::eventHandle(
    const QByteArray& request
) -> void {
    auto event = json::parse( request.toStdString() );

    /* check if we managed to parse the json event
     * if yes then dispatch it but if not then dismiss it */
    if ( ! event.is_discarded() ) {
        eventDispatch( event );
    } else {
        spdlog::error( "failed to parse event" );
        /* what now ?
         * I guess ignore since its not valid event
         * or debug print it I guess */
    }
}

auto HavocClient::getStyleSheet(
    void
) -> QByteArray {
    if ( QFile::exists( "theme.css" ) ) {
        return Helper::FileRead( "theme.css" );
    }

    return Helper::FileRead( ":/style/default" );
}

auto HavocClient::AddProtocol(
    const std::string&  name,
    const py11::object& listener
) -> void {
    protocols.push_back( NamedObject{
        .name   = name,
        .object = listener
    } );
}

auto HavocClient::ProtocolObject(
    const std::string& name
) -> std::optional<py11::object> {
    for ( auto& listener : protocols ) {
        if ( listener.name == name ) {
            return listener.object;
        }
    }

    return std::nullopt;
}

auto HavocClient::Protocols() -> std::vector<std::string> {
    auto names = std::vector<std::string>();

    for ( auto& listener : protocols ) {
        names.push_back( listener.name );
    }

    return names;
}

auto HavocClient::setupThreads() -> void {
    //
    // now set up the event thread and dispatcher
    //
    Events.Thread = new QThread;
    Events.Worker = new EventWorker;
    Events.Worker->moveToThread( Events.Thread );

    QObject::connect( Events.Thread, &QThread::started, Events.Worker, &EventWorker::run );
    QObject::connect( Events.Worker, &EventWorker::availableEvent, this, &HavocClient::eventHandle );
    QObject::connect( Events.Worker, &EventWorker::socketClosed, this, &HavocClient::eventClosed );

    //
    // fire up the even thread that is going to
    // process events and emit signals to the main gui thread
    //
    Events.Thread->start();
}

auto HavocClient::AddBuilder(
    const std::string & name,
    const py11::object& builder
) -> void {
    builders.push_back( NamedObject{
        .name   = name,
        .object = builder
    } );

    Havoc->Gui->PagePayload->AddBuilder( name, builder );
}

auto HavocClient::BuilderObject(
    const std::string& name
) -> std::optional<py11::object> {

    for ( auto& builder : builders ) {
        if ( builder.name == name ) {
            return builder.object;
        }
    }

    return std::nullopt;
}

auto HavocClient::Builders() -> std::vector<std::string>
{
    auto names = std::vector<std::string>();

    for ( auto& builder : builders ) {
        names.push_back( builder.name );
    }

    return names;
}

auto HavocClient::AddListener(
    const json& listener
) -> void {
    spdlog::debug( "listener -> {}", listener.dump() );
    listeners.push_back( listener );

    Gui->PageListener->addListener( listener );
    Gui->PagePayload->RefreshBuilders();
}

auto HavocClient::Listeners() -> std::vector<std::string>
{
    auto names = std::vector<std::string>();

    for ( auto& data : listeners ) {
        if ( data.contains( "name" ) ) {
            if ( data[ "name" ].is_string() ) {
                names.push_back( data[ "name" ].get<std::string>() );
            }
        }
    }

    return names;
}

auto HavocClient::Agent(
    const std::string& uuid
) const -> std::optional<HcAgent*> {

    for ( auto agent : Gui->PageAgent->agents ) {
        if ( agent->uuid == uuid ) {
            return agent;
        }
    }

    return std::nullopt;
}

auto HavocClient::AddAgentObject(
    const std::string&  type,
    const py11::object& object
) -> void {
    agents.push_back( NamedObject {
        .name   = type,
        .object = object
    } );
}

auto HavocClient::AgentObject(
    const std::string& type
) -> std::optional<py11::object> {
    for ( auto object : agents ) {
        if ( object.name == type ) {
            return object.object;
        }
    }

    return std::nullopt;
}