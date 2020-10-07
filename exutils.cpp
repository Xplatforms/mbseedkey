#include "exutils.h"
#include "ecuseedkeydll.h"
#include <QGuiApplication>
#include <QDebug>
#include <QDir>
#include <QRegularExpression>

ExUTILS::ExUTILS(QObject *parent) : QObject(parent),
    p_sett(new QSettings(QStringLiteral("mbseedkey"),QStringLiteral("mbseedkey"),this)),
    p_seedkey_dll(Q_NULLPTR)
{
    this->loadDllsList();
}

Q_INVOKABLE void ExUTILS::searchFor(QString txt)
{
    if(txt.isEmpty())
    {
        this->dlls_list = this->p_bak_dlls_list;
        emit dllsChanged();
        return;
    }

    QStringList srch_list;
    foreach(auto el, this->p_bak_dlls_list)
    {
        if(el.contains(txt, Qt::CaseInsensitive))srch_list.append(el);
    }
    this->dlls_list = srch_list;
    emit dllsChanged();
}

void ExUTILS::loadDllsList(QString search_txt)
{
    Q_UNUSED(search_txt)
    this->dlls_list.clear();
    QDir dir(this->foldername());
    QFileInfoList list = dir.entryInfoList(QStringList() << "*.dll", QDir::Files |QDir::NoSymLinks |QDir::NoDotAndDotDot | QDir::Readable, QDir::Name );
    for(qint32 x = 0; x < list.count(); x++)
    {
        this->dlls_list.append(list.at(x).fileName());
        /*
        auto ecu_test = new ECUSeedKeyDLL(list.at(x).absoluteFilePath());
        if(ecu_test->isSeedKeyDll())
        {
            this->dlls_list.append(ecu_test->DLLName());
            qDebug() << "Adding DLL to list: " << ecu_test->DLLName();
        }
        delete ecu_test;
        */
    }
    this->p_bak_dlls_list = dlls_list;
    emit dllsChanged();
}

void ExUTILS::selectDll(qint32 idx)
{
    if(this->dlls_list.isEmpty() || idx >= this->dlls_list.count())return;

    auto dllname = this->dlls_list.at(idx);
    auto dll_path = QDir::toNativeSeparators(this->foldername()+QDir::toNativeSeparators("/")+dllname);

    if(this->p_seedkey_dll != Q_NULLPTR)
    {
        delete this->p_seedkey_dll;
        this->p_seedkey_dll = Q_NULLPTR;
    }

    this->p_seedkey_dll = new ECUSeedKeyDLL(dll_path);
    emit seedkeydllChanged();
}

ECUSeedKeyDLL * ExUTILS::seedkeydll()
{
    return this->p_seedkey_dll;
}

QString ExUTILS::foldername()
{
    if(this->p_sett->contains(QStringLiteral("current_folder")))
        return QDir::toNativeSeparators(this->p_sett->value(QStringLiteral("current_folder")).toString());
    else return QDir::toNativeSeparators(qApp->applicationDirPath());
}

void ExUTILS::setFoldername(QString fname)
{
    if(this->foldername().compare(QDir::toNativeSeparators(fname)) != 0)
    {
        this->p_sett->setValue(QStringLiteral("current_folder"), fname);
        emit foldernameChanged();
        this->loadDllsList();
    }
}


///TODO: make it better
QString ExUTILS::getInputMaskForSeedLen(qint32 len)
{
    qDebug() << "seed len" << len;
    QString str = "HH;0";
    len--;
    for(qint32 i = 0; i < len; i++)str.push_front("HH ");
    str.push_front(QChar('>'));
    qDebug() << " inputMask " << str;
    return str;
}
