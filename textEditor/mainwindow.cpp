#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "textEditor.h"
#include "QMdiSubWindow"
#include "QtDebug"
#include "QFile"
#include "QFileDialog"
#include "QCloseEvent"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    actionSeqarator_(new QAction(this))
{
    ui_->setupUi(this);

    actionSeqarator_->setSeparator(false);

    updateMenus();

    // update  menu when there is activated textEditors
    connect(ui_->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::on_actionNew_triggered()
{
    TextEditor * child = createTextEditor();
    ui_->mdiArea->addSubWindow(child);
    child->newFile();
    child->show();
}


// Slot triggled by SIGNAL(subWindowActivated(QMdiSubWindow*))
void MainWindow::updateMenus()
{
    qDebug() << "slot updateMenus called with winp = " << activeTextEditor();
    bool hasTextEditors = (activeTextEditor() != nullptr);
    ui_->actionSave->setEnabled(hasTextEditors);
    ui_->actionSaveAs->setEnabled(hasTextEditors);

    ui_->actionPrevious->setEnabled(hasTextEditors);
    ui_->actionNext->setEnabled(hasTextEditors);

    ui_->actionCloseAll->setEnabled(hasTextEditors);
    ui_->actionCascade->setEnabled(hasTextEditors);
    ui_->actionTile->setEnabled(hasTextEditors);
    ui_->actionClose->setEnabled(hasTextEditors);
    ui_->actionPaste->setEnabled(hasTextEditors);

    // cut & copy control
    bool canCutCopy = (activeTextEditor() != nullptr &&
                       activeTextEditor()->textCursor().hasSelection());
    ui_->actionCut->setEnabled(canCutCopy);
    ui_->actionCopy->setEnabled(canCutCopy);

    // undo control
    bool canUndo = (activeTextEditor() != nullptr &&
                    activeTextEditor()->document()->isUndoAvailable());
    ui_->actionUndo->setEnabled(canUndo);

    // redo control
    bool canRedo = (activeTextEditor() != nullptr &&
                    activeTextEditor()->document()->isRedoAvailable());
    ui_->actionRedo->setEnabled(canRedo);
}

TextEditor *MainWindow::createTextEditor()
{
    TextEditor *child = new TextEditor(this);

    // copy & cut control by copyAvailable signals
    connect(child, SIGNAL(copyAvailable(bool)),
            ui_->actionCopy, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            ui_->actionCut, SLOT(setEnabled(bool)));
    // redo & undo by undoAvailable or redoAvailable singal
    connect(child->document(), SIGNAL(redoAvailable(bool)),
            ui_->actionRedo, SLOT(setEnabled(bool)));
    connect(child->document(), SIGNAL(undoAvailable(bool)),
            ui_->actionUndo, SLOT(setEnabled(bool)));

    return child;
}

TextEditor *MainWindow::activeTextEditor() const
{
    QMdiSubWindow* activeSubWindow = ui_->mdiArea->activeSubWindow();
    if(activeSubWindow){
        return qobject_cast<TextEditor*>(activeSubWindow->widget());
    }
    return nullptr;
}

QMdiSubWindow *MainWindow::findActiveEditorByFilePath(const QString &filepath)
{
    auto mdiSubWindowList = ui_->mdiArea->subWindowList();
    for(auto subwindowp : mdiSubWindowList){
        auto *textEditorp = qobject_cast<TextEditor*>(subwindowp->widget());
        if(textEditorp->currentFilePath() == filepath){
            return subwindowp;
        }
    }
    return nullptr;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}


void MainWindow::on_actionOpen_triggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, "打开");

    if(filepath.isEmpty()){
        return ;
    }

    QMdiSubWindow* activeEditor = findActiveEditorByFilePath(filepath);

    if(activeEditor != nullptr){
        ui_->mdiArea->setActiveSubWindow(activeEditor);
        return ;
    }

    TextEditor* child = createTextEditor();
    ui_->mdiArea->addSubWindow(child);
    child->setCurrentFile(filepath);
    // TODO: set cursor to loading
    child->loadFile(filepath);
    child->show();

    return ;
}

void MainWindow::on_actionSave_triggered(){
    TextEditor * activeEditor = activeTextEditor();
    assert(activeEditor != nullptr);

    if(activeEditor->save())
    {
        // disable '[*]' in window title
        activeEditor->setWindowModified(false);
    }
}

void MainWindow::on_actionSaveAs_triggered()
{
    TextEditor * activeEditor = activeTextEditor();
    assert(activeEditor != nullptr);

    activeEditor->saveAs();
    activeEditor->setWindowModified(false);
}

void MainWindow::on_actionExit_triggered()
{
    auto childList = ui_->mdiArea->subWindowList();
    for(auto mdiChild : childList){
        TextEditor* activeEditor =
                    qobject_cast<TextEditor*>(mdiChild->widget());
        activeEditor->save();
        activeEditor->close();
    }
    close();
}

void MainWindow::on_actionCut_triggered()
{
    TextEditor * activeEditor = activeTextEditor();
    assert(activeEditor != nullptr);
    activeEditor->cutToClipBoard();
}

void MainWindow::on_actionCopy_triggered()
{
    TextEditor * activeEditor = activeTextEditor();
    assert(activeEditor != nullptr);
    activeEditor->copySelectedToClipBoard();
}

void MainWindow::on_actionPaste_triggered()
{
    TextEditor * activeEditor = activeTextEditor();
    assert(activeEditor != nullptr);
    activeEditor->pasteFromClipBoard();
}
