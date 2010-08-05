#ifndef GUARDEDDIALOG_H
#define GUARDEDDIALOG_H

#include <QtGui>

class GuardedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GuardedDialog(QWidget *parent = NULL);
    virtual ~GuardedDialog();

protected:
    void accept();

    /*!
        Loads information from external objects required to initialize the dialog state.
     */
    virtual void load() { }

    /*!
        Commits any changes made in the dialog to external objects.
     */
    virtual void save() { }

    /*!
        This method is used by \a accept() to determine whether to allow the dialog to be accepted,
        and whether to alert the user to information or require further input from them.

        \li If at least one error is added, the dialog cannot be accepted.
        \li If at least one warning is added and the user does not opt to continue, the dialog will
        not be accepted.
        \li Any information messages added will be shown if there are no errors and all warnings are
        dismissed, with no bearing on whether the dialog may be accepted.

        Subclasses should reimplement this method to append errors, warnings and informative
        text to each of the parameters to indicate dialog status.

        \param errors The list of errors to display.
        \param warnings The list of warnings to display.
        \param information The list of informational messages to display.
     */
    virtual void getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const = 0;
};

#endif // GUARDEDDIALOG_H
