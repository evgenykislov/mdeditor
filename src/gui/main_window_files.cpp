#include "main_window.h"

#include "markdown_editor.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextDocument>
#include <QTimer>

#include "core/document_io.h"

namespace mdeditor {
namespace {

// How long the messages about files stay in the status bar.
constexpr int kMessageMs = 6000;

QString untitledName() { return QStringLiteral("Безымянный документ.md"); }

// The name of the document without the directory and without the extension.
QString documentName(const QString& filePath) {
    return filePath.isEmpty() ? QStringLiteral("Безымянный документ")
                              : QFileInfo(filePath).completeBaseName();
}

}  // namespace

QString MainWindow::markdownFilters() {
    return QStringLiteral("Документы Markdown (*.md *.markdown);;Текстовые файлы (*.txt);;"
                          "Все файлы (*)");
}

void MainWindow::openFile(const QString& path) { loadFile(path); }

void MainWindow::loadFile(const QString& path) {
    try {
        loadText(DocumentIO::loadMarkdown(path), path);
    } catch (const std::exception& error) {
        showFileError(error);
    }
}

void MainWindow::loadText(const QString& text, const QString& path) {
    filePath_ = path;
    if (!path.isEmpty()) lastDirectory_ = QFileInfo(path).absolutePath();
    editor_->setPlainText(text);
    // The new text is not a change of the document. The window is told so
    // explicitly: the engine clears its own mark of a changed document
    // without telling about it.
    editor_->document()->setModified(false);
    setWindowModified(false);
    // The preview shows the new text right away instead of waiting for a pause.
    previewTimer_->stop();
    updatePreview();
    updateWindowTitle();
    if (!path.isEmpty()) {
        statusBar()->showMessage(
            QStringLiteral("Открыт: %1").arg(QDir::toNativeSeparators(path)), kMessageMs);
    }
}

bool MainWindow::maybeSave() {
    if (!editor_->document()->isModified()) return true;
    const auto answer = QMessageBox::warning(
        this, QStringLiteral("Несохранённые изменения"),
        QStringLiteral("Документ изменён. Сохранить изменения?"), QMessageBox::Save |
                                                                      QMessageBox::Discard |
                                                                      QMessageBox::Cancel,
        QMessageBox::Save);
    if (answer == QMessageBox::Cancel) return false;
    // The user goes on only when the document is saved or its changes are dropped.
    return answer == QMessageBox::Discard || saveDocument() == SaveResult::Saved;
}

MainWindow::SaveResult MainWindow::saveDocument() {
    if (filePath_.isEmpty()) return saveDocumentAs() ? SaveResult::Saved : SaveResult::Cancelled;
    return saveTo(filePath_) ? SaveResult::Saved : SaveResult::NotSaved;
}

bool MainWindow::saveDocumentAs() {
    const QString suggested =
        filePath_.isEmpty() ? QDir(dialogDirectory()).filePath(untitledName()) : filePath_;
    const QString path = askWritePath(QStringLiteral("Сохранить документ"), markdownFilters(),
                                      QStringLiteral(".md"), suggested);
    return !path.isEmpty() && saveTo(path);
}

bool MainWindow::saveTo(const QString& path) {
    try {
        DocumentIO::saveMarkdown(path, editor_->toPlainText());
    } catch (const std::exception& error) {
        showFileError(error);
        return false;
    }
    filePath_ = path;
    lastDirectory_ = QFileInfo(path).absolutePath();
    editor_->document()->setModified(false);
    setWindowModified(false);
    updateWindowTitle();
    statusBar()->showMessage(
        QStringLiteral("Сохранено: %1").arg(QDir::toNativeSeparators(path)), kMessageMs);
    return true;
}

void MainWindow::exportHtml() {
    const QString suggested =
        QDir(dialogDirectory()).filePath(documentName(filePath_) + QStringLiteral(".html"));
    const QString path = askWritePath(QStringLiteral("Экспорт в HTML"),
                                      QStringLiteral("Документ HTML (*.html *.htm)"),
                                      QStringLiteral(".html"), suggested);
    if (path.isEmpty()) return;
    try {
        DocumentIO::exportHtml(path, editor_->toPlainText(), sourceDirectory());
    } catch (const std::exception& error) {
        showFileError(error);
        return;
    }
    statusBar()->showMessage(
        QStringLiteral("Экспортировано в HTML: %1").arg(QDir::toNativeSeparators(path)), kMessageMs);
}

void MainWindow::exportPdf() {
    const QString suggested =
        QDir(dialogDirectory()).filePath(documentName(filePath_) + QStringLiteral(".pdf"));
    const QString path =
        askWritePath(QStringLiteral("Экспорт в PDF"), QStringLiteral("Документ PDF (*.pdf)"),
                     QStringLiteral(".pdf"), suggested);
    if (path.isEmpty()) return;
    try {
        DocumentIO::exportPdf(path, editor_->toPlainText(), sourceDirectory());
    } catch (const std::exception& error) {
        showFileError(error);
        return;
    }
    statusBar()->showMessage(
        QStringLiteral("Экспортировано в PDF: %1").arg(QDir::toNativeSeparators(path)), kMessageMs);
}

QString MainWindow::sourceDirectory() const {
    return filePath_.isEmpty() ? QString() : QFileInfo(filePath_).absolutePath();
}

QString MainWindow::dialogDirectory() const {
    return lastDirectory_.isEmpty() ? QDir::homePath() : lastDirectory_;
}

QString MainWindow::askWritePath(const QString& title, const QString& filters,
                                 const QString& extension, const QString& suggestedName) {
    QString path = QFileDialog::getSaveFileName(this, title, suggestedName, filters);
    // A name without an extension gets the extension of the format.
    if (!path.isEmpty() && QFileInfo(path).suffix().isEmpty()) path += extension;
    return path;
}

}  // namespace mdeditor
