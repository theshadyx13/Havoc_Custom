#ifndef HAVOCCLIENT_HAVOC_H
#define HAVOCCLIENT_HAVOC_H

#include <Common.h>
#include <Events.h>

/* Havoc include */
#include <core/Helper.h>
#include <core/EventWorker.h>

#include <ui/HcPageAgent.h>
#include <ui/HcPageListener.h>
#include <ui/HcPageBuilder.h>
#include <ui/HcPageScript.h>

#include <ui/HcListenerDialog.h>
#include <ui/HcMainWindow.h>
#include <ui/HcConnectDialog.h>
#include <ui/HcLineEdit.h>

#include <api/Engine.h>

#define HAVOC_VERSION  "1.0"
#define HAVOC_CODENAME "King Of The Damned"

class HavocClient : public QWidget {
    
    struct NamedObject {
        std::string  name;
        py11::object object;
    };

    /* current connection info */
    struct {
        std::string Name;
        std::string Host;
        std::string Port;
        std::string User;
        std::string Pass;

        /* login token to interact
         * with the api endpoints */
        std::string Token;
    } Profile;

    struct {
        QThread*     Thread;
        EventWorker* Worker;
    } Events;

    std::vector<json>         listeners = {};
    std::vector<NamedObject>  protocols = {};
    std::vector<NamedObject>  builders  = {};


public:
    HcMainWindow* Gui    = nullptr;
    HcPyEngine*   Python = nullptr;

    /* havoc client constructor and destructor */
    explicit HavocClient();
    ~HavocClient() override;

    /* client entrypoint */
    auto Main(
        int    argc,
        char** argv
    ) -> void;

    /* exit application and free resources */
    auto Exit() -> void;

    /* get server address */
    auto getServer() -> std::string;

    /* get server connection token */
    auto getServerToken() -> std::string;

    auto getStyleSheet() -> QByteArray;

    auto setupThreads() -> void;

    //
    // Listeners
    //
    auto Listeners() -> std::vector<std::string>;

    auto ListenerObject(
        const std::string& name
    ) -> std::vector<json>;

    auto AddListener(
        const json& listener
    ) -> void;

    //
    // Protocols
    //
    auto AddProtocol(
        const std::string&  name,
        const py11::object& listener
    ) -> void;

    auto ProtocolObject(
        const std::string& name
    ) -> std::optional<py11::object>;

    auto Protocols() -> std::vector<std::string>;

    //
    // Payload Builder
    //
    auto AddBuilder(
        const std::string & name,
        const py11::object& builder
    ) -> void;

    auto BuilderObject(
        const std::string& name
    ) -> std::optional<py11::object>;

    auto Builders() -> std::vector<std::string>;

    //
    // Server Api
    //

    /* send request to api endpoint */
    auto ApiSend(
        const std::string& endpoint,
        const json&        body
    ) const -> httplib::Result;

Q_SIGNALS:
    /* signals */
    auto eventHandle(
        const QByteArray& request
    ) -> void;

    auto eventDispatch(
        const json& event
    ) -> void;

    auto eventClosed() -> void;
};

/* a global Havoc app instance */
extern HavocClient* Havoc;

#endif //HAVOCCLIENT_HAVOC_H
