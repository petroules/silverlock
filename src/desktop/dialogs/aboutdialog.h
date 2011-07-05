#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtGui>

namespace Ui
{
    class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = NULL);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;

private slots:
    void displayLicenseDialog();
    void displayBuildInformationDialog();
};

#endif // ABOUTDIALOG_H
