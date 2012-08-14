#ifndef WINDOWSCERTIFICATEVALIDATOR_H
#define WINDOWSCERTIFICATEVALIDATOR_H

#include <QtNetwork>
#include <windows.h>

class WindowsCertificateValidator
{
public:
    static bool validateCertificate(const QString &host, int port = 443);

    static QString certificateTrustStatusErrorString(DWORD errorStatus);
    static QString certificateTrustStatusInfoString(DWORD infoStatus);

private:
    WindowsCertificateValidator() { }
    static bool validateCertificateInternal(const QSslSocket *socket);
};

#endif // WINDOWSCERTIFICATEVALIDATOR_H
