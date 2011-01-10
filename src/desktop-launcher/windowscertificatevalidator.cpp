#include "windowscertificatevalidator.h"
#include <stdio.h>
#include <wincrypt.h>

bool WindowsCertificateValidator::validateCertificate(const QString &host, int port)
{
    QSslSocket *socket = new QSslSocket();
    QObject::connect(socket, SIGNAL(sslErrors(QList<QSslError>)), socket, SLOT(ignoreSslErrors()));
    socket->connectToHostEncrypted(host, port);
    if (socket->waitForEncrypted())
    {
        bool isValid = validateCertificateInternal(socket);
        delete socket;
        return isValid;
    }

    delete socket;
    return false;
}

bool WindowsCertificateValidator::validateCertificateInternal(const QSslSocket *socket)
{
    QSslCertificate cert = socket->peerCertificate();
    QByteArray data = cert.toDer();

    // http://www.proper.com/root-cert-problem/
    // Adapted from http://msdn.microsoft.com/en-us/library/aa382042.aspx
    PCCERT_CHAIN_CONTEXT pChainContext = NULL;
    PCCERT_CONTEXT pCertContext = NULL;

    CERT_ENHKEY_USAGE EnhkeyUsage;
    EnhkeyUsage.cUsageIdentifier = 0;
    EnhkeyUsage.rgpszUsageIdentifier = NULL;

    CERT_USAGE_MATCH CertUsage;
    CertUsage.dwType = USAGE_MATCH_TYPE_AND;
    CertUsage.Usage = EnhkeyUsage;

    CERT_CHAIN_PARA ChainPara;
    ChainPara.cbSize = sizeof(CERT_CHAIN_PARA);
    ChainPara.RequestedUsage = CertUsage;

    bool isValid = false;

    if ((pCertContext = reinterpret_cast<PCCERT_CONTEXT>(CertCreateCertificateContext(X509_ASN_ENCODING, reinterpret_cast<const BYTE*>(data.constData()), data.size()))))
    {
        if (CertGetCertificateChain(NULL, pCertContext, NULL, NULL, &ChainPara, 0, NULL, &pChainContext))
        {
            qDebug() << cert.subjectInfo(QSslCertificate::CommonName);
            qDebug() << QString("The size of the chain context is %1.").arg(pChainContext->cbSize);
            qDebug() << QString("%1 simple chains found.").arg(pChainContext->cChain);
            qDebug() << "Error status for the chain:" << certificateTrustStatusErrorString(pChainContext->TrustStatus.dwErrorStatus);
            qDebug() << "Info status for the chain:" << certificateTrustStatusInfoString(pChainContext->TrustStatus.dwInfoStatus);

            isValid = (pChainContext->TrustStatus.dwErrorStatus == CERT_TRUST_NO_ERROR);

            CertFreeCertificateChain(pChainContext);
        }
        else
        {
            qDebug() << "The chain could not be created.";
        }

        CertFreeCertificateContext(pCertContext);
    }

    return isValid;
}

QString WindowsCertificateValidator::certificateTrustStatusErrorString(DWORD errorStatus)
{
    switch (errorStatus)
    {
    case CERT_TRUST_NO_ERROR:
        return "No error found for this certificate or chain.";
    case CERT_TRUST_IS_NOT_TIME_VALID:
        return "This certificate or one of the certificates in the certificate chain is not time-valid.";
    case CERT_TRUST_IS_REVOKED:
        return "Trust for this certificate or one of the certificates in the certificate chain has been revoked.";
    case CERT_TRUST_IS_NOT_SIGNATURE_VALID:
        return "The certificate or one of the certificates in the certificate chain does not have a valid signature.";
    case CERT_TRUST_IS_NOT_VALID_FOR_USAGE:
        return "The certificate or certificate chain is not valid in its proposed usage.";
    case CERT_TRUST_IS_UNTRUSTED_ROOT:
        return "The certificate or certificate chain is based on an untrusted root.";
    case CERT_TRUST_REVOCATION_STATUS_UNKNOWN:
        return "The revocation status of the certificate or one of the certificates in the certificate chain is unknown.";
    case CERT_TRUST_IS_CYCLIC:
        return "One of the certificates in the chain was issued by a certification authority that the original certificate had certified.";
    case CERT_TRUST_IS_PARTIAL_CHAIN:
        return "The certificate chain is not complete.";
    case CERT_TRUST_CTL_IS_NOT_TIME_VALID:
        return "A CTL used to create this chain was not time-valid.";
    case CERT_TRUST_CTL_IS_NOT_SIGNATURE_VALID:
        return "A CTL used to create this chain did not have a valid signature.";
    case CERT_TRUST_CTL_IS_NOT_VALID_FOR_USAGE:
        return "A CTL used to create this chain is not valid for this usage.";
    default:
        return QString("Unknown error: %1.").arg(errorStatus);
    }
}

QString WindowsCertificateValidator::certificateTrustStatusInfoString(DWORD infoStatus)
{
    switch (infoStatus)
    {
    case 0:
        return "No information status reported.";
    case CERT_TRUST_HAS_EXACT_MATCH_ISSUER:
        return "An exact match issuer certificate has been found for this certificate.";
    case CERT_TRUST_HAS_KEY_MATCH_ISSUER:
        return "A key match issuer certificate has been found for this certificate.";
    case CERT_TRUST_HAS_NAME_MATCH_ISSUER:
        return "A name match issuer certificate has been found for this certificate.";
    case CERT_TRUST_IS_SELF_SIGNED:
        return "This certificate is self-signed.";
    case CERT_TRUST_IS_COMPLEX_CHAIN:
        return "The certificate chain created is a complex chain.";
    default:
        return QString("Unknown information status: %1.").arg(infoStatus);
    }
}
