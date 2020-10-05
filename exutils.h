#ifndef EXUTILS_H
#define EXUTILS_H

#include <QObject>
#include <QSettings>

class ExUTILS : public QObject
{
    Q_OBJECT
public:
    explicit ExUTILS(QObject *parent = nullptr);

    Q_PROPERTY(QString foldername READ foldername WRITE setFoldername NOTIFY foldernameChanged)
    QString foldername();
    void setFoldername(QString fname);

signals:
    void foldernameChanged();

private:
    QSettings *     p_sett;

};

#endif // EXUTILS_H
