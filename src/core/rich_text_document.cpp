#include "core/rich_text_document.h"

#include <QAbstractTextDocumentLayout>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QImageReader>
#include <QPaintDevice>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFragment>
#include <QTextImageFormat>
#include <QTextLayout>
#include <QUrl>

#include "core/html_page.h"
#include "core/markdown_parser.h"
#include "core/markdown_renderer.h"

namespace mdeditor {
namespace {

// The images with a longer side than this are made smaller when they are
// loaded: neither the preview nor a page can show more detail, and the whole
// image would take a lot of memory.
constexpr int kMaxImageSide = 4096;

// The cache does not grow past this number of images: then it starts over.
constexpr int kMaxCachedImages = 200;

QImage readImage(const QString& path) {
    QImageReader reader(path);
    const QSize size = reader.size();
    if (size.isValid() && (size.width() > kMaxImageSide || size.height() > kMaxImageSide)) {
        reader.setScaledSize(size.scaled(kMaxImageSide, kMaxImageSide, Qt::KeepAspectRatio));
    }
    return reader.read();
}

// The file that the image of a document is in. Returns an empty string when
// the address does not point to a local file.
QString imagePath(const QString& address, const QString& sourceDirectory) {
    // An absolute path is taken as it is: on Windows it starts with the
    // letter of the disk, which looks like the scheme of an address.
    const QString path = QUrl::fromPercentEncoding(address.toUtf8());
    if (QDir::isAbsolutePath(path)) return path;
    const QUrl url(address);
    if (url.scheme() == QLatin1String("file")) return url.toLocalFile();
    // An address with a scheme, such as https:, is not a local file.
    if (!url.scheme().isEmpty()) return QString();
    if (sourceDirectory.isEmpty()) return path;
    return QDir(sourceDirectory).filePath(path);
}

// Gives the document its images. The engine shows a placeholder for an image
// that cannot be loaded.
void loadImages(QTextDocument& document, const QString& sourceDirectory, ImageCache* images) {
    for (QTextBlock block = document.begin(); block.isValid(); block = block.next()) {
        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            if (!fragment.isValid() || !fragment.charFormat().isImageFormat()) continue;
            const QTextImageFormat format = fragment.charFormat().toImageFormat();
            const QString path = imagePath(format.name(), sourceDirectory);
            if (path.isEmpty()) continue;
            const QImage image = images->image(path);
            if (!image.isNull()) {
                document.addResource(QTextDocument::ImageResource, QUrl(format.name()), image);
            }
        }
    }
}

}  // namespace

QImage ImageCache::image(const QString& path) {
    const auto found = images_.constFind(path);
    if (found != images_.constEnd()) return found.value();
    if (images_.size() >= kMaxCachedImages) images_.clear();
    const QImage image = readImage(path);
    images_.insert(path, image);
    return image;
}

void ImageCache::clear() { images_.clear(); }

TextStyle defaultTextStyle() {
    TextStyle style;
    style.fontFamily = QGuiApplication::font().family();
    style.codeFontFamily = QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
    return style;
}

std::unique_ptr<QTextDocument> createRichTextDocument(const QString& markdown,
                                                       const QString& sourceDirectory,
                                                       const TextStyle& style,
                                                       ImageCache* images) {
    auto document = std::make_unique<QTextDocument>();
    // The document is made anew when the text changes, so it needs no undo history.
    document->setUndoRedoEnabled(false);
    QFont font(style.fontFamily);
    font.setPointSizeF(style.fontSize > 0 ? style.fontSize : 11.0);
    document->setDefaultFont(font);
    // The style sheet has to be set before the HTML: it is used while the HTML is read.
    document->setDefaultStyleSheet(richTextStyleSheet(style.codeFontFamily));
    document->setHtml(markdownToHtml(markdown, MarkdownRenderOptions{RenderTarget::RichText}));
    // Without a cache of its own the document loads the images and forgets them.
    ImageCache ownCache;
    loadImages(*document, sourceDirectory, images != nullptr ? images : &ownCache);
    return document;
}

namespace {

// How many units of the device one pixel of an image takes in a document
// that the device paints. The engine of Qt decides it from the resolution
// of the device, so it is found out by laying out a picture of a known size
// for the device instead of being computed.
qreal imageUnit(QPaintDevice& device, qreal availableWidth) {
    QImage probe(2, 2, QImage::Format_ARGB32);
    probe.fill(Qt::black);
    QTextDocument document;
    document.setHtml(QStringLiteral("<p><img src=\"probe\"></p>"));
    document.addResource(QTextDocument::ImageResource, QUrl(QStringLiteral("probe")), probe);
    constexpr qreal kProbeWidth = 100.0;
    QTextCursor cursor(&document);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    QTextImageFormat format = cursor.charFormat().toImageFormat();
    format.setWidth(kProbeWidth);
    format.setHeight(kProbeWidth);
    cursor.setCharFormat(format);

    document.documentLayout()->setPaintDevice(&device);
    document.setPageSize(QSizeF(availableWidth, 1000000));
    return document.firstBlock().layout()->lineAt(0).naturalTextWidth() / kProbeWidth;
}

}  // namespace

void fitImagesToWidth(QTextDocument& document, qreal width, QPaintDevice* device) {
    if (width <= 0) return;
    // On the screen one pixel of an image is one unit of the width.
    const qreal scale = device != nullptr ? imageUnit(*device, width) : 1.0;
    QTextCursor cursor(&document);
    cursor.beginEditBlock();
    for (QTextBlock block = document.begin(); block.isValid(); block = block.next()) {
        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            if (!fragment.isValid() || !fragment.charFormat().isImageFormat()) continue;
            const QTextFormat format = fragment.charFormat();
            const QTextImageFormat image = format.toImageFormat();
            const QImage pixmap =
                document.resource(QTextDocument::ImageResource, QUrl(image.name())).value<QImage>();
            if (pixmap.isNull() || pixmap.width() == 0) continue;

            QTextImageFormat fitted = image;
            if (pixmap.width() * scale > width) {
                // The size of an image in the document is in pixels, and
                // the engine makes as many units of it as it makes of a pixel.
                const qreal widthInPixels = width / scale;
                fitted.setWidth(widthInPixels);
                fitted.setHeight(widthInPixels * pixmap.height() / pixmap.width());
            } else {
                // The image fits: it is shown at its natural size.
                fitted.clearProperty(QTextFormat::ImageWidth);
                fitted.clearProperty(QTextFormat::ImageHeight);
            }
            if (static_cast<const QTextFormat&>(fitted) != format) {
                cursor.setPosition(fragment.position());
                cursor.setPosition(fragment.position() + 1, QTextCursor::KeepAnchor);
                cursor.setCharFormat(fitted);
            }
        }
    }
    cursor.endEditBlock();
}

}  // namespace mdeditor
