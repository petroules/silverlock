#ifndef NATIVESAVEFILEDIALOG_H
#define NATIVESAVEFILEDIALOG_H

#include "nativefiledialog.h"

class NativeSaveFileDialog : public NativeFileDialog
{
    Q_OBJECT

public:
    explicit NativeSaveFileDialog(QWidget *parent = 0, const QString &caption = QString(), const QString &dir = QString(), const QString &filter = QString(), QString *selectedFilter = 0, QFileDialog::Options options = 0);
    virtual QDialog* createDialog();
};

#endif // NATIVESAVEFILEDIALOG_H
