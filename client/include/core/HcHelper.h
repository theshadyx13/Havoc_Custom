#ifndef HAVOCCLIENT_HCHELPER_H
#define HAVOCCLIENT_HCHELPER_H

#include <Common.h>

namespace Helper {
    inline auto IsMainThread() -> bool {
        return QThread::currentThread() == QCoreApplication::instance()->thread();
    }

    auto MessageBox(
        QMessageBox::Icon Icon,
        std::string title,
        std::string text
    ) -> void;

    auto FileRead(
        const QString& FilePath
    ) -> QByteArray;

    auto GrayScaleIcon(
        QImage image
    ) -> QIcon;
};

#endif //HAVOCCLIENT_HCHELPER_H
