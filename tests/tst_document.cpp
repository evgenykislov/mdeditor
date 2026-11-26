#include "core/document_io.h"
#include "core/markdown_renderer.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QTemporaryDir>

#include <QtTest>

#include <stdexcept>

using mdeditor::DocumentIO;

namespace {

QByteArray readBytes(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return QByteArray();
    return file.readAll();
}

bool writeBytes(const QString& path, const QByteArray& bytes) {
    QFile file(path);
    return file.open(QIODevice::WriteOnly) && file.write(bytes) == bytes.size();
}

// Writes a picture of one color. Returns a null image when the file cannot be written.
QImage writeImage(const QString& path, int width, int height) {
    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::red);
    return image.save(path) ? image : QImage();
}

int pageCount(const QByteArray& pdf) {
    // Every page has an object of its own. The tree of the pages is not a page.
    return pdf.count("/Type /Page") - pdf.count("/Type /Pages");
}

}  // namespace

// The files of the editor and the two exports.
class TestDocument : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() { QVERIFY(directory_.isValid()); }

    // The files of the Markdown text.
    void saveAndLoadKeepTheText();
    void loadMakesLineBreaksLf();
    void loadRemovesByteOrderMark();
    void loadReadsFileInEncodingOfSystem();
    void loadOfMissingFileThrows();
    void loadOfDirectoryThrows();
    void saveIntoMissingDirectoryThrows();
    void saveReplacesOldContent();

    // The HTML export.
    void htmlExportIsStandalonePage();
    void htmlExportTakesTitleFromHeading();
    void htmlExportTakesTitleFromFileName();
    void htmlExportEscapesTitle();
    void htmlExportKeepsImagesReachable();
    void htmlExportKeepsImagesNextToPage();
    void htmlExportLeavesRemoteAddressesAlone();

    // The PDF export.
    void pdfExportMakesPdfFile();
    void pdfExportOfLongDocumentHasPages();
    void pdfExportHasTitle();
    void pdfExportIncludesImage();
    void pdfExportFitsWideImageIntoPage();
    void pdfExportIntoMissingDirectoryThrows();
    void pdfExportIntoDirectoryThrows();

private:
    QTemporaryDir directory_;
};

void TestDocument::saveAndLoadKeepTheText() {
    const QString path = directory_.filePath(QStringLiteral("документ.md"));
    const QString text =
        QStringLiteral("# Привет\n\nСтрока с табуляцией\tи эмодзи \U0001F389\n日本語\n");
    DocumentIO::saveMarkdown(path, text);

    const QByteArray bytes = readBytes(path);
    // UTF-8 without the byte order mark, with line breaks of one kind.
    QVERIFY(!bytes.startsWith(QByteArrayLiteral("\xEF\xBB\xBF")));
    QVERIFY(!bytes.contains('\r'));
    QCOMPARE(QString::fromUtf8(bytes), text);
    QCOMPARE(DocumentIO::loadMarkdown(path), text);
}

void TestDocument::loadMakesLineBreaksLf() {
    const QString path = directory_.filePath(QStringLiteral("lines.md"));
    QVERIFY(writeBytes(path, "a\r\nb\rc\n\r\n"));
    QCOMPARE(DocumentIO::loadMarkdown(path), QStringLiteral("a\nb\nc\n\n"));
}

void TestDocument::loadRemovesByteOrderMark() {
    const QString path = directory_.filePath(QStringLiteral("bom.md"));
    QVERIFY(writeBytes(path, QByteArrayLiteral("\xEF\xBB\xBF") + "# Заголовок"));
    QCOMPARE(DocumentIO::loadMarkdown(path), QStringLiteral("# Заголовок"));
}

void TestDocument::loadReadsFileInEncodingOfSystem() {
    const QString path = directory_.filePath(QStringLiteral("system.txt"));
    const QString text = QStringLiteral("Привет, мир!");
    // The check tells anything only where the encoding of the system is not UTF-8.
    if (QString::fromUtf8(text.toLocal8Bit()) == text) {
        QSKIP("The encoding of the system is UTF-8.");
    }
    QVERIFY(writeBytes(path, text.toLocal8Bit()));
    QCOMPARE(DocumentIO::loadMarkdown(path), text);
}

void TestDocument::loadOfMissingFileThrows() {
    const QString path = directory_.filePath(QStringLiteral("нет-такого-файла.md"));
    bool isThrown = false;
    try {
        DocumentIO::loadMarkdown(path);
    } catch (const std::runtime_error& error) {
        isThrown = true;
        // The message tells what happened and names the file.
        const QString message = QString::fromUtf8(error.what());
        QVERIFY(!message.isEmpty());
        QVERIFY(message.contains(QFileInfo(path).fileName()));
    }
    QVERIFY(isThrown);
}

void TestDocument::loadOfDirectoryThrows() {
    QVERIFY_EXCEPTION_THROWN(DocumentIO::loadMarkdown(directory_.path()), std::runtime_error);
}

void TestDocument::saveIntoMissingDirectoryThrows() {
    const QString path = directory_.filePath(QStringLiteral("нет-каталога/документ.md"));
    QVERIFY_EXCEPTION_THROWN(DocumentIO::saveMarkdown(path, QStringLiteral("текст")),
                             std::runtime_error);
    QVERIFY(!QFileInfo::exists(path));
}

void TestDocument::saveReplacesOldContent() {
    const QString path = directory_.filePath(QStringLiteral("документ.md"));
    DocumentIO::saveMarkdown(path, QStringLiteral("первая"));
    DocumentIO::saveMarkdown(path, QStringLiteral("вторая"));
    QCOMPARE(DocumentIO::loadMarkdown(path), QStringLiteral("вторая"));
}

void TestDocument::htmlExportIsStandalonePage() {
    const QString path = directory_.filePath(QStringLiteral("страница.html"));
    DocumentIO::exportHtml(
        path, QStringLiteral("# Заголовок\n\nТекст **жирный** и [ссылка](https://example.com)."));
    const QString html = QString::fromUtf8(readBytes(path));
    QVERIFY(html.startsWith(QStringLiteral("<!DOCTYPE html>")));
    QVERIFY(html.contains(QStringLiteral("<meta charset=\"utf-8\">")));
    QVERIFY(html.contains(QStringLiteral("<title>Заголовок</title>")));
    // The style sheet is inside the page: the page needs no other file.
    QVERIFY(html.contains(QStringLiteral("<style>")));
    QVERIFY(html.contains(QStringLiteral("<h1>Заголовок</h1>")));
    QVERIFY(html.contains(QStringLiteral("<strong>жирный</strong>")));
    QVERIFY(html.contains(QStringLiteral("<a href=\"https://example.com\">ссылка</a>")));
    QVERIFY(html.endsWith(QStringLiteral("</html>\n")));
}

void TestDocument::htmlExportTakesTitleFromHeading() {
    // Only a heading of the first level is the title of the document.
    const QString html =
        DocumentIO::renderHtml(QStringLiteral("## Не заголовок\n\n# Главный"));
    QVERIFY(html.contains(QStringLiteral("<title>Главный</title>")));
}

void TestDocument::htmlExportTakesTitleFromFileName() {
    const QString html = DocumentIO::renderHtml(
        QStringLiteral("Текст без заголовка"), QString(), QString(), QStringLiteral("Мои заметки"));
    QVERIFY(html.contains(QStringLiteral("<title>Мои заметки</title>")));
    // The name of the file is the title only when the document has none of its own.
    const QString named = DocumentIO::renderHtml(QStringLiteral("# Есть заголовок"), QString(),
                                                  QString(), QStringLiteral("Мои заметки"));
    QVERIFY(named.contains(QStringLiteral("<title>Есть заголовок</title>")));
}

void TestDocument::htmlExportEscapesTitle() {
    const QString html = DocumentIO::renderHtml(QStringLiteral("# a < b & \"c\""));
    QVERIFY(html.contains(QStringLiteral("<title>a &lt; b &amp; &quot;c&quot;</title>")));
}

void TestDocument::htmlExportKeepsImagesReachable() {
    // The page is saved in another directory than the Markdown text: the
    // paths of the images become relative to the page.
    const QString source = directory_.filePath(QStringLiteral("source"));
    const QString output = directory_.filePath(QStringLiteral("output"));
    QVERIFY(QDir().mkpath(source + QStringLiteral("/img")));
    QVERIFY(QDir().mkpath(output));
    QVERIFY(!writeImage(source + QStringLiteral("/img/a b.png"), 10, 10).isNull());

    const QString page = output + QStringLiteral("/заметка.html");
    DocumentIO::exportHtml(page, QStringLiteral("![x](img/a%20b.png)"), source);
    const QString html = QString::fromUtf8(readBytes(page));
    QVERIFY(html.contains(QStringLiteral("src=\"../source/img/a%20b.png\"")));
}

void TestDocument::htmlExportKeepsImagesNextToPage() {
    // The page is saved where the Markdown text is: the paths are right already.
    const QString html = DocumentIO::renderHtml(QStringLiteral("![x](img/a.png)"),
                                                directory_.path(), directory_.path());
    QVERIFY(html.contains(QStringLiteral("src=\"img/a.png\"")));
}

void TestDocument::htmlExportLeavesRemoteAddressesAlone() {
    const QString html = DocumentIO::renderHtml(QStringLiteral("![x](https://example.com/x.png)"),
                                                directory_.path(),
                                                directory_.filePath(QStringLiteral("другой")));
    QVERIFY(html.contains(QStringLiteral("src=\"https://example.com/x.png\"")));
}

void TestDocument::pdfExportMakesPdfFile() {
    const QString path = directory_.filePath(QStringLiteral("документ.pdf"));
    DocumentIO::exportPdf(path, QStringLiteral("# Заголовок\n\nАбзац текста."));
    const QByteArray pdf = readBytes(path);
    QVERIFY(pdf.startsWith("%PDF-"));
    QVERIFY(pdf.endsWith("%%EOF\n"));
    QCOMPARE(pageCount(pdf), 1);
}

void TestDocument::pdfExportOfLongDocumentHasPages() {
    QString markdown;
    for (int index = 0; index < 120; ++index) {
        markdown += QStringLiteral("Абзац %1 с достаточно длинным текстом.\n\n").arg(index + 1);
    }
    const QString path = directory_.filePath(QStringLiteral("длинный.pdf"));
    DocumentIO::exportPdf(path, markdown);
    QVERIFY(pageCount(readBytes(path)) > 1);
}

void TestDocument::pdfExportHasTitle() {
    const QString path = directory_.filePath(QStringLiteral("title.pdf"));
    DocumentIO::exportPdf(path, QStringLiteral("# Отчёт за год\n\nТекст."));
    const QByteArray pdf = readBytes(path);
    // The title of the PDF is the heading of the document. The PDF format
    // writes it as UTF-16 for the letters to survive.
    QByteArray encoded;
    for (const QChar c : QStringLiteral("Отчёт за год")) {
        encoded += static_cast<char>(c.unicode() >> 8);
        encoded += static_cast<char>(c.unicode() & 0xFF);
    }
    QVERIFY(pdf.contains("/Title"));
    QVERIFY(pdf.contains(encoded));
}

void TestDocument::pdfExportIncludesImage() {
    const QString directory = directory_.path();
    QVERIFY(!writeImage(directory + QStringLiteral("/i.png"), 37, 23).isNull());
    const QString path = directory_.filePath(QStringLiteral("с картинкой.pdf"));
    DocumentIO::exportPdf(path, QStringLiteral("![x](i.png)"), directory);
    const QByteArray pdf = readBytes(path);
    QVERIFY(pdf.count("/Subtype /Image") > 0);
    QVERIFY(pdf.contains("/Width 37"));
    QVERIFY(pdf.contains("/Height 23"));
}

void TestDocument::pdfExportFitsWideImageIntoPage() {
    // The picture is much wider than a page. It is made narrower, and the
    // document stays on one page instead of being cut into pieces.
    const QString directory = directory_.path();
    QVERIFY(!writeImage(directory + QStringLiteral("/wide.png"), 4000, 300).isNull());
    const QString path = directory_.filePath(QStringLiteral("широкая.pdf"));
    DocumentIO::exportPdf(path, QStringLiteral("До.\n\n![x](wide.png)\n\nПосле."), directory);
    QCOMPARE(pageCount(readBytes(path)), 1);
}

void TestDocument::pdfExportIntoMissingDirectoryThrows() {
    const QString path = directory_.filePath(QStringLiteral("нет-каталога/документ.pdf"));
    QVERIFY_EXCEPTION_THROWN(DocumentIO::exportPdf(path, QStringLiteral("текст")),
                             std::runtime_error);
    QVERIFY(!QFileInfo::exists(directory_.filePath(QStringLiteral("нет-каталога"))));
}

void TestDocument::pdfExportIntoDirectoryThrows() {
    // Nothing can be written where a directory is. The directory stays as it
    // is, and no temporary file is left next to it.
    const QString target = directory_.filePath(QStringLiteral("каталог.pdf"));
    QVERIFY(QDir().mkpath(target));
    QVERIFY_EXCEPTION_THROWN(DocumentIO::exportPdf(target, QStringLiteral("текст")),
                             std::runtime_error);
    QVERIFY(QFileInfo(target).isDir());
    const QDir parent(directory_.path());
    const QStringList temporary =
        parent.entryList({QStringLiteral(".mdeditor-*")}, QDir::Files | QDir::Hidden);
    QCOMPARE(temporary, QStringList());
}

QTEST_MAIN(TestDocument)
#include "tst_document.moc"
