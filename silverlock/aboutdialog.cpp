#include "aboutdialog.h"
#include "version.h"

void AboutDialog::show(QWidget* parent, const QString& title, const QString& description)
{
    QString aboutInformation = QString("<h1>%1 %2</h1><small>Version %3 &bull; Qt %4 &bull; %5%6 %7-bit %8</small><p>%9, %10</p>")
                               .arg(VER_FILEDESCRIPTION_STR)
                               .arg(VER_PRODUCTVERSION_STR)
                               .arg(VER_FILEVERSION_STR)
                               .arg(QT_VERSION_STR)
                               .arg(AboutDialog::osString())
                                #ifdef Q_WS_X11
                               .arg(" X11")
                                #else
                               .arg("")
                                #endif
                               .arg(AboutDialog::wordSize())
                               .arg(AboutDialog::byteOrder())
                               .arg(VER_LEGALCOPYRIGHT_STR)
                               .arg(VER_LEGALTRADEMARKS1_STR);

    aboutInformation.append(description);

    QMessageBox::about(parent, title, aboutInformation);
}

QString AboutDialog::osString()
{
    #ifdef Q_OS_WIN32
    switch (QSysInfo::WindowsVersion)
    {
        case QSysInfo::WV_WINDOWS7:
            return "Windows 7 (6.1)";
        case QSysInfo::WV_VISTA:
            return "Windows Vista (6.0)";
        case QSysInfo::WV_2003:
            return "Windows Server 2003 (5.2)";
        case QSysInfo::WV_XP:
            return "Windows XP (5.1)";
        default:
            return "Windows";
    }
    #endif

    #ifdef Q_OS_MAC
    switch (QSysInfo::MacintoshVersion)
    {
        case QSysInfo::MV_SNOWLEOPARD:
            return "Mac OSX 10.6 Snow Leopard";
        case QSysInfo::MV_LEOPARD:
            return "Mac OSX 10.5 Leopard";
        case QSysInfo::MV_TIGER:
            return "Mac OSX 10.4 Tiger";
        case QSysInfo::MV_PANTHER:
            return "Mac OSX 10.3 Panther";
        default:
            return "Mac OS";
    }
    #endif

    #ifdef Q_OS_LINUX
    return "Linux";
    #endif

    #ifdef Q_OS_FREEBSD
    return "FreeBSD";
    #endif

    #ifdef Q_OS_NETBSD
    return "NetBSD";
    #endif

    #ifdef Q_OS_OPENBSD
    return "OpenBSD";
    #endif

    #ifdef Q_OS_SOLARIS
    return "Oracle Solaris";
    #endif

    #ifdef Q_OS_BSDI
    return "BSD/OS";
    #endif

    #ifdef Q_OS_BSD4
    return "BSD 4.4";
    #endif

    #ifdef Q_OS_UNIX
    return "Unix";
    #endif
}

QString AboutDialog::byteOrder()
{
    switch (QSysInfo::ByteOrder)
    {
        case QSysInfo::LittleEndian:
            return "Little-endian";
        case QSysInfo::BigEndian:
            return "Big-endian";
    }
}

int AboutDialog::wordSize()
{
    return QSysInfo::WordSize;
}
