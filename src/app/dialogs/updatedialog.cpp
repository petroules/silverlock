#include "updatedialog.h"
#include "ui_updatedialog.h"
#include "nativedialogs.h"
#include "silverlockpreferences.h"
#include <QtXml>

/*!
    \class UpdateDialog

    The UpdateDialog class provides a dialog allowing the user to check for, download, and install
    updated versions of the application.
 */

/*!
    Constructs a new UpdateDialog.

    \param parent The parent widget of the dialog.
 */
UpdateDialog::UpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateDialog), m_network(new QNetworkAccessManager(this))
{
    this->ui->setupUi(this);

    QObject::connect(this->m_network, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
    QObject::connect(this->m_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(checkReplyFinished(QNetworkReply*)));
}

/*!
    Destroys the dialog.
 */
UpdateDialog::~UpdateDialog()
{
    if (this->m_network)
    {
        delete this->m_network;
        this->m_network = NULL;
    }

    delete this->ui;
}

/*!
    Returns a value indicating whether the built-in automatic update
    mechanism is available on the current platform and configuration.
 */
bool UpdateDialog::automaticUpdatesSupported()
{
#ifdef Q_WS_MAC
    // Get the name of the application bundle and the directory that the application bundle resides in
    QDir bundleDir = QDir(QCoreApplication::applicationDirPath());
    bundleDir.cdUp();
    bundleDir.cdUp();
    QString bundleName = bundleDir.dirName();
    bundleDir.cdUp();

    // Verify that the bundle is code signed
    QProcess proc;
    proc.setWorkingDirectory(bundleDir.path());
    proc.start(QString("codesign --verify %1").arg(bundleName));
    proc.waitForFinished();

    // If the output from codesign says the object is not signed,
    // we're not running in the Mac App Store and can use our own automatic updates
    // Otherwise we let the Mac App Store keep the application up to date
    return proc.exitCode() != 0;
#elif (defined(Q_WS_WIN) && !defined(Q_WS_WINCE)) || (defined(Q_OS_LINUX) && defined(Q_WS_X11))
    // Windows and Linux/X11 support automatic updates by default
    return true;
#else
    // Any other/unknown platforms do not
    return false;
#endif
}

void UpdateDialog::check()
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->checkingPage);
    QNetworkReply *reply = this->m_network->get(QNetworkRequest(QString("https://www.petroules.com/version/silverlock")));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(checkError(QNetworkReply::NetworkError)));
}

void UpdateDialog::sslErrors(QNetworkReply *reply, QList<QSslError> errors)
{
    // Ensure the update dialog is visible so our message box doesn't go off screen
    this->show();

    QStringList sslErrors;
    foreach (QSslError error, errors)
    {
        sslErrors += error.errorString();
    }

    QString sslErrorString = sslErrors.join("</li><li>");
    int ret = NativeDialogs::warning(this, tr("SSL Errors"),
        tr("<p>SSL Errors for %1</p><ul><li>%2</li></ul><p>Do you want to ignore these errors?</p>")
        .arg(reply->url().toString()).arg(sslErrorString), tr("You should only ignore these errors if you are certain this message is in error."), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes)
    {
        reply->ignoreSslErrors();
    }
}

void UpdateDialog::checkReplyFinished(QNetworkReply *reply)
{
    QString error;
    int line;
    int column;

    QString platformCode;
#ifdef Q_OS_WIN
    platformCode = "windows";
#elif defined(Q_OS_MACX)
    platformCode = "macosx";
#endif

    QDomDocument doc;
    if (doc.setContent(QString(reply->readAll()), &error, &line, &column))
    {
        QDomElement release = doc.documentElement().firstChildElement();
        while (!release.isNull())
        {
            if (release.attribute("Platform") == platformCode)
            {
                QDomElement releaseChild = release.firstChildElement();
                while (!releaseChild.isNull())
                {
                    if (releaseChild.tagName() == "Version")
                    {
                        this->m_newVersion = QVersion(releaseChild.text()).simplified();
                    }
                    else if (releaseChild.tagName() == "DownloadURL")
                    {
                        this->m_downloadUrl = QUrl(releaseChild.text());
                    }
                    else if (releaseChild.tagName() == "DownloadChecksum")
                    {
                        this->m_checksum = releaseChild.text();
                    }

                    releaseChild = releaseChild.nextSiblingElement();
                }

                break;
            }

            release = release.nextSiblingElement();
        }
    }
    else
    {
        qDebug() << error << "on line:" << line << ", column:" << column;
    }

    if (this->m_newVersion.isValid() && !this->m_downloadUrl.isEmpty() && !this->m_checksum.isEmpty())
    {
        QVersion currentVersion = QVersion(QApplication::applicationVersion()).simplified();
        if (this->m_newVersion == currentVersion)
        {
            this->setAlreadyLatest(currentVersion);
        }
        else if (this->m_newVersion > currentVersion)
        {
            this->setUpdateAvailable(this->m_newVersion, currentVersion);
        }
        else
        {
            this->setError(tr("Your version of %1 appears to be newer than the latest available version. Please contact <a href=\"http://www.petroules.com/\">Petroules</a> for further assistance.").arg(qApp->applicationName()));
        }
    }
    else
    {
        QString error = tr("Unable to determine the latest version of %1.").arg(qApp->applicationName());
        if (!reply->errorString().isEmpty())
        {
            error += " " + reply->errorString();
        }

        this->setError(error);
    }
}

void UpdateDialog::checkError(QNetworkReply::NetworkError error)
{
    this->setError(tr("A network error was encountered when attempting to determine the latest version of %1. The error returned was: %2").arg(qApp->applicationName(), error));
}

void UpdateDialog::setError(const QString &error)
{
    // Make sure the update dialog's visible
    this->show();

    this->ui->stackedWidget->setCurrentWidget(this->ui->errorPage);
    this->ui->errorLabel->setText(tr("An error occurred during the update process:\n\n%1").arg(error));
}

void UpdateDialog::setAlreadyLatest(const QVersion &version)
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->alreadyLatestPage);
    this->ui->updateCheckCompletedLabel->setText(tr("Congratulations, you are running the latest version of %1 (<b>%2</b>)! There is no need to update.").arg(qApp->applicationName(), version.toString()));
}

void UpdateDialog::setUpdateAvailable(const QVersion &newVersion, const QVersion &currentVersion)
{
    // Make sure the update dialog's visible
    this->show();

    this->ui->stackedWidget->setCurrentWidget(this->ui->updateAvailablePage);
    this->ui->updateAvailableLabel->setText(tr("An update to version <b>%1</b> is available! You are running version <b>%2</b>. It is strongly recommended that you upgrade now. Continue?").arg(newVersion.toString(), currentVersion.toString()));
}

void UpdateDialog::acceptUpgrade()
{
    // Here we'll check if we already have the installer downloaded
    QFile tempFile(SilverlockPreferences::instance().updateInstallerPath());
    if (!tempFile.fileName().isEmpty() && tempFile.open(QIODevice::ReadOnly))
    {
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(tempFile.readAll());
        if (QString(hash.result().toHex()).compare(this->m_checksum) == 0)
        {
            // Verified, proceed to install page
            this->ui->stackedWidget->setCurrentWidget(this->ui->installPage);
            return;
        }

        // If we fail to verify integrity, carry on...
    }

    // If we fail to open, carry on...

    this->ui->stackedWidget->setCurrentWidget(this->ui->downloadUpdatePage);
    this->ui->downloadingLabel->setText(tr("Contacting download server..."));

    this->m_download = new QNetworkAccessManager(this);
    QNetworkReply *reply = this->m_download->get(QNetworkRequest(this->m_downloadUrl));

    QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    QObject::connect(this->m_download, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadReplyFinished(QNetworkReply*)));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadError(QNetworkReply::NetworkError)));
}

void UpdateDialog::downloadProgress(qint64 received, qint64 total)
{
    this->ui->downloadingLabel->setText(tr("Downloading %1 version <b>%2</b> from %3").arg(qApp->applicationName(), this->m_newVersion.toString(), this->m_downloadUrl.toString()));

    this->ui->progressBar->setTextVisible(true);
    this->ui->progressBar->setValue(received);
    this->ui->progressBar->setMaximum(total);
}

void UpdateDialog::downloadReplyFinished(QNetworkReply *reply)
{
#ifdef Q_WS_WIN
    QTemporaryFile temp(QDir::tempPath() + "/qt_temp.XXXXXX.msi");
#elif defined(Q_WS_MAC)
    QTemporaryFile temp(QDir::tempPath() + "/qt_temp.XXXXXX.dmg");
#elif defined(Q_OS_LINUX)
    QTemporaryFile temp(QDir::tempPath() + "/qt_temp.XXXXXX.tar.gz");
#else
    #error "Implement automatic updater for this platform!"
#endif
    temp.setAutoRemove(false);
    if (temp.open())
    {
        SilverlockPreferences::instance().setUpdateInstallerPath((this->m_file = temp.fileName()));

        QByteArray fileData = reply->readAll();
        temp.write(fileData);

        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(fileData);
        if (QString(hash.result().toHex()).compare(this->m_checksum) == 0)
        {
            this->ui->stackedWidget->setCurrentWidget(this->ui->installPage);
        }
        else
        {
            this->setError(tr("Failed to verify file integrity; data may have been corrupted during the download process."));
        }
    }
    else
    {
        this->setError(tr("Failed to open temporary file to download installer."));
    }
}

void UpdateDialog::downloadError(QNetworkReply::NetworkError error)
{
    this->setError(QString(tr("A network error was encountered when downloading the latest version of Silverlock. The error returned was: %1")).arg(error));
}

void UpdateDialog::on_cancelCheckingPushButton_clicked()
{
    if (this->m_network)
    {
        delete this->m_network;
        this->m_network = NULL;
    }

    this->reject();
}

void UpdateDialog::on_alreadyLatestPushButton_clicked()
{
    this->reject();
}

void UpdateDialog::on_acceptUpgradePushButton_clicked()
{
    this->acceptUpgrade();
}

void UpdateDialog::on_rejectUpgradePushButton_clicked()
{
    this->reject();
}

void UpdateDialog::on_cancelDownloadPushButton_clicked()
{
    if (this->m_download)
    {
        delete this->m_download;
        this->m_download = NULL;
    }

    this->reject();
}

void UpdateDialog::on_installPushButton_clicked()
{
    QApplication::closeAllWindows();

    QString nativeFilePath = QDir::toNativeSeparators(this->m_file);

    QProcess process;
#ifdef Q_WS_WIN
    QStringList args;
    args << "/i";
    args << nativeFilePath;
    if (!process.startDetached("msiexec", QStringList(args)))
#elif defined(Q_WS_MAC)
    if (!process.startDetached("open", QStringList(nativeFilePath)))
#elif defined(Q_OS_LINUX)
    if (!process.startDetached("xdg-open", QStringList(nativeFilePath)))
#else
#error "Implement automatic updater for this platform!"
#endif
    {
        NativeDialogs::critical(this, tr("Error"), QString(tr("Failed to start the installer process. The error returned was: %1")).arg(process.errorString()));
        this->reject();
    }
}

void UpdateDialog::on_acknowledgeErrorPushButton_clicked()
{
    this->reject();
}
