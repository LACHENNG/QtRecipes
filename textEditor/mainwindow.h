// Author : Lance @ nwpu

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class TextEditor;
class QMdiSubWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();
    void updateMenus();
    TextEditor *createTextEditor();

private:
    TextEditor* activeTextEditor() const;

private:
    Ui::MainWindow *ui_;
    QAction *actionSeqarator_;
    TextEditor* activeTextEditor_;
};

#endif // MAINWINDOW_H
