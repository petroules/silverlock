#ifndef INACTIVITYEVENTFILTER_H
#define INACTIVITYEVENTFILTER_H

#include <QObject>

class InactivityEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit InactivityEventFilter(QObject *parent = NULL);

signals:
    /*!
        Emitted when a mouse or key event occurs.

        \param object The object that generated the event.
     */
    void resetIdleTimer(QObject *object);

protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // INACTIVITYEVENTFILTER_H
