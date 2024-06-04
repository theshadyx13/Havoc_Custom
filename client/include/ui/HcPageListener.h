#ifndef HAVOCCLIENT_HCPAGELISTENER_H
#define HAVOCCLIENT_HCPAGELISTENER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class HcPageListener : public QWidget
{
    bool SplitterMoveToggle = false;
    int  ListenersRunning   = 0;

    typedef struct {
        QTableWidgetItem* Name;
        QTableWidgetItem* Type;
        QTableWidgetItem* Host;
        QTableWidgetItem* Port;
        QTableWidgetItem* Status;

        QTextEdit*        Logger;
    } Listener;

    std::vector<Listener*> TableEntries  = {};

public:
    QGridLayout*      gridLayout        = nullptr;
    QLabel*           ActiveLabel       = nullptr;
    QSpacerItem*      horizontalSpacer  = nullptr;
    QPushButton*      ButtonNewListener = nullptr;
    QSplitter*        Splitter          = nullptr;
    QTableWidget*     TableWidget       = nullptr;
    QTabWidget*       TabWidget         = nullptr;
    QTableWidgetItem* TitleName         = nullptr;
    QTableWidgetItem* TitleType         = nullptr;
    QTableWidgetItem* TitleHost         = nullptr;
    QTableWidgetItem* TitlePort         = nullptr;
    QTableWidgetItem* TitleStatus       = nullptr;
    std::vector<json> Protocols         = {};
    std::vector<json> Listeners         = {};

    explicit HcPageListener();
    auto retranslateUi() -> void;

    auto addListener(
        const json& data
    ) -> void;

    auto addListenerLog(
        const std::string& name,
        const std::string& log
    ) -> void;

    auto setListenerStatus(
        const std::string& name,
        const std::string& status
    ) -> void;

    auto updateListenersRunningLabel(
        int value
    ) const -> void;

    auto handleListenerContextMenu(
        const QPoint& pos
    ) -> void;

    auto tabCloseRequested(
        int index
    ) const -> void;

    auto buttonAddListener() -> void;
};

QT_END_NAMESPACE

#endif //HAVOCCLIENT_HCPAGELISTENER_H
