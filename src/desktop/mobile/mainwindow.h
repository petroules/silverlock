#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtNetwork/QNetworkReply>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

private slots:
    void on_pushButton_clicked();
    void loadFromUrlCompleted(QNetworkReply*);
    void load(QIODevice &device, const QString &password);

    void on_searchLineEdit_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    class Private;
    Private *d;
};

#endif // MAINWINDOW_H
