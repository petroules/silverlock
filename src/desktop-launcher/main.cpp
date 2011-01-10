#include <QtCore>
#include "windowscertificatevalidator.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // This validates the server certificate for the update server
    if (WindowsCertificateValidator::validateCertificate("petroules.com"))
    {
        QProcess::startDetached("silverlock.exe", a.arguments());
    }

    return 0;
}
