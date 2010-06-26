#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtCore>
#include <QtGui>

class AboutDialog
{
public:
    static void show(QWidget* parent, const QString& title, const QString& description);
private:
    static QString osString();
    static QString byteOrder();
    static int wordSize();
};

#endif // ABOUTDIALOG_H
