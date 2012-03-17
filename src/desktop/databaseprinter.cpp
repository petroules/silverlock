#include "databaseprinter.h"

/*!
    Constructs a new DatabasePrinter for the specified \a Database.
 */
DatabasePrinter::DatabasePrinter(const Database *database, QObject *parent)
    : QObject(parent), m_database(database)
{
}

const Database* DatabasePrinter::database() const
{
    return this->m_database;
}

void DatabasePrinter::setDatabase(const Database *database)
{
    this->m_database = database;
}

QTextDocument* DatabasePrinter::toTextDocument(const DatabasePrinterFields &fields) const
{
    if (!this->m_database)
    {
        return NULL;
    }

    QTextDocument *document = new QTextDocument();
    document->setHtml(this->toHtml(fields));
    return document;
}

QString DatabasePrinter::toHtml(const DatabasePrinterFields &fields) const
{
    if (!this->m_database)
    {
        return QString();
    }

    return DatabasePrinter::groupToHtml(this->m_database, fields);
}

/*!
    Gets an HTML-encoded string representing \a group and its child nodes (both groups and entries).

    \param group The group to get an HTML representation of.
 */
QString DatabasePrinter::groupToHtml(const Group *group, const DatabasePrinterFields &fields)
{
    if (!group)
    {
        return QString();
    }

    QString html;
    html += QString("<h1>%1</h1>").arg(group->title());

    QStringList entries;
    foreach (Entry *entry, group->entries())
    {
        entries.append(DatabasePrinter::entryToHtml(entry, fields));
    }

    html += entries.join("<hr />");

    foreach (Group *group, group->groups())
    {
        html += DatabasePrinter::groupToHtml(group, fields);
    }

    return html;
}

/*!
    Gets an HTML-encoded string representing \a entry.

    \param entry The entry to get an HTML representation of.
 */
QString DatabasePrinter::entryToHtml(const Entry *entry, const DatabasePrinterFields &fields)
{
    if (!entry)
    {
        return QString();
    }

    QString html;
    if (fields.title && !entry->title().isEmpty())
    {
        html += QString(tr("<b>Title:</b> %1")).arg(Qt::escape(entry->title())) + "<br />";
    }

    if (fields.username && !entry->username().isEmpty())
    {
        html += QString(tr("<b>Username:</b> %1")).arg(Qt::escape(entry->username())) + "<br />";
    }

    if (fields.password && !entry->password().isEmpty())
    {
        html += QString(tr("<b>Password:</b> %1")).arg(Qt::escape(entry->password())) + "<br />";
    }

    if (fields.url && !entry->url().isEmpty())
    {
        html += QString("<b>URL:</b> %1").arg(Qt::escape(entry->url().toString())) + "<br />";
    }

    if (fields.notes && !entry->notes().isEmpty())
    {
        html += QString(tr("<b>Notes:</b> %1")).arg(Qt::escape(entry->notes())
            .replace(QRegExp("((?:https?|ftp)://\\S+)"), "<a href=\"\\1\">\\1</a>")
            .replace("\n", "<br />")) + "<br />";
    }

    if (fields.recoveryInfo && !entry->recoveryInfo().isEmpty())
    {
        QStringList recovery;
        QMapIterator<QString, QString> i(entry->recoveryInfo());
        while (i.hasNext())
        {
            i.next();
            recovery.append(QString("<em>%1</em> %2").arg(Qt::escape(i.key())).arg(Qt::escape(i.value())));
        }

        html += QString(tr("<b>Recovery info:</b><br />%1")).arg(recovery.join("<br />")) + "<br />";
    }

    if (fields.customFields && !entry->customFields().isEmpty())
    {
        QStringList customFields;
        QMapIterator<QString, QString> i(entry->customFields());
        while (i.hasNext())
        {
            i.next();
            customFields.append(QString("<em>%1</em> %2").arg(Qt::escape(i.key())).arg(Qt::escape(i.value())));
        }

        html += QString(tr("<b>Custom fields:</b><br />%1")).arg(customFields.join("<br />")) + "<br />";
    }

    if (fields.uuid)
    {
        html += QString("<b>UUID:</b> <code>%1</code>").arg(Qt::escape(entry->uuid().toString())) + "<br />";
    }

    if (fields.creationTime)
    {
        html += QString("<b>Created</b>: %1").arg(Qt::escape(entry->created().toLocalTime().toString(Qt::SystemLocaleLongDate))) + "<br />";
    }

    if (fields.accessTime)
    {
        html += QString("<b>Accessed</b>: %1").arg(Qt::escape(entry->accessed().toLocalTime().toString(Qt::SystemLocaleLongDate))) + "<br />";
    }

    if (fields.modificationTime)
    {
        html += QString("<b>Modified</b>: %1").arg(Qt::escape(entry->modified().toLocalTime().toString(Qt::SystemLocaleLongDate))) + "<br />";
    }

    return html;
}
