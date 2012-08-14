#include <QtCore>
#include "windowscertificatevalidator.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // This validates the server certificate for the update server
    WindowsCertificateValidator::validateCertificate("petroules.com");

    // First argument is the executable name
    QStringList args = a.arguments();
    if (!args.isEmpty())
    {
        args.removeFirst();
    }

    QProcess::startDetached(QCoreApplication::applicationDirPath() + "/silverlock.exe", args, QDir::currentPath());

    return 0;
}
