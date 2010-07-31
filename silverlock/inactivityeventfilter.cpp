#include "inactivityeventfilter.h"

InactivityEventFilter::InactivityEventFilter(QObject *parent) :
    QObject(parent)
{
}

bool InactivityEventFilter::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress ||
        event->type() == QEvent::KeyRelease ||
        event->type() == QEvent::MouseButtonDblClick ||
        event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseMove)
    {
        emit this->resetIdleTimer(object);
    }

    return QObject::eventFilter(object, event);
}
