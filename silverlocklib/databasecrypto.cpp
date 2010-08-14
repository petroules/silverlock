#include "databasecrypto.h"
#include "database.h"
#include <botan/botan.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#define ALGO "AES"
#define HEADER "SILVERLOCK DATABASE FILE"
#define COMPRESSED "ZLIB"
#define UNCOMPRESSED "UNCOMPRESSED"

// Some black magic to catch problems at compile time
#define X_ASSERT(pred) switch(0){case 0:case pred:;}

using namespace Botan;

/*!
    \class DatabaseCrypto

    The DatabaseCrypto class provides methods for encrypting and decrypting strings using a
    string passphrase.

    It is intended for use by DatabaseReader and DatabaseWriter to encrypt and decrypt Silverlock
    database files.

    Data is encrypted using the AES (Rijndael) algorithm and the format uses a MAC to verify
    integrity when decrypting an encrypted string.
 */

/*!
    Constructs a new DatabaseCrypto.
 */
DatabaseCrypto::DatabaseCrypto(QObject *parent) :
    QObject(parent)
{
    // Make sure a Botan byte is the same size as a char (QByteArray's underlying storage)
    X_ASSERT(sizeof(Botan::byte) == sizeof(char));
}

/*!
    Encrypts \a data using \a password as the passphrase.

    \param data The data to encrypt.
    \param password The passphrase used to encrypt \a data.
    \param compressionLevel The level of compression to use on \a data. See Database::compression.
    \param error If this parameter is non-\c NULL, it will be set to DatabaseCrypto::NoError if the
    encryption process succeeded, or one of the other DatabaseCrypto::CryptoStatus enumeration
    constants if an error occurred.

    \return The encrypted data encoded in base 64, or an empty string if an error occurred.
 */
QString DatabaseCrypto::encrypt(const QString &data, const QString &password, int compressionLevel, CryptoStatus *error)
{
    const std::string algo = ALGO;
    const QString header = HEADER;

    Botan::LibraryInitializer init;
    Q_UNUSED(init);

    if (error)
    {
        *error = NoError;
    }

    std::string passphrase = password.toStdString();

    // Holds the output data
    QString out;

    try
    {
        const u32bit key_len = max_keylength_of(algo);
        const u32bit iv_len = block_size_of(algo);
        const bool compressed = compressionLevel != 0;

        AutoSeeded_RNG rng;

        std::auto_ptr<S2K> s2k(get_s2k("PBKDF2(SHA-1)"));
        s2k->set_iterations(8192);
        s2k->new_random_salt(rng, 8);

        SymmetricKey bc_key = s2k->derive_key(key_len, "BLK" + passphrase);
        InitializationVector iv = s2k->derive_key(iv_len, "IVL" + passphrase);
        SymmetricKey mac_key = s2k->derive_key(16, "MAC" + passphrase);

        // Write the standard file header and the salt encoded in base64
        out += QString("%1 %2 %3\n").arg(header).arg(Database::version().toString())
               .arg(compressed ? COMPRESSED : UNCOMPRESSED);
        out += QString::fromStdString(b64_encode(s2k->current_salt())) + "\n";

        Pipe pipe(new Fork(
            new Chain(new MAC_Filter("HMAC(SHA-1)", mac_key),
            new Base64_Encoder),
            new Chain(get_cipher(algo + "/CBC", bc_key, iv, ENCRYPTION),
            new Base64_Encoder(true))));

        // Write our input data to the pipe to process it - if compressionLevel = 0,
        // nothing will be compressed
        pipe.start_msg();
        QByteArray qCompressedData = qCompress(data.toUtf8(), compressionLevel);
        Botan::byte rawCompressedData[qCompressedData.length()];
        for (int i = 0; i < qCompressedData.length(); i++)
        {
            rawCompressedData[i] = qCompressedData[i];
        }

        pipe.write(rawCompressedData, qCompressedData.length());
        pipe.end_msg();

        // Get the encrypted data back from the pipe and write it to our output variable
        out += QString::fromStdString(pipe.read_all_as_string(0)) + "\n";
        out += QString::fromStdString(pipe.read_all_as_string(1));

        return out;
    }
    catch (Algorithm_Not_Found)
    {
        if (error)
        {
            *error = UnknownError;
        }

        return QString();
    }
    catch (std::exception &e)
    {
        std::cout << __FUNCTION__ << e.what() << std::endl;
        if (error)
        {
            *error = UnknownError;
        }

        return QString();
    }
}

/*!
    Decrypts \a data using \a password as the passphrase.

    \param data The data to decrypt, encoded in base 64.
    \param password The passphrase used to decrypt \a data.
    \param error If this parameter is non-\c NULL, it will be set to DatabaseCrypto::NoError if the
    decryption process succeeded, or one of the other DatabaseCrypto::CryptoStatus enumeration
    constants if an error occurred.

    \return The decrypted data, or an empty string if an error occurred.
 */
QString DatabaseCrypto::decrypt(const QString &data, const QString &password, CryptoStatus *error)
{
    const std::string algo = ALGO;
    const QString header = HEADER;

    Botan::LibraryInitializer init;
    Q_UNUSED(init);

    if (error)
    {
        *error = NoError;
    }

    std::string passphrase = password.toStdString();

    // Create a stream to read the input data
    QString dataCopy = data;
    QTextStream in(&dataCopy, QIODevice::ReadOnly);

    try
    {
        // Read the actual header line and store the expected prefix
        QString actualHeaderLine = in.readLine();
        QString headerLine = QString("%1 ").arg(header);

        // Try the old header first... just in case (TODO: we'll remove this as of version 1.1)
        bool wasCompressed = actualHeaderLine != "-------- SILVERLOCK DATABASE FILE --------";
        if (wasCompressed)
        {
            // If the actual header is less than (or equal!) to our expected one, there is no version
            // and is thus invalid, OR if the actual header doesn't start with our header, it's invalid
            if (actualHeaderLine.length() <= headerLine.length() || !actualHeaderLine.startsWith(headerLine))
            {
                if (error)
                {
                    *error = MissingHeader;
                }

                return QString();
            }

            // Everything after the "SILVERLOCK DATABASE FILE"
            QStringList theRest = actualHeaderLine.right(actualHeaderLine.length() - headerLine.length())
                    .split(' ', QString::SkipEmptyParts);
            if (theRest.count() == 2)
            {
                QVersion version(theRest[0]);
                if (version.simplified() != Database::version())
                {
                    if (error)
                    {
                        *error = UnsupportedVersion;
                    }

                    return QString();
                }
            }
            else
            {
                if (error)
                {
                    *error = MissingHeader;
                }

                return QString();
            }
        }

        std::string salt_str = in.readLine().toStdString();
        std::string mac_str = in.readLine().toStdString();

        const u32bit key_len = max_keylength_of(algo);
        const u32bit iv_len = block_size_of(algo);

        std::auto_ptr<S2K> s2k(get_s2k("PBKDF2(SHA-1)"));
        s2k->set_iterations(8192);
        s2k->change_salt(b64_decode(salt_str));

        SymmetricKey bc_key = s2k->derive_key(key_len, "BLK" + passphrase);
        InitializationVector iv = s2k->derive_key(iv_len, "IVL" + passphrase);
        SymmetricKey mac_key = s2k->derive_key(16, "MAC" + passphrase);

        Pipe pipe(new Base64_Decoder, get_cipher(algo + "/CBC", bc_key, iv, DECRYPTION),
            new Fork(0, new Chain(new MAC_Filter("HMAC(SHA-1)", mac_key), new Base64_Encoder)));

        // Read all our data into the pipe for decryption
        pipe.start_msg();
        pipe.write(in.readAll().toStdString());
        pipe.end_msg();

        // Read the message authentication code from the pipe
        // and verify that it matches what was in the file
        if (pipe.read_all_as_string(1) != mac_str)
        {
            if (error)
            {
                *error = VerificationFailed;
            }

            return QString();
        }

        // No errors were encountered; return the decrypted data
        SecureVector<Botan::byte> rawDecryptedData = pipe.read_all(0);
        QByteArray qDecryptedData(rawDecryptedData.size(), '\0');
        for (int i = 0; i < qDecryptedData.size(); i++)
        {
            qDecryptedData[i] = rawDecryptedData[i];
        }

        if (wasCompressed)
        {
            return QString::fromUtf8(qUncompress(qDecryptedData));
        }
        else
        {
            return QString::fromUtf8(qDecryptedData);
        }
    }
    catch (Algorithm_Not_Found)
    {
        if (error)
        {
            *error = UnknownError;
        }
    }
    catch (Decoding_Error)
    {
        if (error)
        {
            *error = DecodingError;
        }
    }
    catch (std::exception &e)
    {
        std::cout << __FUNCTION__ << e.what() << std::endl;
        if (error)
        {
            *error = UnknownError;
        }
    }

    return QString();
}

/*!
    Encodes the Botan byte array \a in as a base 64 string.

    \param in The Botan byte array to encode.
 */
std::string DatabaseCrypto::b64_encode(const SecureVector<Botan::byte> &in)
{
    Pipe pipe(new Base64_Encoder);
    pipe.process_msg(in);
    return pipe.read_all_as_string();
}

/*!
    Decodes the base 64 string \a in to a Botan byte array.

    \param in The base 64 string to decode.
 */
SecureVector<Botan::byte> DatabaseCrypto::b64_decode(const std::string &in)
{
    Pipe pipe(new Base64_Decoder);
    pipe.process_msg(in);
    return pipe.read_all();
}

/*!
    Returns a status message corresponding to \a status.

    \param status The status enumeration constant to get the message for.

    \return A localized status message corresponding to \a status; an empty string if \a status is
    DatabaseCrypto::NoError or an unknown value.
 */
QString DatabaseCrypto::statusMessage(CryptoStatus status)
{
    switch (status)
    {
        case MissingHeader:
            return tr("The file was missing its standard header.");
        case VerificationFailed:
            return tr("The message authentication codes were mismatched. The file may have been corrupted or tampered with.");
        case DecodingError:
            return tr("There was a problem decoding the file; either the password was invalid or the file may be corrupt.");
        case UnsupportedVersion:
            return tr("Unsupported database version.");
        case UnknownError:
            return tr("An unknown error occurred while decoding the file.");
        default:
            return QString();
    }
}
