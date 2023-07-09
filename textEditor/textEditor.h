// Author : Lance @ nwpu

#ifndef TextEditor_H
#define TextEditor_H

#include <QWidget>
#include <QTextEdit>

class TextEditor : public QTextEdit
{
    // This line of Macro is needed in order to use signal and slots in Qt
    Q_OBJECT
public:
    explicit TextEditor(QWidget* parent = nullptr);
    void newFile();
    bool loadFile(const QString& filepath);
    bool save();
    bool saveAs();

    void cutToClipBoard();
    void copySelectedToClipBoard();
    void pasteFromClipBoard();

    bool fileCommited();
    void setCurrentFile(const QString& absFilePath);
    QString currentFilePath() { return currentFilePath_;}
    QString currentFileName();

private:
    bool saveFile(const QString& filepath);
    void closeEvent(QCloseEvent *event) override;

private slots:
    void documentWasModified();
    void setfileCommited();
    void setfileUnCommited();

private:
    QString currentFilePath_;
    bool isUntitled;
    // file is commited to disk(saved)
    bool isFileCommited_;
};

#endif // TextEditor_H
