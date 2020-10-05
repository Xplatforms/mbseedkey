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


ECUSeedKeyDLL::ECUSeedKeyDLL(QString dll_path, QObject *parent) : QObject(parent),
    GetConfiguredAccessTypes(Q_NULLPTR), GetSeedLength(Q_NULLPTR), GetKeyLength(Q_NULLPTR),  GetECUName(Q_NULLPTR),
    GenerateKeyExOpt(Q_NULLPTR),
    p_dllHandle(Q_NULLPTR),
    p_dllPath(dll_path)
{
    if(dll_path.isEmpty())
    {
        this->setErrorMsg(tr("DLL path is empty!"));
        return;
    }

    QFileInfo info(this->p_dllPath);
    if(!info.exists() || !info.isFile() || info.isSymLink())
    {
        this->setErrorMsg(tr("DLL file doesn't exist or wrong."));
        return;
    }

    this->p_ecu_name = info.baseName();
    QString dll_info_str = "[" + info.fileName() + "] ";
    qInfo() <<  dll_info_str << "Loading ...";

    this->p_dllHandle = LoadLibraryW(QDir::toNativeSeparators(info.absoluteFilePath()).toStdWString().data());
    if(!this->p_dllHandle)
    {
        auto error_str = GetLastErrorAsString();
        this->setErrorMsg(tr("Could not load DLL file. Reason: ")+error_str);
        qInfo() << dll_info_str << this->errorMsg();
        return;
    }
    this->loadDllfuncs();
}

ECUSeedKeyDLL::~ECUSeedKeyDLL()
{
    if(this->p_dllHandle != Q_NULLPTR)::FreeLibrary(this->p_dllHandle);
}

void ECUSeedKeyDLL::loadDllfuncs()
{

}
