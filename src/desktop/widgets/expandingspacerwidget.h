#ifndef EXPANDINGSPACERWIDGET_H
#define EXPANDINGSPACERWIDGET_H

#include <QWidget>

class ExpandingSpacerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExpandingSpacerWidget(Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = NULL);
    QSize sizeHint() const;
};

#endif // EXPANDINGSPACERWIDGET_H
