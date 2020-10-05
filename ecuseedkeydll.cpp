#include "ecuseedkeydll.h"
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <signal.h>

auto tmp_ret_def_seedkey_len = [](int){return 4;};

QString GetLastErrorAsString()
{
    //Get the error message, if any.
    auto errorMessageID = ::GetLastError();
    if(errorMessageID == 0)return QStringLiteral(""); //No error message has been recorded

    LPWSTR messageBuffer = Q_NULLPTR;
    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer, 0, NULL);

    auto message = QString::fromWCharArray(messageBuffer, size);
    LocalFree(messageBuffer);

    return message;
}
