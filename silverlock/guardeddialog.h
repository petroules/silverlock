#ifndef GUARDEDDIALOG_H
#define GUARDEDDIALOG_H

#include <QtGui>

class GuardedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GuardedDialog(QWidget *parent = NULL);

protected:
    void accept();

    virtual void load() { }
    virtual void save() { }

    /*!
        Subclasses should reimplement this method to append errors, warnings and informative
        text to each of the parameters to indicate dialog status.

        This method is used by \a accept() to determine whether to allow the dialog to be accepted,
        and whether to alerting the user to information or require further input from them.

        \list
            \i If at least one error is added, the dialog cannot be accepted.
            \i If at least one warning is added and the user clicks no, the dialog will not be accepted.
            \i Any information messages added will be shown if there are no errors and all warnings
            are dismissed, with no bearing on whether the dialog may be accepted.
        \endlist
     */
    virtual void getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const = 0;
};

#endif // GUARDEDDIALOG_H
