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

    setCurrentFile(tr("./Untitled %1.txt").arg(seqenceNumber++));

    setWindowTitle(currentFileName() + "[*]");

}
bool TextEditor::loadFile(const QString& filepath)
{
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
    setPlainText(in.readAll());
    // disable '*' in window title at first load file content
    setWindowModified(false);

    return true;
}

bool TextEditor::save()
{
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

bool TextEditor::saveFile(const QString& filepath)
{
    QFile file(filepath);
    if(! file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, tr("多文档编辑器"),
                             tr("无法写入文件 %1:\n%2.")
                             .arg(filepath).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    // write in plain text
    out << toPlainText();
    currentFilePath_ = filepath;
    return true;
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
