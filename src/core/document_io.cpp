#include "core/document_io.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPageLayout>
#include <QPageSize>
#include <QPdfWriter>
#include <QRegularExpression>
#include <QSaveFile>
#include <QTemporaryFile>
#include <QTextDocument>
#include <QUrl>

#include <stdexcept>

#include "core/html_page.h"
#include "core/markdown_parser.h"
#include "core/markdown_renderer.h"

namespace mdeditor {
namespace {

[[noreturn]] void throwFileError(const QString& operation, const QString& path,
                                 const QString& reason) {
    const QByteArray message =
        QStringLiteral("%1 \"%2\": %3")
            .arg(operation, QDir::toNativeSeparators(path), reason)
            .toUtf8();
    throw std::runtime_error(message.constData());
}

QString decodeText(const QByteArray& bytes) {
    QByteArray contents = bytes;
    // The byte order mark is not a part of the text.
    if (contents.startsWith(QByteArrayLiteral("\xEF\xBB\xBF"))) contents.remove(0, 3);
    // The text is UTF-8 when it turns back into the same bytes. A file in
    // another encoding is read in the encoding of the system.
    QString text = QString::fromUtf8(contents);
    if (text.toUtf8() != contents) text = QString::fromLocal8Bit(contents);
    text.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    text.replace(u'\r', u'\n');
    return text;
}

// Writes the bytes to the file so that the old content stays there when the
// writing fails.
void writeFile(const QString& path, const QByteArray& contents) {
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        throwFileError(QStringLiteral("Не удалось открыть файл для записи"), path,
                       file.errorString());
    }
    if (file.write(contents) != contents.size()) {
        throwFileError(QStringLiteral("Не удалось записать файл"), path, file.errorString());
    }
    if (!file.commit()) {
        throwFileError(QStringLiteral("Не удалось сохранить файл"), path, file.errorString());
    }
}

// Makes the paths of the images in the HTML relative to another directory, so
// that the images are found where the HTML is saved.
QString redirectImages(const QString& html, const QString& sourceDirectory,
                       const QString& outputDirectory) {
    const QRegularExpression image(QStringLiteral("<img src=\"([^\"]*)\""));
    QString result;
    int position = 0;
    auto match = image.globalMatch(html);
    while (match.hasNext()) {
        const QRegularExpressionMatch found = match.next();
        result += html.mid(position, found.capturedStart(1) - position);
        const QString source = found.captured(1);
        // An address with a scheme, or an absolute path, is left as it is.
        const QString path = QUrl::fromPercentEncoding(source.toUtf8());
        const bool isRelative = QUrl(source).scheme().isEmpty() && !QDir::isAbsolutePath(path);
        result += isRelative
                      ? md::encodeUrl(QDir(outputDirectory).relativeFilePath(
                            QDir(sourceDirectory).filePath(path)))
                      : source;
        position = found.capturedEnd(1);
    }
    result += html.mid(position);
    return result;
}

}  // namespace

QString DocumentIO::loadMarkdown(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        throwFileError(QStringLiteral("Не удалось открыть файл"), path, file.errorString());
    }
    const QByteArray contents = file.readAll();
    if (file.error() != QFileDevice::NoError) {
        throwFileError(QStringLiteral("Не удалось прочитать файл"), path, file.errorString());
    }
    return decodeText(contents);
}

void DocumentIO::saveMarkdown(const QString& path, const QString& markdown) {
    writeFile(path, markdown.toUtf8());
}

std::unique_ptr<QTextDocument> DocumentIO::createPreviewDocument(const QString& markdown,
                                                                 const QString& sourceDirectory,
                                                                 const TextStyle& style,
                                                                 ImageCache* images) {
    return createRichTextDocument(markdown, sourceDirectory, style, images);
}

QString DocumentIO::renderHtml(const QString& markdown, const QString& sourceDirectory,
                               const QString& outputDirectory, const QString& title) {
    QString body = markdownToHtml(markdown);
    const bool isElsewhere =
        !sourceDirectory.isEmpty() && !outputDirectory.isEmpty() &&
        QDir(sourceDirectory).canonicalPath() != QDir(outputDirectory).canonicalPath();
    if (isElsewhere) body = redirectImages(body, sourceDirectory, outputDirectory);
    return wrapHtmlPage(
        body, documentTitle(markdown, title.isEmpty() ? QStringLiteral("Документ Markdown") : title));
}

void DocumentIO::exportHtml(const QString& path, const QString& markdown,
                            const QString& sourceDirectory) {
    const QString directory = QFileInfo(path).absolutePath();
    // When the document has no heading of the first level, the page is named after its file.
    writeFile(path, renderHtml(markdown, sourceDirectory, directory,
                               QFileInfo(path).completeBaseName())
                  .toUtf8());
}

void DocumentIO::exportPdf(const QString& path, const QString& markdown,
                           const QString& sourceDirectory, ImageCache* images) {
    // The PDF is made next to its destination and is put there only when it
    // is complete. When the export fails, the file that may be there stays
    // as it is.
    const QString directory = QFileInfo(path).absolutePath();
    QTemporaryFile temporary(QDir(directory).filePath(QStringLiteral(".mdeditor-XXXXXX.pdf")));
    temporary.setAutoRemove(true);
    if (!temporary.open()) {
        throwFileError(QStringLiteral("Не удалось создать файл в каталоге"), directory,
                       temporary.errorString());
    }
    const QString temporaryPath = temporary.fileName();
    temporary.close();

    ImageCache ownCache;
    if (images == nullptr) images = &ownCache;
    auto document = createRichTextDocument(markdown, sourceDirectory, defaultTextStyle(), images);
    {
        QPdfWriter pdf(temporaryPath);
        // The function that prints the document puts margins of two
        // centimeters of its own around the text. The writer gets margins of
        // two millimeters only: the page has margins of a little more than
        // two centimeters, and the page number has its place under the text.
        const QPageLayout layout(QPageSize(QPageSize::A4), QPageLayout::Portrait,
                                 QMarginsF(2, 2, 2, 2), QPageLayout::Millimeter);
        pdf.setPageLayout(layout);
        pdf.setTitle(documentTitle(markdown, QStringLiteral("Документ Markdown")));
        pdf.setCreator(QStringLiteral("mdeditor"));

        // The images are fitted into the width that is left for the text:
        // between the margins of the page and the margins of the document.
        const QRectF page = pdf.pageLayout().paintRectPixels(pdf.resolution());
        const qreal innerMargin = (2.0 / 2.54) * pdf.resolution();
        const qreal textWidth = page.width() - 2 * innerMargin;
        fitImagesToWidth(*document, textWidth, &pdf);
        document->print(&pdf);
    }

    QFile generated(temporaryPath);
    if (!generated.open(QIODevice::ReadOnly)) {
        throwFileError(QStringLiteral("Не удалось создать PDF"), path, generated.errorString());
    }
    const QByteArray contents = generated.readAll();
    if (!contents.startsWith("%PDF-")) {
        throwFileError(QStringLiteral("Не удалось создать PDF"), path,
                       QStringLiteral("движок записи вернул файл другого формата"));
    }
    writeFile(path, contents);
}

}  // namespace mdeditor
