#include "applicationinfo.h"
#include "version.h"
#include <liel.h>

ApplicationInfo::ApplicationInfo(QObject *parent) :
    QObject(parent)
{
}

QString ApplicationInfo::organizationName()
{
    return VER_COMPANYNAME_STR;
}

QString ApplicationInfo::organizationDomain()
{
    return VER_COMPANYDOMAIN_STR;
}

QString ApplicationInfo::applicationName()
{
    return APP_DISPLAYNAME;
}

QString ApplicationInfo::unixName()
{
    return APP_UNIXNAME;
}

QVersion ApplicationInfo::applicationVersion()
{
    return QVersion(VER_PRODUCTVERSION_STR);
}

QVersion ApplicationInfo::fileVersion()
{
    return QVersion(VER_FILEVERSION_STR);
}

QString ApplicationInfo::copyright()
{
    return VER_LEGALCOPYRIGHT_STR;
}

QString ApplicationInfo::copyrightLink()
{
    return copyright().replace(organizationName(), QString("<a href=\"http://www.%1\">%2</a>")
        .arg(organizationDomain())
        .arg(organizationName()));
}

QString ApplicationInfo::trademarks()
{
    return VER_LEGALTRADEMARKS1_STR;
}

void ApplicationInfo::initialize(QApplication &application)
{
    application.setOrganizationName(organizationName());
    application.setOrganizationDomain(organizationName());
    application.setApplicationName(applicationName());
    application.setApplicationVersion(applicationVersion().toString());
}
