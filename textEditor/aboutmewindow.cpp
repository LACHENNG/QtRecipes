#include "aboutmewindow.h"
#include "ui_aboutmewindow.h"

AboutMeWindow::AboutMeWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutMeWindow)
{
    // setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
}

AboutMeWindow::~AboutMeWindow()
{
    delete ui;
}
