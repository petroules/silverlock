#include "expandingspacerwidget.h"

ExpandingSpacerWidget::ExpandingSpacerWidget(Qt::Orientation orientation, QWidget *parent) :
    QWidget(parent)
{
    if (orientation == Qt::Horizontal)
    {
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    else if (orientation == Qt::Vertical)
    {
        this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
}

QSize ExpandingSpacerWidget::sizeHint() const
{
    return QSize(1, 1);
}
