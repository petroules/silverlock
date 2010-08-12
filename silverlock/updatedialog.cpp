#include "updatedialog.h"
#include "ui_updatedialog.h"
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

    QNetworkReply *reply = this->m_network->get(QNetworkRequest(ApplicationInfo::url(ApplicationInfo::ApplicationUpdate)));
    QObject::connect(this->m_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(checkReplyFinished(QNetworkReply*)));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(checkError(QNetworkReply::NetworkError)));
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

void UpdateDialog::checkReplyFinished(QNetworkReply *reply)
{
    QDomDocument doc;
    if (doc.setContent(QString(reply->readAll())))
    {
        QDomNode node = doc.documentElement().firstChild();
        while (!node.isNull())
        {
            QDomElement e = node.toElement();
            if (!e.isNull())
            {
                if (e.tagName() == "Version")
                {
                    this->m_newVersion = QVersion(e.text()).simplified();
                }
                else if (e.tagName() == "DownloadURL")
                {
                    this->m_downloadUrl = QUrl(e.text());
                }
                else if (e.tagName() == "DownloadChecksum")
                {
                    this->m_checksum = e.text();
                }
            }

            node = node.nextSibling();
        }
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
            this->setError(tr("Your version of Silverlock appears to be newer than the latest available version. Please contact Petroules Enterprises for further assistance."));
        }
    }
    else
    {
        this->setError(tr("Unable to determine the latest version of Silverlock."));
    }
}

void UpdateDialog::checkError(QNetworkReply::NetworkError error)
{
    this->setError(QString(tr("A network error was encountered when attempting to determine the latest version of Silverlock. The error returned was: %1")).arg(error));
}

void UpdateDialog::setError(const QString &error)
{
    // Make sure the update dialog's visible
    this->show();

    this->ui->stackedWidget->setCurrentWidget(this->ui->errorPage);
    this->ui->errorLabel->setText(QString(tr("An error occurred during the update process:\n\n%1")).arg(error));
}

void UpdateDialog::setAlreadyLatest(const QVersion &version)
{
    this->ui->stackedWidget->setCurrentWidget(this->ui->alreadyLatestPage);
    this->ui->updateCheckCompletedLabel->setText(QString(tr("Congratulations, you are running the latest version of Silverlock (<b>%1</b>)! There is no need to update.")).arg(version.toString()));
}

void UpdateDialog::setUpdateAvailable(const QVersion &newVersion, const QVersion &currentVersion)
{
    // Make sure the update dialog's visible
    this->show();

    this->ui->stackedWidget->setCurrentWidget(this->ui->updateAvailablePage);
    this->ui->updateAvailableLabel->setText(QString(tr("An update to version <b>%1</b> is available! You are running version <b>%2</b>. It is strongly recommended that you upgrade now. Continue?")).arg(newVersion.toString()).arg(currentVersion.toString()));
}

void UpdateDialog::acceptUpgrade()
{
    // Here we'll check if we already have the installer downloaded
    QFile tempFile(SilverlockPreferences::instance().updateInstallerPath());
    if (tempFile.open(QIODevice::ReadOnly))
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
    this->ui->downloadingLabel->setText(QString(tr("Downloading Silverlock version <b>%1</b> from %2")).arg(this->m_newVersion.toString()).arg(this->m_downloadUrl.toString()));

    this->ui->progressBar->setTextVisible(true);
    this->ui->progressBar->setValue(received);
    this->ui->progressBar->setMaximum(total);
}

void UpdateDialog::downloadReplyFinished(QNetworkReply *reply)
{
    QTemporaryFile temp;
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

    this->setEnabled(false);
    this->reject();
}

void UpdateDialog::on_alreadyLatestPushButton_clicked()
{
    this->setEnabled(false);
    this->reject();
}

void UpdateDialog::on_acceptUpgradePushButton_clicked()
{
    this->acceptUpgrade();
}

void UpdateDialog::on_rejectUpgradePushButton_clicked()
{
    this->setEnabled(false);
    this->reject();
}

void UpdateDialog::on_cancelDownloadPushButton_clicked()
{
    if (this->m_download)
    {
        delete this->m_download;
        this->m_download = NULL;
    }

    this->setEnabled(false);
    this->reject();
}

void UpdateDialog::on_installPushButton_clicked()
{
    QApplication::closeAllWindows();

    QProcess process;
    if (!process.startDetached(this->m_file))
    {
        QMessageBox::critical(this, tr("Error"), QString(tr("Failed to start the installer process. The error returned was: %1")).arg(process.errorString()));
        this->reject();
    }
}

void UpdateDialog::on_acknowledgeErrorPushButton_clicked()
{
    this->setEnabled(false);
    this->reject();
}
