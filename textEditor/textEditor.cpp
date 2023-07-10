#include "textEditor.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextEncoder>
#include <QCloseEvent>
#include <QDebug>
#include <QMenu>

TextEditor::TextEditor(QWidget* parent)
  : QTextEdit(parent),
    isUntitled(true),
    isFileCommited_(false)
{
    // destory this class object at close
    // otherwise it is hidden by default
    setAttribute(Qt::WA_DeleteOnClose);

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(setfileUnCommited()));
}
void TextEditor::newFile()
{
    isUntitled = true;
    setfileCommited();

    static int seqenceNumber = 1;
    setCurrentFile(tr("./Untitled %1.txt").arg(seqenceNumber++));
    setWindowTitle(currentFileName() + "[*]");
}

bool TextEditor::loadFile(const QString& filepath)
{
    isUntitled = false;

    QFile file(filepath);

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, tr("多文档编辑器"),
                             tr("无法读取文件 %1:\n%2.").
                             arg(filepath).arg(file.errorString()));
        return false;
    }
    setCurrentFile(filepath);
    setWindowTitle(currentFileName() + "[*]");

    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName("utf-8"));
    setPlainText(in.readAll());
    // disable '*' in window title at first load file content
    setWindowModified(false);
    setfileCommited();
    return true;
}

bool TextEditor::save()
{
    // nothing to save as file is allready commit to disk
    if(fileCommited()){
        return true;
    }
    if(!isUntitled){
        return saveFile(currentFilePath());
    }
    return saveAs();
}

bool TextEditor::saveAs()
{
    auto filepath = QFileDialog::getSaveFileName(this, tr("另存为"), currentFilePath_);
    if(filepath.isEmpty())
        return false;

    return saveFile(filepath);
}

void TextEditor::cutToClipBoard()
{
    cut();
}

void TextEditor::copySelectedToClipBoard()
{
    copy();
}

void TextEditor::pasteFromClipBoard()
{
    paste();
}

void TextEditor::clearContent()
{
    clear();
    setfileUnCommited();
}

bool TextEditor::fileCommited()
{
    return isFileCommited_;
}

bool TextEditor::saveFile(const QString& filepath)
{
    isUntitled = false;

    QFile file(filepath);
    if(! file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, tr("多文档编辑器"),
                             tr("无法写入文件 %1:\n%2.")
                             .arg(filepath).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("utf-8"));
    // write in plain text
    out << toPlainText();

    setCurrentFile(filepath);
    setWindowTitle(currentFileName() + "[*]");
    setWindowModified(false);
    setfileCommited();
    return true;
}

void TextEditor::closeEvent(QCloseEvent *event)
{
    if(!document()->isModified() || fileCommited()){
        event->accept();
        return ;
    }
    QMessageBox msgBox;
    msgBox.setText("你想将更改保存到");
    msgBox.setInformativeText(currentFilePath());
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    msgBox.setButtonText(QMessageBox::Save, tr("保存(&S)"));
    msgBox.setButtonText(QMessageBox::Discard, tr("丢弃(&N)"));
    msgBox.setButtonText(QMessageBox::Cancel, tr("取消"));

    auto choice = msgBox.exec();
    switch (choice){
        case QMessageBox::Save:
            if(save()){
                event->accept();
            }
            break;
        case QMessageBox::Discard:
            event->accept();
            break;
        case QMessageBox::Cancel:
            event->ignore();
            break;
        default:
            event->ignore();
            break;
    }
}

void TextEditor::contextMenuEvent(QContextMenuEvent *e)
{
    QScopedPointer<QMenu> menu(new QMenu());

    auto aundo = menu->addAction(tr("撤销(&R)"), this, SLOT(undo()), QKeySequence::Undo);
    auto aredo = menu->addAction(tr("恢复(&R)"), this, SLOT(redo()), QKeySequence::Redo);
    aundo->setEnabled(document()->isUndoAvailable());
    aredo->setEnabled(document()->isRedoAvailable());
    menu->addSeparator();

    auto acut = menu->addAction(tr("剪切(&T)"), this, SLOT(cutToClipBoard()), QKeySequence::Cut);
    auto acopy = menu->addAction(tr("复制(&T)"), this, SLOT(copySelectedToClipBoard()), QKeySequence::Cut);
    auto apaste = menu->addAction(tr("粘贴(&T)"), this, SLOT(pasteFromClipBoard()), QKeySequence::Cut);
    auto aclear = menu->addAction(tr("清空(&P)"), this, SLOT(clearContent()));
    acut->setEnabled(textCursor().hasSelection());
    acopy->setEnabled(textCursor().hasSelection());
    apaste->setEnabled(true);
    aclear->setEnabled(true);

    menu->exec(e->globalPos());
}

void TextEditor::setCurrentFile(const QString &absFilePath)
{
    if(isUntitled){
        currentFilePath_ = absFilePath;
        return ;
    }
    currentFilePath_ = QFileInfo(absFilePath).canonicalFilePath();
}

QString TextEditor::currentFileName()
{
    return QFileInfo(currentFilePath()).fileName();
}

void TextEditor::documentWasModified()
{
    // any '[*]'text  set in window title
    // will automaticly display when
    // setWinowModified(true) is called
    setWindowModified(document()->isModified());
}

void TextEditor::setfileCommited()
{
    isFileCommited_ = true;
}

void TextEditor::setfileUnCommited()
{
    isFileCommited_ = false;
}
