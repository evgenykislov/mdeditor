#include "dialog_script.h"
#include "gui/main_window.h"
#include "gui/markdown_editor.h"
#include "gui/markdown_preview.h"

#include <QAction>
#include <QApplication>
#include <QDesktopServices>
#include <QKeyEvent>
#include <QLabel>
#include <QSplitter>
#include <QStatusBar>
#include <QTemporaryDir>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextList>
#include <QTextTable>

#include <QtTest>

#include <memory>

#include "core/document_io.h"

using mdeditor::DocumentIO;
using mdeditor::MarkdownEditor;
using mdeditor::MainWindow;
using mdeditor::MarkdownPreview;

namespace {

// Sends a key to the widget the way the keyboard would. The widget does not
// have to have the focus.
void pressKey(QWidget* widget, int key) {
    QKeyEvent press(QEvent::KeyPress, key, Qt::NoModifier);
    QKeyEvent release(QEvent::KeyRelease, key, Qt::NoModifier);
    QApplication::sendEvent(widget, &press);
    QApplication::sendEvent(widget, &release);
}

// The block of the document that consists of the text. It is invalid when there is none.
QTextBlock blockOfText(const QTextDocument& document, const QString& text) {
    for (QTextBlock block = document.begin(); block.isValid(); block = block.next()) {
        if (block.text() == text) return block;
    }
    return QTextBlock();
}

// The character format of the part of the document that consists of the text.
QTextCharFormat formatOfText(const QTextDocument& document, const QString& text) {
    for (QTextBlock block = document.begin(); block.isValid(); block = block.next()) {
        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            if (it.fragment().isValid() && it.fragment().text() == text) {
                return it.fragment().charFormat();
            }
        }
    }
    return QTextCharFormat();
}

QString readBytes(const QString& path) {
    QFile file(path);
    return file.open(QIODevice::ReadOnly) ? QString::fromUtf8(file.readAll()) : QString();
}

}  // namespace

// The window of the program: what the user sees and does.
class TestMainWindow : public QObject {
    Q_OBJECT

private slots:
    // Every test gets a window of its own and a directory of its own: the
    // files of one test cannot stand in the way of another.
    void init();
    void cleanup();

    void windowHasTwoPanels();
    void newDocumentIsEmpty();
    void previewFollowsText();
    void previewShowsMarkup();
    void previewIsEmptyWhenTextIsDeleted();
    void typingMarksDocumentModified();
    void tabIndentsLines();
    void backtabRemovesIndentation();
    void cursorPositionIsShown();

    void saveAsksForFileAndWritesIt();
    void saveAddsExtension();
    void saveAgainNeedsNoDialog();
    void saveAsWritesAnotherFile();
    void cancelledSaveChangesNothing();
    void failedSaveIsReported();

    void openReadsFile();
    void openOfMissingFileIsReported();

    void newDocumentAsksAboutUnsavedChanges();
    void closeAsksAboutUnsavedChanges();

    void exportHtmlWritesPage();
    void exportPdfWritesFile();
    void cancelledExportWritesNothing();

    void previewOpensLinks();

private:
    QAction* action(const QString& name) const {
        return window_->findChild<QAction*>(name);
    }
    MarkdownEditor* editor() const { return window_->editor(); }
    MarkdownPreview* preview() const { return window_->preview(); }
    // A path inside the directory of the test.
    QString fileName(const QString& name) const { return directory_->filePath(name); }
    // Puts the text into the editor the way it appears when the user types
    // it. Unlike setPlainText, it makes the document a changed one.
    void typeText(const QString& text) { editor()->insertPlainText(text); }
    // Whether the document of the preview has a block with the text.
    bool showsBlock(const QString& text) const {
        return blockOfText(*preview()->document(), text).isValid();
    }
    // What the status bar says about the place of the cursor.
    QString positionText() const {
        for (QLabel* label : window_->statusBar()->findChildren<QLabel*>()) {
            if (label->text().startsWith(QStringLiteral("Строка"))) return label->text();
        }
        return QString();
    }

    std::unique_ptr<QTemporaryDir> directory_;
    MainWindow* window_ = nullptr;
    DialogScript* dialogs_ = nullptr;
};

void TestMainWindow::init() {
    directory_ = std::make_unique<QTemporaryDir>();
    QVERIFY(directory_->isValid());
    window_ = new MainWindow;
    dialogs_ = new DialogScript(window_);
    dialogs_->arm();
    window_->show();
    QVERIFY(QTest::qWaitForWindowExposed(window_));
}

void TestMainWindow::cleanup() {
    const bool unexpected = dialogs_->sawUnexpected();
    delete window_;
    directory_.reset();
    QVERIFY2(!unexpected, "A dialog opened that the test did not expect.");
}

void TestMainWindow::windowHasTwoPanels() {
    // The editor is on the left, and the preview is on the right: in the window and on the screen.
    auto* splitter = window_->findChild<QSplitter*>();
    QVERIFY(splitter != nullptr);
    QCOMPARE(splitter->orientation(), Qt::Horizontal);
    QCOMPARE(splitter->indexOf(editor()), 0);
    QCOMPARE(splitter->indexOf(preview()), 1);
    QVERIFY(!splitter->childrenCollapsible());

    QVERIFY(editor()->isVisible());
    QVERIFY(preview()->isVisible());
    QVERIFY(editor()->geometry().right() < preview()->geometry().left());
    QVERIFY(editor()->isReadOnly() == false);
    QVERIFY(preview()->isReadOnly());
}

void TestMainWindow::newDocumentIsEmpty() {
    QVERIFY(editor()->toPlainText().isEmpty());
    // Nothing is saved of a new document, but it is not changed either.
    QVERIFY(window_->filePath().isEmpty());
    QVERIFY(!window_->isWindowModified());
    // The [*] in the title is the place of the mark of a document that is not saved.
    QVERIFY(window_->windowTitle().remove(QStringLiteral("[*]")) ==
            QStringLiteral("Безымянный документ.md — mdeditor"));

    // The preview says that it is waiting for the text.
    auto* placeholder = window_->findChild<QLabel*>(QStringLiteral("previewPlaceholder"));
    QVERIFY(placeholder != nullptr);
    QVERIFY(placeholder->isVisible());
    QVERIFY(preview()->document()->isEmpty());
}

void TestMainWindow::previewFollowsText() {
    editor()->insertPlainText(QStringLiteral("# Заголовок"));
    // The preview waits for a pause in the typing. The check waits too.
    QTRY_VERIFY(showsBlock(QStringLiteral("Заголовок")));

    // The text goes on: the preview follows it.
    editor()->moveCursor(QTextCursor::End);
    editor()->insertPlainText(QStringLiteral("\n\nПервый абзац."));
    QTRY_VERIFY(showsBlock(QStringLiteral("Первый абзац.")));
    QVERIFY(showsBlock(QStringLiteral("Заголовок")));

    // A change inside a line is shown too.
    editor()->moveCursor(QTextCursor::End);
    editor()->insertPlainText(QStringLiteral(" И ещё."));
    QTRY_VERIFY(showsBlock(QStringLiteral("Первый абзац. И ещё.")));
}

void TestMainWindow::previewShowsMarkup() {
    editor()->setPlainText(QStringLiteral(
        "# Заголовок\n\n**жирный** текст\n\n- пункт\n\n> цитата\n\n| a |\n| - |\n| b |"));
    QTRY_VERIFY(showsBlock(QStringLiteral("Заголовок")));

    // The heading is bold and larger than the text.
    const QTextCharFormat heading =
        formatOfText(*preview()->document(), QStringLiteral("Заголовок"));
    QCOMPARE(heading.fontWeight(), int(QFont::Bold));
    const QTextCharFormat text = formatOfText(*preview()->document(), QStringLiteral("текст"));
    QVERIFY(heading.font().pointSizeF() > text.font().pointSizeF());

    QCOMPARE(formatOfText(*preview()->document(), QStringLiteral("жирный")).fontWeight(),
             int(QFont::Bold));
    QVERIFY(blockOfText(*preview()->document(), QStringLiteral("пункт")).textList() != nullptr);
    QVERIFY(blockOfText(*preview()->document(), QStringLiteral("цитата"))
                .blockFormat()
                .leftMargin() > 0);
    const QList<QTextFrame*> frames = preview()->document()->rootFrame()->childFrames();
    QCOMPARE(frames.size(), 1);
    QVERIFY(qobject_cast<QTextTable*>(frames.first()) != nullptr);
    // The markup characters themselves are not shown.
    QVERIFY(!showsBlock(QStringLiteral("# Заголовок")));
    QVERIFY(!showsBlock(QStringLiteral("**жирный** текст")));
}

void TestMainWindow::previewIsEmptyWhenTextIsDeleted() {
    editor()->setPlainText(QStringLiteral("текст"));
    QTRY_VERIFY(showsBlock(QStringLiteral("текст")));
    auto* placeholder = window_->findChild<QLabel*>(QStringLiteral("previewPlaceholder"));
    QVERIFY(!placeholder->isVisible());

    editor()->clear();
    QTRY_VERIFY(placeholder->isVisible());
    QVERIFY(preview()->document()->isEmpty());
}

void TestMainWindow::typingMarksDocumentModified() {
    QVERIFY(!window_->isWindowModified());
    editor()->insertPlainText(QStringLiteral("текст"));
    // The title shows the mark of a document that has changes which are not saved.
    QVERIFY(window_->isWindowModified());
    QVERIFY(window_->windowTitle().contains(QLatin1Char('*')));
}

void TestMainWindow::tabIndentsLines() {
    editor()->setPlainText(QStringLiteral("первый\nвторой"));
    // Tab moves the text to the right instead of leaving the editor.
    pressKey(editor(), Qt::Key_Tab);
    QCOMPARE(editor()->toPlainText(), QStringLiteral("    первый\nвторой"));

    // All the lines that the selection touches are indented.
    editor()->selectAll();
    pressKey(editor(), Qt::Key_Tab);
    QCOMPARE(editor()->toPlainText(), QStringLiteral("        первый\n    второй"));
}

void TestMainWindow::backtabRemovesIndentation() {
    editor()->setPlainText(QStringLiteral("  две строки"));
    editor()->moveCursor(QTextCursor::Start);
    pressKey(editor(), Qt::Key_Backtab);
    // Only the indentation goes. The text stays as it is.
    QCOMPARE(editor()->toPlainText(), QStringLiteral("две строки"));
}

void TestMainWindow::cursorPositionIsShown() {
    editor()->setPlainText(QStringLiteral("абв\nгде"));
    QCOMPARE(positionText(), QStringLiteral("Строка 1, столбец 1"));
    editor()->moveCursor(QTextCursor::End);
    QCOMPARE(positionText(), QStringLiteral("Строка 2, столбец 4"));
}

void TestMainWindow::saveAsksForFileAndWritesIt() {
    const QString text = QStringLiteral("# Заголовок\n\nТекст документа.");
    editor()->setPlainText(text);
    const QString path = fileName(QStringLiteral("заметка.md"));

    dialogs_->expectFile(path);
    action(QStringLiteral("actionSave"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());

    QCOMPARE(readBytes(path), text);
    QCOMPARE(window_->filePath(), path);
    // The document is saved: it has no unsaved changes any more, and its name is in the title.
    QVERIFY(!window_->isWindowModified());
    QVERIFY(window_->windowTitle().contains(QStringLiteral("заметка.md")));
}

void TestMainWindow::saveAddsExtension() {
    editor()->setPlainText(QStringLiteral("текст"));
    // The name is written without an extension.
    dialogs_->expectFile(fileName(QStringLiteral("имя")));
    action(QStringLiteral("actionSave"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());

    QVERIFY(QFileInfo::exists(fileName(QStringLiteral("имя.md"))));
    QVERIFY(window_->filePath().endsWith(QStringLiteral(".md")));
}

void TestMainWindow::saveAgainNeedsNoDialog() {
    const QString path = fileName(QStringLiteral("заметка.md"));
    editor()->setPlainText(QStringLiteral("первая"));
    dialogs_->expectFile(path);
    action(QStringLiteral("actionSave"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());

    // The document has a file already: the next saving writes to it without asking.
    editor()->setPlainText(QStringLiteral("вторая"));
    action(QStringLiteral("actionSave"))->trigger();
    QTest::qWait(100);
    QCOMPARE(readBytes(path), QStringLiteral("вторая"));
    QVERIFY(!window_->isWindowModified());
}

void TestMainWindow::saveAsWritesAnotherFile() {
    const QString first = fileName(QStringLiteral("первый.md"));
    const QString second = fileName(QStringLiteral("второй.md"));
    editor()->setPlainText(QStringLiteral("первая"));
    dialogs_->expectFile(first);
    action(QStringLiteral("actionSave"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());

    editor()->setPlainText(QStringLiteral("вторая"));
    dialogs_->expectFile(second);
    action(QStringLiteral("actionSaveAs"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());

    // The document now goes to the new file. The old one is as it was.
    QCOMPARE(readBytes(second), QStringLiteral("вторая"));
    QCOMPARE(readBytes(first), QStringLiteral("первая"));
    QCOMPARE(window_->filePath(), second);
    QVERIFY(window_->windowTitle().contains(QStringLiteral("второй.md")));
}

void TestMainWindow::cancelledSaveChangesNothing() {
    typeText(QStringLiteral("текст"));
    dialogs_->expectCancel();
    action(QStringLiteral("actionSave"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());

    QVERIFY(window_->filePath().isEmpty());
    QVERIFY(window_->isWindowModified());
    QVERIFY(QDir(directory_->path()).entryList(QDir::Files | QDir::NoDotAndDotDot).isEmpty());
}

void TestMainWindow::failedSaveIsReported() {
    typeText(QStringLiteral("текст"));
    // The file cannot be written where there is no directory.
    dialogs_->expectFile(fileName(QStringLiteral("нет-каталога/заметка.md")));
    dialogs_->expectMessage(QMessageBox::Ok);
    action(QStringLiteral("actionSave"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());

    // The document is not saved, and its text is still there.
    QVERIFY(window_->filePath().isEmpty());
    QVERIFY(window_->isWindowModified());
    QCOMPARE(editor()->toPlainText(), QStringLiteral("текст"));
}

void TestMainWindow::openReadsFile() {
    const QString path = fileName(QStringLiteral("из файла.md"));
    DocumentIO::saveMarkdown(path, QStringLiteral("# Из файла\n\nтекст файла"));

    dialogs_->expectFile(path);
    action(QStringLiteral("actionOpen"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());

    // The text is in the editor, and the preview shows what it means.
    QCOMPARE(editor()->toPlainText(), QStringLiteral("# Из файла\n\nтекст файла"));
    QTRY_VERIFY(showsBlock(QStringLiteral("Из файла")));
    QCOMPARE(formatOfText(*preview()->document(), QStringLiteral("Из файла")).fontWeight(),
             int(QFont::Bold));
    QVERIFY(!window_->isWindowModified());
    QVERIFY(window_->windowTitle().contains(QStringLiteral("из файла.md")));
}

void TestMainWindow::openOfMissingFileIsReported() {
    editor()->setPlainText(QStringLiteral("прежний текст"));
    dialogs_->expectMessage(QMessageBox::Ok);
    // This is the way the file of the command line is opened.
    window_->openFile(fileName(QStringLiteral("нет-файла.md")));
    QTRY_VERIFY(dialogs_->isDone());

    // What was in the editor stays there.
    QCOMPARE(editor()->toPlainText(), QStringLiteral("прежний текст"));
}

void TestMainWindow::newDocumentAsksAboutUnsavedChanges() {
    typeText(QStringLiteral("изменённый текст"));

    // The user changes their mind: the document stays.
    dialogs_->expectMessage(QMessageBox::Cancel);
    action(QStringLiteral("actionNew"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());
    QCOMPARE(editor()->toPlainText(), QStringLiteral("изменённый текст"));

    // The user drops the changes: a new empty document comes.
    dialogs_->expectMessage(QMessageBox::Discard);
    action(QStringLiteral("actionNew"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());
    QVERIFY(editor()->toPlainText().isEmpty());
    QVERIFY(!window_->isWindowModified());
    // The [*] in the title is the place of the mark of a document that is not saved.
    QVERIFY(window_->windowTitle().remove(QStringLiteral("[*]")) ==
            QStringLiteral("Безымянный документ.md — mdeditor"));
}

void TestMainWindow::closeAsksAboutUnsavedChanges() {
    const QString text = QStringLiteral("несохранённый текст");
    const QString path = fileName(QStringLiteral("при закрытии.md"));
    typeText(text);

    // The user cancels: the window stays open.
    dialogs_->expectMessage(QMessageBox::Cancel);
    QVERIFY(!window_->close());
    QVERIFY(window_->isVisible());

    // The user saves: the file is written, and the window closes.
    dialogs_->expectMessage(QMessageBox::Save);
    dialogs_->expectFile(path);
    QVERIFY(window_->close());
    QVERIFY(!window_->isVisible());
    QCOMPARE(readBytes(path), text);
}

void TestMainWindow::exportHtmlWritesPage() {
    typeText(QStringLiteral("# Титул\n\nТекст **жирный**."));
    const QString path = fileName(QStringLiteral("страница.html"));
    dialogs_->expectFile(path);
    action(QStringLiteral("actionExportHtml"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());

    const QString html = readBytes(path);
    QVERIFY(html.startsWith(QStringLiteral("<!DOCTYPE html>")));
    QVERIFY(html.contains(QStringLiteral("<title>Титул</title>")));
    QVERIFY(html.contains(QStringLiteral("<h1>Титул</h1>")));
    QVERIFY(html.contains(QStringLiteral("<strong>жирный</strong>")));
    // An export is not a saving of the document.
    QVERIFY(window_->isWindowModified());
}

void TestMainWindow::exportPdfWritesFile() {
    editor()->setPlainText(QStringLiteral("# Титул\n\nТекст."));
    const QString path = fileName(QStringLiteral("документ.pdf"));
    dialogs_->expectFile(path);
    action(QStringLiteral("actionExportPdf"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QVERIFY(file.read(5).startsWith("%PDF-"));
}

void TestMainWindow::cancelledExportWritesNothing() {
    typeText(QStringLiteral("текст"));
    dialogs_->expectCancel();
    action(QStringLiteral("actionExportPdf"))->trigger();
    QTRY_VERIFY(dialogs_->isDone());

    QVERIFY(QDir(directory_->path()).entryList(QDir::Files | QDir::NoDotAndDotDot).isEmpty());
}

void TestMainWindow::previewOpensLinks() {
    // A document with a link to a file next to it and a link to the web.
    const QString document = fileName(QStringLiteral("документ.md"));
    DocumentIO::saveMarkdown(
        document, QStringLiteral("[файл](соседний.txt)\n\n[сайт](https://example.com/страница)\n\n"
                                 "[пропажа](нет-файла.txt)"));
    DocumentIO::saveMarkdown(fileName(QStringLiteral("соседний.txt")),
                             QStringLiteral("сосед"));

    UrlReceiver receiver;
    QDesktopServices::setUrlHandler(QStringLiteral("file"), &receiver, "openUrl");
    QDesktopServices::setUrlHandler(QStringLiteral("https"), &receiver, "openUrl");
    dialogs_->expectMessage(QMessageBox::Ok);
    window_->openFile(document);

    // The panel asks the system to open the address the way it does for a click on the link.
    const auto click = [this](const QString& link) {
        QMetaObject::invokeMethod(preview(), "anchorClicked", Q_ARG(QUrl, QUrl(link)));
    };
    click(QStringLiteral("соседний.txt"));
    QTRY_COMPARE(receiver.opened.size(), 1);
    QCOMPARE(receiver.opened.last(),
             QUrl::fromLocalFile(fileName(QStringLiteral("соседний.txt"))));

    click(QStringLiteral("https://example.com/страница"));
    QTRY_COMPARE(receiver.opened.size(), 2);
    QCOMPARE(receiver.opened.last(), QUrl(QStringLiteral("https://example.com/страница")));

    // A link to a file that is not there is told about instead of being opened.
    click(QStringLiteral("нет-файла.txt"));
    QTRY_VERIFY(dialogs_->isDone());
    QTest::qWait(50);
    QCOMPARE(receiver.opened.size(), 2);

    QDesktopServices::unsetUrlHandler(QStringLiteral("file"));
    QDesktopServices::unsetUrlHandler(QStringLiteral("https"));
}

int main(int argc, char* argv[]) {
    // The dialogs of Qt are used: the dialogs of the window system cannot be answered from a test.
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    QApplication application(argc, argv);
    TestMainWindow test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_main_window.moc"
