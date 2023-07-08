// Author : Lance @ nwpu
// MDI: multiple Document Interface example : text editor

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
    void on_actionOpen_triggered();
    void updateMenus();
    TextEditor *createTextEditor();


private:
    TextEditor* activeTextEditor() const;
       QMdiSubWindow *findActiveEditorByFilePath(const QString& filepath);
private:
    Ui::MainWindow *ui_;
    QAction *actionSeqarator_;
//    TextEditor* activeTextEditor_;
};

#endif // MAINWINDOW_H
