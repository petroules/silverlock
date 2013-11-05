#ifndef TOOLBARSEARCHWIDGET_H
#define TOOLBARSEARCHWIDGET_H

#include <QWidget>

namespace Ui
{
    class ToolbarSearchWidget;
}

class ToolbarSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolbarSearchWidget(QWidget *parent = NULL);
    ~ToolbarSearchWidget();
    QString text() const;
    void setText(const QString &text);
    void clear();
    bool isTextHidden() const;
    void setTextHidden(bool hide);

signals:
    void searchRequested(QString);

private slots:
    void requestSearch();

private:
    Ui::ToolbarSearchWidget *ui;
};

#endif // TOOLBARSEARCHWIDGET_H
