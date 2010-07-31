#include "databaseprintdialog.h"
#include "ui_databaseprintdialog.h"
#include <silverlocklib.h>

DatabasePrintDialog::DatabasePrintDialog(Database *database, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabasePrintDialog)
{
    this->ui->setupUi(this);
    this->m_database = database;
    this->updatePreview();
}

DatabasePrintDialog::~DatabasePrintDialog()
{
    delete this->ui;
}

void DatabasePrintDialog::print()
{
    QPrinter printer;
    QTextDocument document;
    document.setHtml(this->groupText(this->m_database));
    document.print(&printer);

    QPrintDialog dialog(&printer, this);
    this->hide();
    dialog.exec();
}

void DatabasePrintDialog::selectionLinkActivated(const QString &link)
{
    if (link == "#select-all")
    {
        this->selectAll(true);
    }
    else if (link == "#clear-all")
    {
        this->selectAll(false);
    }
}

void DatabasePrintDialog::selectAll(bool select)
{
    this->ui->titleCheckBox->setChecked(select);
    this->ui->urlCheckBox->setChecked(select);
    this->ui->usernameCheckBox->setChecked(select);
    this->ui->passwordCheckBox->setChecked(select);
    this->ui->notesCheckBox->setChecked(select);
    this->ui->recoveryInfoCheckBox->setChecked(select);
    this->ui->customFieldsCheckBox->setChecked(select);
    this->ui->uuidCheckBox->setChecked(select);
    this->ui->creationTimeCheckBox->setChecked(select);
    this->ui->accessTimeCheckBox->setChecked(select);
    this->ui->modificationTimeCheckBox->setChecked(select);
}

void DatabasePrintDialog::updatePreview()
{
    QString preview;
    if (this->ui->showActualCheckBox->isChecked())
    {
        preview += this->groupText(this->m_database);
    }
    else
    {
        Group *group = new Group(tr("My Group"));

        Entry *entry = new Entry("Google", group);
        entry->setUrl(QUrl("http://www.google.com/"));
        entry->setUsername("john.doe");
        entry->setPassword("!$uper$ecretPa$$w0rd~");
        entry->setNotes(tr("This is my Google account!\n\nI created it on 2008-09-19 & have been using it since."));
        entry->insertRecoveryInfo(tr("What is your mother's maiden name?"), "Doe");
        entry->insertRecoveryInfo(tr("What is your favourite colour?"), "Blue");
        entry->insertCustomField("PIN", "0123");

        preview += this->groupText(group);
    }

    this->ui->textBrowser->setHtml(preview);
}

QString DatabasePrintDialog::groupText(const Group *group) const
{
    if (!group)
    {
        return QString();
    }

    QString preview;
    preview += QString("<h1>%1</h1>").arg(group->title());

    QStringList entries;
    foreach (Entry *entry, group->entries())
    {
        entries.append(this->entryText(entry));
    }

    preview += entries.join("<hr />");

    foreach (Group *group, group->groups())
    {
        preview += this->groupText(group);
    }

    return preview;
}

QString DatabasePrintDialog::entryText(const Entry *entry) const
{
    if (!entry)
    {
        return QString();
    }

    QString preview;
    if (this->ui->titleCheckBox->isChecked() && !entry->title().isEmpty())
    {
        preview += QString(tr("<b>Title:</b> %1")).arg(Qt::escape(entry->title())) + "<br />";
    }

    if (this->ui->urlCheckBox->isChecked() && !entry->url().isEmpty())
    {
        preview += QString("<b>URL:</b> %1").arg(Qt::escape(entry->url().toString())) + "<br />";
    }

    if (this->ui->usernameCheckBox->isChecked() && !entry->username().isEmpty())
    {
        preview += QString(tr("<b>Username:</b> %1")).arg(Qt::escape(entry->username())) + "<br />";
    }

    if (this->ui->passwordCheckBox->isChecked() && !entry->password().isEmpty())
    {
        preview += QString(tr("<b>Password:</b> %1")).arg(Qt::escape(entry->password())) + "<br />";
    }

    if (this->ui->notesCheckBox->isChecked() && !entry->notes().isEmpty())
    {
        preview += QString(tr("<b>Notes:</b> %1")).arg(Qt::escape(entry->notes())
            .replace(QRegExp("((?:https?|ftp)://\\S+)"), "<a href=\"\\1\">\\1</a>")
            .replace("\n", "<br />")) + "<br />";
    }

    if (this->ui->recoveryInfoCheckBox->isChecked() && !entry->recoveryInfo().isEmpty())
    {
        QStringList recovery;
        QMapIterator<QString, QString> i(entry->recoveryInfo());
        while (i.hasNext())
        {
            i.next();
            recovery.append(QString("<em>%1</em> %2").arg(Qt::escape(i.key())).arg(Qt::escape(i.value())));
        }

        preview += QString(tr("<b>Recovery info:</b><br />%1")).arg(recovery.join("<br />")) + "<br />";
    }

    if (this->ui->customFieldsCheckBox->isChecked() && !entry->customFields().isEmpty())
    {
        QStringList customFields;
        QMapIterator<QString, QString> i(entry->customFields());
        while (i.hasNext())
        {
            i.next();
            customFields.append(QString("<em>%1</em> %2").arg(Qt::escape(i.key())).arg(Qt::escape(i.value())));
        }

        preview += QString(tr("<b>Custom fields:</b><br />%1")).arg(customFields.join("<br />")) + "<br />";
    }

    if (this->ui->uuidCheckBox->isChecked())
    {
        preview += QString("<b>UUID:</b> <code>%1</code>").arg(Qt::escape(entry->uuid().toString())) + "<br />";
    }

    if (this->ui->creationTimeCheckBox->isChecked())
    {
        preview += QString("<b>Created</b>: %1").arg(Qt::escape(entry->created().toLocalTime().toString(Qt::SystemLocaleLongDate))) + "<br />";
    }

    if (this->ui->accessTimeCheckBox->isChecked())
    {
        preview += QString("<b>Accessed</b>: %1").arg(Qt::escape(entry->accessed().toLocalTime().toString(Qt::SystemLocaleLongDate))) + "<br />";
    }

    if (this->ui->modificationTimeCheckBox->isChecked())
    {
        preview += QString("<b>Modified</b>: %1").arg(Qt::escape(entry->modifiedTime().toLocalTime().toString(Qt::SystemLocaleLongDate))) + "<br />";
    }

    return preview;
}
