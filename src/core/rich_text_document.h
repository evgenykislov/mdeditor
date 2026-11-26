#pragma once

#include <QHash>
#include <QImage>
#include <QString>

#include <memory>

class QPaintDevice;
class QTextDocument;

namespace mdeditor {

// The fonts of the document that the rich text engine of Qt shows and prints.
struct TextStyle {
    QString fontFamily;
    QString codeFontFamily;
    // In points.
    double fontSize = 11.0;
};

// The fonts of the system: the font of its interface for the text and the
// monospaced font of the system for the code.
TextStyle defaultTextStyle();

// The images of the documents by file path. A null image stands for a file
// that could not be loaded: the disk is not asked for it again.
class ImageCache {
public:
    QImage image(const QString& path);
    void clear();

private:
    QHash<QString, QImage> images_;
};

// The document that shows the Markdown text in the rich text engine of Qt.
// The images are loaded through the cache: a relative path is looked for in
// sourceDirectory. The document has no owner: the caller becomes its owner.
std::unique_ptr<QTextDocument> createRichTextDocument(const QString& markdown,
                                                       const QString& sourceDirectory,
                                                       const TextStyle& style,
                                                       ImageCache* images);

// Shrinks the images that are wider than width so that they fit into it. The
// other images are shown at their natural size. The width is in the units of
// the device that will paint the document: the page of a PDF has many more
// of them than the screen has in the same place. Without a device the width
// is in the pixels of the screen.
void fitImagesToWidth(QTextDocument& document, qreal width, QPaintDevice* device = nullptr);

}  // namespace mdeditor
