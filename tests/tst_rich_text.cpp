#include "core/rich_text_document.h"

#include <QDir>
#include <QFile>
#include <QAbstractTextDocumentLayout>
#include <QImage>
#include <QPageLayout>
#include <QPageSize>
#include <QPdfWriter>
#include <QTemporaryDir>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextImageFormat>
#include <QTextList>
#include <QTextObject>
#include <QTextTable>
#include <QUrl>

#include <QtTest>

using mdeditor::createRichTextDocument;
using mdeditor::defaultTextStyle;
using mdeditor::fitImagesToWidth;
using mdeditor::ImageCache;
using mdeditor::TextStyle;

namespace {

// Writes a picture of one color. Returns a null image when the file cannot be written.
QImage writeImage(const QString& path, int width, int height) {
    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::red);
    return image.save(path) ? image : QImage();
}

// The picture that the document shows for the image with the given address.
// It is null when the document has no such picture.
QImage imageOf(const QTextDocument& document, const QString& address) {
    return document.resource(QTextDocument::ImageResource, QUrl(address)).value<QImage>();
}

// The formats of the images of the document in the order of the text.
QList<QTextImageFormat> imageFormats(const QTextDocument& document) {
    QList<QTextImageFormat> formats;
    for (QTextBlock block = document.begin(); block.isValid(); block = block.next()) {
        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            if (it.fragment().isValid() && it.fragment().charFormat().isImageFormat()) {
                formats << it.fragment().charFormat().toImageFormat();
            }
        }
    }
    return formats;
}

QTextBlock blockOfText(const QTextDocument& document, const QString& text) {
    for (QTextBlock block = document.begin(); block.isValid(); block = block.next()) {
        if (block.text() == text) return block;
    }
    return QTextBlock();
}

// The character format of the part of the document that consists of the given text.
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

}  // namespace

// The document that the preview shows and that the PDF export prints: what
// the rich text engine of Qt makes of the Markdown text.
class TestRichText : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() { QVERIFY(directory_.isValid()); }

    void headings();
    void codeBlock();
    void lists();
    void taskList();
    void blockQuote();
    void table();
    void inlineMarkup();

    void loadsImages();
    void loadsImageOfPathWithSpaces();
    void leavesPlaceholderForMissingImage();
    void leavesRemoteImageAlone();
    void fitsWideImages();
    void fitsImagesForPrinter();
    void fittedImageFitsPageOfPrinter();
    void narrowImageKeepsSize();

    void imageCacheRemembersImage();
    void imageCacheShrinksHugeImage();
    void defaultStyleHasFonts();

private:
    QTemporaryDir directory_;
    const TextStyle style_ = defaultTextStyle();
};

void TestRichText::headings() {
    const auto document = createRichTextDocument(
        QStringLiteral("# Первый\n\n## Второй\n\nТекст"), QString(), style_, nullptr);
    const qreal textSize = style_.fontSize;

    const QTextCharFormat first = formatOfText(*document, QStringLiteral("Первый"));
    QCOMPARE(first.fontWeight(), int(QFont::Bold));
    QVERIFY(first.font().pointSizeF() > textSize);
    const QTextCharFormat second = formatOfText(*document, QStringLiteral("Второй"));
    QCOMPARE(second.fontWeight(), int(QFont::Bold));
    QVERIFY(second.font().pointSizeF() > textSize);
    QVERIFY(second.font().pointSizeF() < first.font().pointSizeF());

    const QTextCharFormat text = formatOfText(*document, QStringLiteral("Текст"));
    // The text has no font of its own: it takes the font of the document.
    QVERIFY(!text.hasProperty(QTextFormat::FontFamily));
    QCOMPARE(document->defaultFont().pointSizeF(), textSize);
    QCOMPARE(document->defaultFont().family(), style_.fontFamily);
}

void TestRichText::codeBlock() {
    const auto document = createRichTextDocument(
        QStringLiteral("```\nстрока 1\nстрока 2\n```"), QString(), style_, nullptr);
    // Every line of the code is a line of its own in the font of the code.
    const QTextBlock first = blockOfText(*document, QStringLiteral("строка 1"));
    QVERIFY(first.isValid());
    QCOMPARE(QTextCursor(first).charFormat().fontFamily(), style_.codeFontFamily);
    QVERIFY(blockOfText(*document, QStringLiteral("строка 2")).isValid());
}

void TestRichText::lists() {
    const auto document = createRichTextDocument(
        QStringLiteral("- первый\n- второй\n\n1. третий"), QString(), style_, nullptr);
    const QTextBlock first = blockOfText(*document, QStringLiteral("первый"));
    QVERIFY(first.isValid());
    QTextList* bullets = first.textList();
    QVERIFY(bullets != nullptr);
    QCOMPARE(bullets->count(), 2);
    QVERIFY(bullets->format().style() != QTextListFormat::ListDecimal);

    const QTextBlock third = blockOfText(*document, QStringLiteral("третий"));
    QVERIFY(third.isValid());
    QTextList* numbers = third.textList();
    QVERIFY(numbers != nullptr);
    QCOMPARE(numbers->format().style(), QTextListFormat::ListDecimal);
}

void TestRichText::taskList() {
    const auto document = createRichTextDocument(
        QStringLiteral("- [ ] купить хлеб\n- [x] сделать дело"), QString(), style_, nullptr);
    // The engine cannot draw the box of a task, so the box stands as a character.
    QVERIFY(blockOfText(*document, QStringLiteral("☐ купить хлеб")).isValid());
    QVERIFY(blockOfText(*document, QStringLiteral("☑ сделать дело")).isValid());
}

void TestRichText::blockQuote() {
    const auto document =
        createRichTextDocument(QStringLiteral("> цитата"), QString(), style_, nullptr);
    const QTextBlock block = blockOfText(*document, QStringLiteral("цитата"));
    QVERIFY(block.isValid());
    // A quotation stands apart from the rest of the text.
    QVERIFY(block.blockFormat().leftMargin() > 0);
}

void TestRichText::table() {
    const auto document = createRichTextDocument(
        QStringLiteral("| Имя | Значение |\n| :--- | ---: |\n| a | **1** |"), QString(),
        style_, nullptr);
    const QList<QTextFrame*> frames = document->rootFrame()->childFrames();
    QCOMPARE(frames.size(), 1);
    auto* table = qobject_cast<QTextTable*>(frames.first());
    QVERIFY(table != nullptr);
    QCOMPARE(table->rows(), 2);
    QCOMPARE(table->columns(), 2);
    QCOMPARE(table->cellAt(1, 0).firstCursorPosition().block().text(), QStringLiteral("a"));
    QCOMPARE(table->cellAt(1, 1).firstCursorPosition().block().text(), QStringLiteral("1"));
    // The markup inside a cell is shown too.
    QCOMPARE(table->cellAt(1, 1).firstCursorPosition().charFormat().fontWeight(),
             int(QFont::Bold));

    // The columns have the alignment that the table asks for.
    const auto alignment = [table](int row, int column) {
        // The way of aligning is what matters here, not the flag that tells
        // that the alignment is absolute.
        return table->cellAt(row, column).firstCursorPosition().blockFormat().alignment() &
               (Qt::AlignLeft | Qt::AlignRight | Qt::AlignHCenter | Qt::AlignJustify);
    };
    QCOMPARE(alignment(0, 0), Qt::AlignLeft);
    QCOMPARE(alignment(1, 0), Qt::AlignLeft);
    QCOMPARE(alignment(0, 1), Qt::AlignRight);
    QCOMPARE(alignment(1, 1), Qt::AlignRight);
}

void TestRichText::inlineMarkup() {
    const auto document = createRichTextDocument(
        QStringLiteral("*курсив* **жирный** ~~зачёркнутый~~ [ссылка](/адрес) `код`"), QString(),
        style_, nullptr);
    QVERIFY(formatOfText(*document, QStringLiteral("курсив")).fontItalic());
    QCOMPARE(formatOfText(*document, QStringLiteral("жирный")).fontWeight(), int(QFont::Bold));
    QVERIFY(formatOfText(*document, QStringLiteral("зачёркнутый")).fontStrikeOut());
    // The address is the value of the attribute: the letters outside ASCII are written as codes in it.
    QCOMPARE(formatOfText(*document, QStringLiteral("ссылка")).anchorHref(),
             QStringLiteral("/%D0%B0%D0%B4%D1%80%D0%B5%D1%81"));
    QCOMPARE(formatOfText(*document, QStringLiteral("код")).fontFamily(), style_.codeFontFamily);
}

void TestRichText::loadsImages() {
    QVERIFY(!writeImage(directory_.filePath(QStringLiteral("pic.png")), 37, 23).isNull());
    const auto document = createRichTextDocument(
        QStringLiteral("![x](pic.png)"), directory_.path(), style_, nullptr);
    QCOMPARE(imageOf(*document, QStringLiteral("pic.png")).size(), QSize(37, 23));
}

void TestRichText::loadsImageOfPathWithSpaces() {
    const QString folder = directory_.filePath(QStringLiteral("папка"));
    QVERIFY(QDir().mkpath(folder));
    QVERIFY(!writeImage(folder + QStringLiteral("/a b.png"), 5, 4).isNull());
    // The spaces and the letters outside ASCII are written as codes in the address.
    const QString address = QStringLiteral("%D0%BF%D0%B0%D0%BF%D0%BA%D0%B0/a%20b.png");
    const auto document =
        createRichTextDocument(QStringLiteral("![x](%1)").arg(address), directory_.path(),
                               style_, nullptr);
    QVERIFY(!imageOf(*document, address).isNull());
}

void TestRichText::leavesPlaceholderForMissingImage() {
    const auto document = createRichTextDocument(
        QStringLiteral("![x](нет-файла.png)"), directory_.path(), style_, nullptr);
    // The document keeps the place of the image: the engine shows its own sign there.
    QCOMPARE(imageFormats(*document).size(), 1);
    QVERIFY(imageOf(*document, QStringLiteral("нет-файла.png")).isNull());
}

void TestRichText::leavesRemoteImageAlone() {
    // The program does not go to the network for the images.
    const QString address = QStringLiteral("https://example.com/x.png");
    const auto document =
        createRichTextDocument(QStringLiteral("![x](%1)").arg(address), directory_.path(),
                               style_, nullptr);
    QVERIFY(imageOf(*document, address).isNull());
}

void TestRichText::fitsWideImages() {
    const QString directory = directory_.path();
    QVERIFY(!writeImage(directory + QStringLiteral("/wide.png"), 500, 100).isNull());
    QVERIFY(!writeImage(directory + QStringLiteral("/narrow.png"), 100, 50).isNull());
    const auto document = createRichTextDocument(
        QStringLiteral("![a](wide.png)\n\n![b](narrow.png)"), directory, style_, nullptr);

    fitImagesToWidth(*document, 300);

    const QList<QTextImageFormat> formats = imageFormats(*document);
    QCOMPARE(formats.size(), 2);
    // The wide image is made narrower. Its proportions stay.
    QCOMPARE(formats.at(0).width(), 300.0);
    QCOMPARE(formats.at(0).height(), 60.0);
    // The narrow image is left alone.
    QVERIFY(!formats.at(1).hasProperty(QTextFormat::ImageWidth));
}

void TestRichText::fitsImagesForPrinter() {
    // A page of a PDF has many more dots than the screen has pixels in the
    // same place: a small image can be wider than the page. The device that
    // will paint the document tells how much wider a pixel is there.
    const QString directory = directory_.path();
    QVERIFY(!writeImage(directory + QStringLiteral("/i.png"), 100, 50).isNull());
    const auto document =
        createRichTextDocument(QStringLiteral("![x](i.png)"), directory, style_, nullptr);

    QPdfWriter pdf(directory + QStringLiteral("/page.pdf"));
    pdf.setPageLayout(QPageLayout(QPageSize(QPageSize::A4), QPageLayout::Portrait,
                                  QMarginsF(2, 2, 2, 2), QPageLayout::Millimeter));
    const qreal pageWidth = pdf.pageLayout().paintRectPixels(pdf.resolution()).width();
    fitImagesToWidth(*document, 300, &pdf);

    // Laid out for the same device, the image is 300 dots wide and keeps its
    // proportions.
    document->documentLayout()->setPaintDevice(&pdf);
    document->setPageSize(QSizeF(pageWidth, 1000000));
    const QTextLine line = document->begin().layout()->lineAt(0);
    QVERIFY(qAbs(line.naturalTextWidth() - 300.0) < 0.5);
    const QList<QTextImageFormat> formats = imageFormats(*document);
    QCOMPARE(formats.size(), 1);
    QVERIFY(qAbs(formats.first().height() / formats.first().width() - 0.5) < 0.001);
}

void TestRichText::fittedImageFitsPageOfPrinter() {
    // The image is fitted into the width that the text of the page has.
    const QString directory = directory_.path();
    QVERIFY(!writeImage(directory + QStringLiteral("/wide.png"), 4000, 600).isNull());
    const auto document =
        createRichTextDocument(QStringLiteral("![x](wide.png)"), directory, style_, nullptr);

    QPdfWriter pdf(directory + QStringLiteral("/page.pdf"));
    pdf.setPageLayout(QPageLayout(QPageSize(QPageSize::A4), QPageLayout::Portrait,
                                  QMarginsF(2, 2, 2, 2), QPageLayout::Millimeter));
    const qreal pageWidth = pdf.pageLayout().paintRectPixels(pdf.resolution()).width();
    const qreal textWidth = pageWidth - 2 * ((2.0 / 2.54) * pdf.resolution());
    fitImagesToWidth(*document, textWidth, &pdf);

    // The engine lays the document out for the page the way the printing does.
    document->documentLayout()->setPaintDevice(&pdf);
    document->setPageSize(QSizeF(pageWidth, 1000000));
    const QTextBlock block = blockOfText(*document, QString(QChar::ObjectReplacementCharacter));
    QVERIFY(block.isValid());
    // The image is the whole line: the natural width of the line is its width.
    const qreal imageWidth = block.layout()->lineAt(0).naturalTextWidth();
    // The image goes neither beyond the text of the page nor away from its
    // edges.
    QVERIFY2(imageWidth <= textWidth,
             qPrintable(QStringLiteral("The image is %1 wide, the text of the page is %2.")
                            .arg(imageWidth)
                            .arg(textWidth)));
    QVERIFY(imageWidth > textWidth * 0.95);
}

void TestRichText::narrowImageKeepsSize() {
    const QString directory = directory_.path();
    QVERIFY(!writeImage(directory + QStringLiteral("/i.png"), 100, 50).isNull());
    const auto document =
        createRichTextDocument(QStringLiteral("![x](i.png)"), directory, style_, nullptr);

    // The image fits: it has no size of its own.
    fitImagesToWidth(*document, 300);
    QVERIFY(!imageFormats(*document).first().hasProperty(QTextFormat::ImageWidth));

    // The panel becomes narrow: the image is made smaller.
    fitImagesToWidth(*document, 50);
    QCOMPARE(imageFormats(*document).first().width(), 50.0);
    QCOMPARE(imageFormats(*document).first().height(), 25.0);

    // The panel becomes wide again: the image is itself again.
    fitImagesToWidth(*document, 300);
    QVERIFY(!imageFormats(*document).first().hasProperty(QTextFormat::ImageWidth));
}

void TestRichText::imageCacheRemembersImage() {
    const QString path = directory_.filePath(QStringLiteral("i.png"));
    QVERIFY(!writeImage(path, 8, 6).isNull());

    ImageCache cache;
    QVERIFY(!cache.image(path).isNull());
    // The file is read once: the picture stays in the memory of the program.
    QVERIFY(QFile::remove(path));
    QVERIFY(!cache.image(path).isNull());

    cache.clear();
    QVERIFY(cache.image(path).isNull());
}

void TestRichText::imageCacheShrinksHugeImage() {
    const QString path = directory_.filePath(QStringLiteral("huge.png"));
    QVERIFY(!writeImage(path, 5000, 200).isNull());
    // A picture of thousands of pixels is made smaller: the screen cannot
    // show more, and the whole picture would take a lot of memory.
    const QImage image = ImageCache().image(path);
    QVERIFY(image.width() <= 4096);
    QVERIFY(qAbs(image.height() - image.width() * 200 / 5000) <= 1);
}

void TestRichText::defaultStyleHasFonts() {
    QVERIFY(!style_.fontFamily.isEmpty());
    QVERIFY(!style_.codeFontFamily.isEmpty());
    QVERIFY(style_.fontSize > 0);
}

QTEST_MAIN(TestRichText)
#include "tst_rich_text.moc"
