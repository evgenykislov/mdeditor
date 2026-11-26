#pragma once

#include <QMainWindow>
#include <QString>

#include <exception>

class QLabel;
class QCloseEvent;
class QSplitter;
class QTimer;

namespace mdeditor {

class MarkdownEditor;
class MarkdownPreview;

// The window of the program: the editor of the Markdown text on the left and
// the document that is made of it on the right.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    // Reads the Markdown file into the editor. The reason is shown in a
    // message when the file cannot be read.
    void openFile(const QString& path);

    MarkdownEditor* editor() const { return editor_; }
    MarkdownPreview* preview() const { return preview_; }
    // The file of the document. It is empty until the document is saved.
    QString filePath() const { return filePath_; }

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    // What came of saving the document.
    enum class SaveResult { Saved, NotSaved, Cancelled };

    // The files that the open dialog shows.
    static QString markdownFilters();

    void createActions();
    void loadText(const QString& text, const QString& path);

    // Shows the editor text in the preview.
    void updatePreview();
    void updateWindowTitle();
    void updateCursorPosition();
    void showFileError(const std::exception& error);

    // Asks whether the changes of the document should be saved. Returns
    // false when the user decided not to go on with what was about to happen.
    bool maybeSave();
    SaveResult saveDocument();
    // Returns false when the user cancelled the dialog or the file could not be written.
    bool saveDocumentAs();
    bool saveTo(const QString& path);
    void loadFile(const QString& path);
    void exportHtml();
    void exportPdf();

    // The directory that the relative paths of the images are relative to.
    QString sourceDirectory() const;
    // The directory that the file dialogs show: the one that was used last.
    QString dialogDirectory() const;
    // Asks for a file to write to. The extension is added to a name that has
    // none. Returns an empty string when the user cancels the dialog.
    QString askWritePath(const QString& title, const QString& filters,
                         const QString& extension, const QString& suggestedName);

    MarkdownEditor* editor_;
    MarkdownPreview* preview_;
    QSplitter* splitter_;
    QLabel* positionLabel_;
    // Counts the time from the last change of the text to the next preview.
    QTimer* previewTimer_;
    QString filePath_;
    QString lastDirectory_;
};

}  // namespace mdeditor
