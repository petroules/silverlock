#ifndef INACTIVITYEVENTFILTER_H
#define INACTIVITYEVENTFILTER_H

#include <QtCore>

class InactivityEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit InactivityEventFilter(QObject *parent = NULL);

signals:
    void resetIdleTimer(QObject *object);

protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // INACTIVITYEVENTFILTER_H
