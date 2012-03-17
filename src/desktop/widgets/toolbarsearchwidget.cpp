#include "toolbarsearchwidget.h"
#include "ui_toolbarsearchwidget.h"

ToolbarSearchWidget::ToolbarSearchWidget(QWidget *parent) :
    IToolbarSearchWidget(parent),
    ui(new Ui::ToolbarSearchWidget)
{
    this->ui->setupUi(this);
}

ToolbarSearchWidget::~ToolbarSearchWidget()
{
    delete this->ui;
}

void ToolbarSearchWidget::requestSearch()
{
    emit this->searchRequested(this->ui->searchLineEdit->text());
}

QString ToolbarSearchWidget::text() const
{
    return this->ui->searchLineEdit->text();
}

void ToolbarSearchWidget::setText(const QString &text)
{
    this->ui->searchLineEdit->setText(text);
}

void ToolbarSearchWidget::clear()
{
    this->ui->searchLineEdit->clear();
}

bool ToolbarSearchWidget::isTextHidden() const
{
    QLineEdit::EchoMode mode = this->ui->searchLineEdit->echoMode();

    // We don't consider PasswordEchoOnEdit "hidden" since it does reveal some characters
    return mode == QLineEdit::NoEcho || mode == QLineEdit::Password;
}

void ToolbarSearchWidget::setTextHidden(bool hide)
{
    this->ui->searchLineEdit->setEchoMode(hide ? QLineEdit::NoEcho : QLineEdit::Normal);
}
