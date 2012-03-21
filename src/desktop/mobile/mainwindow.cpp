#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QCoreApplication>
#include <QtNetwork>
#include <petroules-utilities.h>
#include <silverlocklib.h>

class MainWindow::Private
{
public:
    Private()
    {
        db = NULL;
    }

    ~Private()
    {
        if (db)
        {
            delete db;
        }
    }

    Database *db;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    d = new Private();
}

MainWindow::~MainWindow()
{
    delete d;
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}

void MainWindow::on_pushButton_clicked()
{
    /*QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(loadFromUrlCompleted(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("http://www.jakepetroules.com/files/Jake.sdbx")));*/

    QList<QUrl> urls;
    urls << QUrl::fromLocalFile("/mnt/sdcard/Android/data/com.dropbox.android/files/scratch"); // Dropbox

    QFileDialog dialog;
    dialog.setSidebarUrls(urls);
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString fileName = dialog.selectedFiles().first();

        bool ok;
        QString password = QInputDialog::getText(this, tr("Enter password"), tr("Password:"), QLineEdit::Password, QString(), &ok);
        if (ok)
        {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly))
            {
                load(file, password);
            }
        }
    }
}

void MainWindow::loadFromUrlCompleted(QNetworkReply *reply)
{
    load(*reply, QString());
}

void MainWindow::load(QIODevice &device, const QString &password)
{
    if (d->db)
    {
        delete d->db;
    }

    DatabaseReader reader;
    d->db = reader.read(device, password);
    if (!d->db)
    {
        ui->label->setText("Failed - " + reader.errorString());
    }
}

void MainWindow::on_searchLineEdit_textChanged(const QString &arg1)
{
    if (d->db)
    {
        SearchParameters p;
        p.searchPattern = arg1;
        QList<Entry*> entries = d->db->findEntries(p);

        QString output;
        foreach (Entry* entry, entries)
        {
            output += QString("<b>Title:</b> %1<br />").arg(entry->title());
            output += QString("<b>Username:</b> %1<br />").arg(entry->username());
            output += QString("<b>Password:</b> %1<br />").arg(entry->password());
            output += QString("<b>URL:</b> <a href='%1'>%2</a><br />").arg(entry->url().toString(), entry->url().toString(QUrl::RemoveScheme));

            QMapIterator<QString, QString> i2(entry->customFields());
            while (i2.hasNext())
            {
                i2.next();
                output += QString("<b>%1:</b> %2<br />").arg(i2.key(), i2.value());
            }

            QMapIterator<QString, QString> i(entry->recoveryInfo());
            if (i.hasNext())
            {
                output += QString("<b>Recovery info</b>:<br />");
            }

            while (i.hasNext())
            {
                i.next();
                output += i.key() + " <i>" + i.value() + "</i><br />";
            }

            output += entry->notes();

            output += "\n<hr />";
        }

        ui->label->setText(output);
    }
}
