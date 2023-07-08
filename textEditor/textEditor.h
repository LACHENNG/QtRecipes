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
    bool loadFile(const QString& filename);
    bool save();
    bool saveAs();
    bool saveFile(const QString& filename);

protected:
    void onClose(QCloseEvent* event);

private slots:
    void documentWasModified();

private:
    QString currentFileName_;
    bool isUntitled;
};

#endif // TextEditor_H
