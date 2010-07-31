#ifndef LICENSEDIALOG_H
#define LICENSEDIALOG_H

#include <QtGui>

namespace Ui {
    class LicenseDialog;
}

class LicenseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LicenseDialog(QWidget *parent = 0);
    ~LicenseDialog();

private:
    Ui::LicenseDialog *ui;

    QString getLicense(const QString &license);
};

#endif // LICENSEDIALOG_H
