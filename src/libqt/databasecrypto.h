#ifndef DATABASECRYPTO_H
#define DATABASECRYPTO_H

#include "silverlocklib_global.h"
#include <QtCore>

// Little fix for the SecureVector template on MSVC...
#ifdef min
#undef min
#endif

namespace Botan
{
    template <typename T> class SecureVector;
    typedef unsigned char byte;
}

class SILVERLOCKLIBSHARED_EXPORT DatabaseCrypto : public QObject
{
    Q_OBJECT

public:
    /*!
        Enumerates possible status codes returned from the encryption and decryption process.
     */
    enum CryptoStatus
    {
        /*!
            No error was encountered during the encryption/decryption process.
         */
        NoError,

        /*!
            The file header was missing when attempting to decrypt the file.
         */
        MissingHeader,

        /*!
            The message authentication codes were found to be mismatched after decrypting the file.
         */
        VerificationFailed,

        /*!
            A decoding error occurred - this indicates an incorrect passphrase or corrupted file.
         */
        DecodingError,

        /*!
            An unsupported database version was encountered.
         */
        UnsupportedVersion,

        /*!
            An unknown error occurred during the encryption/decryption process.
         */
        UnknownError
    };

    static QString botanVersion();
    static QString encrypt(const QString &file, const QString &password, int compressionLevel, CryptoStatus *error = NULL);
    static QString decrypt(const QString &file, const QString &password, CryptoStatus *error = NULL);
    static QString statusMessage(CryptoStatus status);

private:
    explicit DatabaseCrypto(QObject *parent = 0);
    static std::string b64_encode(const Botan::SecureVector<Botan::byte> &in);
    static Botan::SecureVector<Botan::byte> b64_decode(const std::string &in);
};

#endif // DATABASECRYPTO_H
