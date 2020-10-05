#include "exutils.h"
#include <QGuiApplication>
#include <QDir>

ExUTILS::ExUTILS(QObject *parent) : QObject(parent),
    p_sett(new QSettings(QStringLiteral("mbseedkey"),QStringLiteral("mbseedkey"),this)){}

QString ExUTILS::foldername()
{
    if(this->p_sett->contains(QStringLiteral("current_folder")))
        return QDir::toNativeSeparators(this->p_sett->value(QStringLiteral("current_folder")).toString());
    else return QDir::toNativeSeparators(qApp->applicationDirPath());
}

void ExUTILS::setFoldername(QString fname)
{
    this->p_sett->setValue(QStringLiteral("current_folder"), fname);
    emit foldernameChanged();
}
