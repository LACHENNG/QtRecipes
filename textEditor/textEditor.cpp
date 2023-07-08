#include "textEditor.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>

TextEditor::TextEditor(QWidget* parent)
  : QTextEdit(parent)
{
    // destory this class object at close
    // otherwise it is hidden by default
    setAttribute(Qt::WA_DeleteOnClose);

    isUntitled = true;

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
}
void TextEditor::newFile()
{
    static int seqenceNumber = 1;

    isUntitled = true;

    currentFileName_ = tr("Untitled %1").arg(seqenceNumber++);

    setWindowTitle(currentFileName_ + "[*]");

}
bool TextEditor::loadFile(const QString& filename)
{
    QFile file(filename);
    // read only
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, tr("多文档编辑器"),
                             tr("无法读取文件 %1:\n%2.").
                             arg(filename).arg(file.errorString()));
        return false;
    }
    currentFileName_ = filename;

    QTextStream in(&file);
    setPlainText(in.readAll());

    return true;
}

bool TextEditor::save()
{
    if(!isUntitled){
        return saveFile(currentFileName_);
    }

    return saveAs();
}

bool TextEditor::saveAs()
{
    auto filename = QFileDialog::getSaveFileName(this, tr("另存为"), currentFileName_);
    if(filename.isEmpty())
        return false;
    return saveFile(filename);
}

bool TextEditor::saveFile(const QString& filename)
{
    QFile file(filename);
    if(! file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, tr("多文档编辑器"),
                             tr("无法写入文件 %1:\n%2.")
                             .arg(filename).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    // write in plain text
    out << toPlainText();
    currentFileName_ = filename;
    return true;
}

void TextEditor::documentWasModified()
{
    // any '[*]'text  set in window title
    // will automaticly display when
    // setWinowModified(true) is called
    setWindowModified(document()->isModified());
}
