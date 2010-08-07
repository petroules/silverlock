#ifndef APPLICATIONINFO_H
#define APPLICATIONINFO_H

#include <QObject>

class QVersion;

class ApplicationInfo : public QObject
{
    Q_OBJECT

public:
    static QString organizationName();
    static QString organizationDomain();
    static QString applicationName();
    static QString unixName();
    static QVersion applicationVersion();
    static QVersion fileVersion();
    static QString copyright();
    static QString copyrightLink();
    static QString trademarks();
    static void initialize(QApplication &application);

private:
    explicit ApplicationInfo(QObject *parent = NULL);
};

#endif // APPLICATIONINFO_H
