#ifndef HAVOCCLIENT_HCSTOREPLUGINWORKER_H
#define HAVOCCLIENT_HCSTOREPLUGINWORKER_H

#include <Common.h>

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

struct PluginView {
    std::string parent;
    std::string repo;
    json        object;

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

class HcStorePluginWorker : public QThread {
    Q_OBJECT

public:
    void run() {}

public slots:
    auto RegisterRepository(
        const std::string& repository
    ) -> void;

    auto PluginProcess(
        const std::string& parent,
        const json&        object
    ) -> void;

signals:
    auto AddPlugin(
        const std::string& parent,
        const std::string& repo,
        PluginView*        plugin
    ) -> void;
};

#endif //HAVOCCLIENT_HCSTOREPLUGINWORKER_H
