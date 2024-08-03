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

class HcLabelNamed : public QWidget {
    QHBoxLayout* Layout = { 0 };
public:
    QLabel* Label = { 0 };
    QLabel* Text  = { 0 };

    explicit HcLabelNamed(
        const QString& title,
        const QString& text,
        QWidget*       parent = nullptr
    );
};

struct PluginView {
    std::string repo;
    json        object;
    std::string plugin_id;
    bool        installed;

    QListWidgetItem* ListItem;
    QWidget*         ListWidget;
    QWidget*         Widget;
    QGridLayout*     GridLayout;
    QLabel*          LabelName;
    HcLabelNamed*    LabelAuthor;
    QLabel*          LabelDescription;
    QTextEdit*       TextReadme;
    QPushButton*     ButtonInstall;
    QSpacerItem*     Spacers[ 3 ];
    QHBoxLayout*     LayoutCategories;
};

class HcStoreWidget : public QWidget {
    QGridLayout*             gridLayout_3;
    QSplitter*               splitter;
    QGridLayout*             gridLayout;
    QWidget*                 MarketPlaceWidget;
    QLineEdit*               MarketPlaceSearch;
    QListWidget*             MarketPlaceList;
    QStackedWidget*          PluginViewStack;
    std::vector<PluginView*> Plugins;

public:
    explicit HcStoreWidget( QWidget* parent = nullptr );

    auto AddPlugin(
        const QString& repo
    ) -> void;

    auto AddPluginObject(
        const std::string& repo,
        const json&        object
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
};

#endif //HAVOCCLIENT_HCSTOREWIDGET_H
