#ifndef HAVOCCLIENT_HCCONSOLE_H
#define HAVOCCLIENT_HCCONSOLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class HcConsole : public QWidget
{
    Q_OBJECT

    QGridLayout* gridLayout  = nullptr;
    QTextEdit*   Console     = nullptr;

public:
    QLabel*    LabelHeader = nullptr;
    QLabel*    LabelBottom = nullptr;
    QLabel*    LabelInput  = nullptr;
    QLineEdit* Input       = nullptr;

    explicit HcConsole(
        QWidget* parent = nullptr
    );

    auto setHeaderLabel(
        const QString& text
    ) -> void;

    auto setBottomLabel(
        const QString& text
    ) -> void;

    auto setInputLabel(
        const QString& text
    ) -> void;

    auto appendConsole(
        const QString& text
    ) -> void;

    static auto formatString(
        const std::string& format,
        const std::string& output = ""
    ) -> std::string;

    virtual auto inputEnter() -> void;
};

QT_END_NAMESPACE

#endif //HAVOCCLIENT_HCCONSOLE_H
