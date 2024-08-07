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
    std::string name;
    QDir        plugin_dir;
    json        object;
    std::string access_token;
    std::mutex  mutex;
    bool        remove;

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
        const std::string&              repository,
        const std::vector<std::string>& plugins,
        const std::string&              credentials
    ) -> void;

    auto RegisterPlugin(
        const std::string& directory,
        const json&        object
    ) -> void;

    auto PluginProcess(
        const std::string& parent,
        const json&        object,
        const std::string& access_token
    ) -> std::optional<PluginView*>;

    auto PluginProcess(
        const std::string& parent,
        const json&        object,
        const bool         local = false,
        const std::string& access_token = ""
    ) -> std::optional<PluginView*>;

    auto PluginInstall(
        PluginView* plugin
    ) -> void;

signals:
    auto AddPlugin(
        const std::string& parent,
        const std::string& repo,
        PluginView*        plugin
    ) -> void;

    auto PluginIsInstalled(
        PluginView* plugin
    ) -> void;

    auto PluginIsInstalling(
        PluginView* plugin
    ) -> void;

    auto MessageBox(
        QMessageBox::Icon  icon,
        const std::string& title,
        const std::string& text
    ) -> void;
};

#endif //HAVOCCLIENT_HCSTOREPLUGINWORKER_H
