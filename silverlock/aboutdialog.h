#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtGui>

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;

private slots:
    void on_licensePushButton_clicked();
};

#endif // ABOUTDIALOG_H
