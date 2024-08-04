#ifndef HAVOCCLIENT_HCSTOREWIDGET_H
#define HAVOCCLIENT_HCSTOREWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>
#include <QtWidgets/QStackedWidget>

#include <ui/HcLineEdit.h>
#include <core/HcStorePluginWorker.h>

class HcStoreWidget : public QWidget {
    Q_OBJECT

private:
    QGridLayout*             gridLayout_3;
    QSplitter*               splitter;
    QGridLayout*             gridLayout;
    QWidget*                 MarketPlaceWidget;
    QLineEdit*               MarketPlaceSearch;
    QListWidget*             MarketPlaceList;
    QStackedWidget*          PluginViewStack;
    std::vector<PluginView*> Plugins;

    struct {
        QThread*             Thread;
        HcStorePluginWorker* Worker;
    } PluginWorker;

public:
    explicit HcStoreWidget( QWidget* parent = nullptr );

    auto AddPlugin(
        const std::string& parent,
        const std::string& repo,
        PluginView*        plugin
    ) -> void;

    auto AddPluginToMarketList(
        PluginView* plugin
    ) -> void;

    auto QueryPluginMarket(
        const QString& query
    ) -> void;

    auto PluginQueryContainMeta(
        const PluginView* plugin,
        const QString&    query
    ) -> bool;

    auto PluginCheckInstalled(
        const PluginView* plugin
    ) -> bool;

    static auto HttpGet(
        const std::string& url
    ) -> std::optional<std::string>;

signals:
    auto RegisterRepository(
        const std::string& repository
    ) -> void;
};

#endif //HAVOCCLIENT_HCSTOREWIDGET_H
