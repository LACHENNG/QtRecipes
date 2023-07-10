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
class QSignalMapper;

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
    void on_actionSave_triggered();
    TextEditor *createTextEditor();

    void on_actionSaveAs_triggered();

    void on_actionExit_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionClose_triggered();

    void on_actionCloseAll_triggered();

    void on_actionTile_triggered();

    void on_actionCascade_triggered();

    void on_actionNext_triggered();

    void on_actionPrevious_triggered();

    void on_actionAbout_triggered();

    void on_actionAboutQt_triggered();
    // update subwindow list of file names for opened
    // text editors in window menu
    void updateSubWindowListInMenu();
    void setActiveMdiSubWindow(QWidget*);
    void showCurLinePosInStatusBar();
private:
    TextEditor* activeTextEditor() const;
    QMdiSubWindow *findActiveEditorByFilePath(const QString& filepath);
    void closeEvent(QCloseEvent *event) override;

    void writeAppSettings();
    void readAppSettings();

private:
    Ui::MainWindow *ui_;
    QAction *actionSeqarator_;
    QSignalMapper *windowMapper_;
//    TextEditor* activeTextEditor_;
};

#endif // MAINWINDOW_H
