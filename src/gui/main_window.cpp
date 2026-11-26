#include "main_window.h"

#include "markdown_editor.h"
#include "markdown_preview.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QTextCursor>
#include <QTextDocument>
#include <QTimer>

namespace mdeditor {
namespace {

// The preview is not made for every keystroke: the typing stays smooth when
// the document is large.
constexpr int kPreviewDelayMs = 150;

QString untitledName() { return QStringLiteral("Безымянный документ.md"); }

}  // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setObjectName(QStringLiteral("mainWindow"));
    resize(1120, 720);

    editor_ = new MarkdownEditor(this);
    preview_ = new MarkdownPreview(this);
    splitter_ = new QSplitter(Qt::Horizontal, this);
    splitter_->addWidget(editor_);
    splitter_->addWidget(preview_);
    // The panels share the window equally, and neither of them can be folded away.
    splitter_->setStretchFactor(0, 1);
    splitter_->setStretchFactor(1, 1);
    splitter_->setChildrenCollapsible(false);
    setCentralWidget(splitter_);

    positionLabel_ = new QLabel(this);
    statusBar()->addPermanentWidget(positionLabel_);

    previewTimer_ = new QTimer(this);
    previewTimer_->setSingleShot(true);
    previewTimer_->setInterval(kPreviewDelayMs);

    createActions();

    // The preview follows the text: a little later, when the typing pauses.
    connect(editor_, &QPlainTextEdit::textChanged, this, [this] { previewTimer_->start(); });
    connect(previewTimer_, &QTimer::timeout, this, &MainWindow::updatePreview);
    connect(editor_->document(), &QTextDocument::modificationChanged, this,
            &QMainWindow::setWindowModified);
    connect(editor_, &QPlainTextEdit::cursorPositionChanged, this,
            &MainWindow::updateCursorPosition);

    editor_->document()->setModified(false);
    updatePreview();
    updateWindowTitle();
    updateCursorPosition();

    // The program that is run by the tests has no organization name: its
    // windows leave no traces in the settings of the user.
    if (QCoreApplication::organizationName().isEmpty()) return;
    const QSettings settings;
    restoreGeometry(settings.value(QStringLiteral("window/geometry")).toByteArray());
    splitter_->restoreState(settings.value(QStringLiteral("window/splitter")).toByteArray());
    lastDirectory_ = settings.value(QStringLiteral("files/directory")).toString();
}

MainWindow::~MainWindow() = default;

void MainWindow::createActions() {
    QMenu* file = menuBar()->addMenu(QStringLiteral("&Файл"));

    QAction* newDocument = file->addAction(QStringLiteral("&Создать"));
    newDocument->setObjectName(QStringLiteral("actionNew"));
    newDocument->setShortcut(QKeySequence::New);
    connect(newDocument, &QAction::triggered, this, [this] {
        if (maybeSave()) loadText(QString(), QString());
    });

    QAction* open = file->addAction(QStringLiteral("&Открыть…"));
    open->setObjectName(QStringLiteral("actionOpen"));
    open->setShortcut(QKeySequence::Open);
    connect(open, &QAction::triggered, this, [this] {
        if (!maybeSave()) return;
        const QString path = QFileDialog::getOpenFileName(
            this, QStringLiteral("Открыть документ"), dialogDirectory(), markdownFilters());
        if (!path.isEmpty()) loadFile(path);
    });

    QAction* save = file->addAction(QStringLiteral("&Сохранить"));
    save->setObjectName(QStringLiteral("actionSave"));
    save->setShortcut(QKeySequence::Save);
    connect(save, &QAction::triggered, this, [this] { saveDocument(); });

    QAction* saveAs = file->addAction(QStringLiteral("Сохранить &как…"));
    saveAs->setObjectName(QStringLiteral("actionSaveAs"));
    saveAs->setShortcut(QKeySequence::SaveAs);
    connect(saveAs, &QAction::triggered, this, [this] { saveDocumentAs(); });

    file->addSeparator();

    QAction* exportHtmlAction = file->addAction(QStringLiteral("Экспорт в &HTML…"));
    exportHtmlAction->setObjectName(QStringLiteral("actionExportHtml"));
    connect(exportHtmlAction, &QAction::triggered, this, &MainWindow::exportHtml);

    QAction* exportPdfAction = file->addAction(QStringLiteral("Экспорт в &PDF…"));
    exportPdfAction->setObjectName(QStringLiteral("actionExportPdf"));
    connect(exportPdfAction, &QAction::triggered, this, &MainWindow::exportPdf);

    file->addSeparator();

    QAction* quit = file->addAction(QStringLiteral("В&ыход"));
    quit->setMenuRole(QAction::QuitRole);
    quit->setShortcut(QKeySequence::Quit);
    connect(quit, &QAction::triggered, this, &QWidget::close);

    QMenu* edit = menuBar()->addMenu(QStringLiteral("&Правка"));

    QAction* undo = edit->addAction(QStringLiteral("&Отменить"));
    undo->setShortcut(QKeySequence::Undo);
    connect(undo, &QAction::triggered, editor_, &QPlainTextEdit::undo);
    connect(editor_, &QPlainTextEdit::undoAvailable, undo, &QAction::setEnabled);
    undo->setEnabled(false);

    QAction* redo = edit->addAction(QStringLiteral("&Повторить"));
    redo->setShortcut(QKeySequence::Redo);
    connect(redo, &QAction::triggered, editor_, &QPlainTextEdit::redo);
    connect(editor_, &QPlainTextEdit::redoAvailable, redo, &QAction::setEnabled);
    redo->setEnabled(false);

    edit->addSeparator();

    const auto addEditAction = [this, edit](const QString& text, QKeySequence::StandardKey key,
                                            void (QPlainTextEdit::*method)()) {
        QAction* action = edit->addAction(text);
        action->setShortcut(key);
        connect(action, &QAction::triggered, editor_, method);
        return action;
    };
    QAction* cut = addEditAction(QStringLiteral("Вы&резать"), QKeySequence::Cut, &QPlainTextEdit::cut);
    QAction* copy = addEditAction(QStringLiteral("&Копировать"), QKeySequence::Copy, &QPlainTextEdit::copy);
    addEditAction(QStringLiteral("Вст&авить"), QKeySequence::Paste, &QPlainTextEdit::paste);
    addEditAction(QStringLiteral("Выделить &всё"), QKeySequence::SelectAll, &QPlainTextEdit::selectAll);
    const auto enableCutAndCopy = [cut, copy](bool available) {
        cut->setEnabled(available);
        copy->setEnabled(available);
    };
    connect(editor_, &QPlainTextEdit::copyAvailable, this, enableCutAndCopy);
    enableCutAndCopy(false);

    QMenu* view = menuBar()->addMenu(QStringLiteral("&Вид"));

    QAction* zoomIn = view->addAction(QStringLiteral("У&величить масштаб предпросмотра"));
    zoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(zoomIn, &QAction::triggered, this, [this] { preview_->changeZoom(1); });

    QAction* zoomOut = view->addAction(QStringLiteral("У&меньшить масштаб предпросмотра"));
    zoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOut, &QAction::triggered, this, [this] { preview_->changeZoom(-1); });

    QAction* zoomReset = view->addAction(QStringLiteral("&Обычный масштаб предпросмотра"));
    zoomReset->setShortcut(Qt::CTRL | Qt::Key_0);
    connect(zoomReset, &QAction::triggered, this, [this] { preview_->changeZoom(-preview_->zoom()); });

    QMenu* help = menuBar()->addMenu(QStringLiteral("&Справка"));
    QAction* about = help->addAction(QStringLiteral("О &программе mdeditor…"));
    about->setMenuRole(QAction::AboutRole);
    connect(about, &QAction::triggered, this, [this] {
        QMessageBox::about(
            this, QStringLiteral("О программе"),
            QStringLiteral("<b>mdeditor</b> ") + QStringLiteral(MDEDITOR_VERSION) +
                QStringLiteral("<p>Редактор текста в разметке Markdown. Слева редактируется "
                               "текст, справа видно то, что из него получается.") + "<p>" +
                QStringLiteral("Поддерживаются CommonMark и расширения GitHub Flavored "
                               "Markdown: таблицы, списки задач, зачёркивание и автоматические "
                               "ссылки."));
    });
}

void MainWindow::updatePreview() {
    preview_->showMarkdown(editor_->toPlainText(), sourceDirectory());
}

void MainWindow::updateWindowTitle() {
    // The [*] becomes the mark of a document that has unsaved changes.
    const QString name = filePath_.isEmpty() ? QFileInfo(untitledName()).fileName()
                                             : QFileInfo(filePath_).fileName();
    setWindowTitle(QStringLiteral("%1[*] — mdeditor").arg(name));
}

void MainWindow::updateCursorPosition() {
    const QTextCursor cursor = editor_->textCursor();
    positionLabel_->setText(QStringLiteral("Строка %1, столбец %2")
                                .arg(cursor.blockNumber() + 1)
                                .arg(cursor.columnNumber() + 1));
}

void MainWindow::showFileError(const std::exception& error) {
    QMessageBox::critical(this, QStringLiteral("Ошибка файла"), QString::fromUtf8(error.what()));
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (!maybeSave()) {
        event->ignore();
        return;
    }
    if (!QCoreApplication::organizationName().isEmpty()) {
        QSettings settings;
        settings.setValue(QStringLiteral("window/geometry"), saveGeometry());
        settings.setValue(QStringLiteral("window/splitter"), splitter_->saveState());
        settings.setValue(QStringLiteral("files/directory"), lastDirectory_);
    }
    event->accept();
}

}  // namespace mdeditor
