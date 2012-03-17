#include <QtCore/QString>
#include <QtTest/QtTest>
#include <silverlocklib.h>

class LibqttestTest : public QObject
{
    Q_OBJECT

public:
    LibqttestTest();

private Q_SLOTS:
    void testDecryption();
};

LibqttestTest::LibqttestTest()
{
}

void LibqttestTest::testDecryption()
{
    QFile file("/Users/jakepetroules/Desktop/silverlock/TestDatabasePassword.sdbx");
    if (file.open(QIODevice::ReadOnly))
    {
        DatabaseCrypto::CryptoStatus status;
        QString data = DatabaseCrypto::decrypt(file.readAll(), "test", &status);

        qDebug() << data;
        qDebug() << DatabaseCrypto::statusMessage(status);
    }

    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(LibqttestTest);

#include "tst_libqttesttest.moc"
