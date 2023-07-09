#ifndef ABOUTMEWINDOW_H
#define ABOUTMEWINDOW_H

#include <QWidget>

namespace Ui {
class AboutMeWindow;
}

class AboutMeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AboutMeWindow(QWidget *parent = nullptr);
    ~AboutMeWindow();

private:
    Ui::AboutMeWindow *ui;
};

#endif // ABOUTMEWINDOW_H
