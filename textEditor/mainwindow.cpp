#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "textEditor.h"
#include "QMdiSubWindow"
#include "QtDebug"
#include "QFile"
#include "QFileDialog"
#include "QCloseEvent"
#include "aboutmewindow.h"
#include "QSignalMapper"
#include "QSettings"
#include "QGuiApplication"
#include "QScreen"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow),
    actionSeqarator_(new QAction(this)),
    windowMapper_(new QSignalMapper(this))
{
    ui_->setupUi(this);
//    ui_->mdiArea->setViewMode(QMdiArea::TabbedView);
    actionSeqarator_->setSeparator(true);

    updateMenus();

    // update  menu when there is activated textEditors
    connect(ui_->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));

    // signal mapper
    connect(windowMapper_, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveMdiSubWindow(QWidget*)));
    connect(ui_->menuW, SIGNAL(aboutToShow()),
            this, SLOT(updateSubWindowListInMenu()));

    readAppSettings();
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
    connect(child, SIGNAL(cursorPositionChanged()),
            this, SLOT(showCurLinePosInStatusBar()));

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
    ui_->mdiArea->closeAllSubWindows();
    bool midchildAllClosed = (ui_->mdiArea->subWindowList().size() == 0);
    if(midchildAllClosed){
        writeAppSettings();
        event->accept();
    }
    else event->ignore();
}

void MainWindow::writeAppSettings()
{
    QSettings settings("lanceFreeSoftware", "textEditor");
    settings.setValue("window_pos", pos());
    settings.setValue("window_size", size());
}

void MainWindow::readAppSettings()
{
    // default position and size
    // centralize
    QRect rect = QGuiApplication::primaryScreen()->geometry();
    int desktop_width = rect.width();
    int desktop_height = rect.height();
    QSize defaultWindSize(desktop_width/2, desktop_height/2);
    QPoint center(desktop_height, desktop_width);
    QPoint offset(defaultWindSize.height() / 2, defaultWindSize.width() / 2);
    QPoint defaultPos(center - offset);

    QSettings settings("lanceFreeSoftware", "textEditor");
    auto pos = settings.value("window_pos", defaultWindSize).toPoint();
    auto size = settings.value("window_size", defaultPos).toSize();
    move(pos);
    resize(size);
}

void MainWindow::setActiveMdiSubWindow(QWidget *targetWind)
{
    auto mdiChild = qobject_cast<QMdiSubWindow*>(targetWind);
    ui_->mdiArea->setActiveSubWindow(mdiChild);
}

void MainWindow::showCurLinePosInStatusBar()
{
    if(activeTextEditor() == nullptr){
        return ;
    }
    int rowNum = activeTextEditor()->textCursor().blockNumber() + 1;
    int colNum = activeTextEditor()->textCursor().columnNumber() + 1;
    ui_->statusbar->showMessage(tr("%1 行 %2 列")
                                .arg(rowNum).arg(colNum), 2000);
}

void MainWindow::updateSubWindowListInMenu()
{
    auto menuW = ui_->menuW;
    menuW->clear();
    menuW->addAction(ui_->actionClose);
    menuW->addAction(ui_->actionCloseAll);
    menuW->addSeparator();
    menuW->addAction(ui_->actionTile);
    menuW->addAction(ui_->actionCascade);
    menuW->addSeparator();
    menuW->addAction(ui_->actionPrevious);
    menuW->addAction(ui_->actionNext);
    menuW->addAction(actionSeqarator_);


    const auto& midSubWindList = ui_->mdiArea->subWindowList();
    bool hasOpenedEditors = (midSubWindList.size() != 0);
    actionSeqarator_->setVisible(hasOpenedEditors);

    int i = 1;
    for(auto midSubWind : midSubWindList){
        TextEditor* editorp = qobject_cast<TextEditor*>(midSubWind->widget());

        auto name = editorp->currentFileName();
        QAction* action = new QAction(menuW);
        if(i <= 9){
            action->setText(tr("&%1 %2").arg(i).arg(name));
        }else{
            action->setText(tr("%1 %2").arg(i).arg(name));
        }

        action->setCheckable(true);
        action->setChecked(activeTextEditor() == editorp);
        menuW->addAction(action);

        // windowMapper reemit singals of sender to receiver
        // with extra parameters
        connect(action, SIGNAL(triggered()),
                windowMapper_, SLOT(map()));
        windowMapper_->setMapping(action, midSubWind);
        i++;
    }
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

    if(activeEditor->saveAs())
        activeEditor->setWindowModified(false);
}

void MainWindow::on_actionExit_triggered()
{
//    on_actionCloseAll_triggered();
//    close();
    QApplication::closeAllWindows();
    writeAppSettings();
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

void MainWindow::on_actionClose_triggered()
{
    auto *midActiveChild = ui_->mdiArea->activeSubWindow();
    midActiveChild->close();
}

void MainWindow::on_actionCloseAll_triggered()
{
    ui_->mdiArea->closeAllSubWindows();
}

void MainWindow::on_actionTile_triggered()
{
    ui_->mdiArea->tileSubWindows();
}

void MainWindow::on_actionCascade_triggered()
{
    ui_->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actionNext_triggered()
{
    ui_->mdiArea->activateNextSubWindow();
}

void MainWindow::on_actionPrevious_triggered()
{
    ui_->mdiArea->activatePreviousSubWindow();
}

void MainWindow::on_actionAbout_triggered()
{
    auto *aboutWind = new AboutMeWindow();
    aboutWind->setWindowModality(Qt::WindowModality::ApplicationModal);
    aboutWind->show();
}

void MainWindow::on_actionAboutQt_triggered()
{
    qApp->aboutQt();
}
