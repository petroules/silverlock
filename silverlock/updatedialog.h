#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QtGui>
#include <QtNetwork>
#include <liel.h>

namespace Ui {
    class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = 0);
    ~UpdateDialog();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::UpdateDialog *ui;
    QNetworkAccessManager *m_network;
    QNetworkAccessManager *m_download;
    QString m_file;
    QVersion m_newVersion;
    QString m_checksum;
    QUrl m_downloadUrl;

    void setError(const QString &error);
    void setAlreadyLatest(const QVersion &version);
    void setUpdateAvailable(const QVersion &newVersion, const QVersion &currentVersion);
    void acceptUpgrade();

private slots:
    void on_installPushButton_clicked();
    void on_cancelDownloadPushButton_clicked();
    void on_acknowledgeErrorPushButton_clicked();
    void on_rejectUpgradePushButton_clicked();
    void on_acceptUpgradePushButton_clicked();
    void on_alreadyLatestPushButton_clicked();
    void on_cancelCheckingPushButton_clicked();
    void checkReplyFinished(QNetworkReply *reply);
    void checkError(QNetworkReply::NetworkError error);
    void downloadProgress(qint64 received, qint64 total);
    void downloadReplyFinished(QNetworkReply *reply);
    void downloadError(QNetworkReply::NetworkError error);
};

#endif // UPDATEDIALOG_H
