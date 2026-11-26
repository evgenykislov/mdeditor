#pragma once

#include <QString>

#include <memory>

#include "core/rich_text_document.h"

class QTextDocument;

namespace mdeditor {

// The files of the editor and the documents that are made of the Markdown
// text. The functions throw std::runtime_error with a message that can be
// shown to the user when something goes wrong.
class DocumentIO {
public:
    // Reads a text file as UTF-8. A file that is not UTF-8 is read in the
    // encoding of the system. Line breaks of all kinds become LF.
    static QString loadMarkdown(const QString& path);

    // Writes the text to the file in UTF-8. The file keeps its old content
    // when the writing fails.
    static void saveMarkdown(const QString& path, const QString& markdown);

    // The document of the preview. It has no owner: the caller becomes its
    // owner. The paths of the images are relative to sourceDirectory.
    static std::unique_ptr<QTextDocument> createPreviewDocument(const QString& markdown,
                                                                 const QString& sourceDirectory,
                                                                 const TextStyle& style,
                                                                 ImageCache* images);

    // A standalone HTML page with the rendered Markdown. The paths of the
    // images become relative to outputDirectory when it differs from
    // sourceDirectory, the directory of the Markdown file: they keep working
    // where the page is saved. The title is the title of the page when the
    // document has no heading of the first level.
    static QString renderHtml(const QString& markdown, const QString& sourceDirectory = QString(),
                              const QString& outputDirectory = QString(),
                              const QString& title = QString());

    static void exportHtml(const QString& path, const QString& markdown,
                           const QString& sourceDirectory = QString());

    // Writes the rendered Markdown to a PDF file of A4 pages.
    static void exportPdf(const QString& path, const QString& markdown,
                          const QString& sourceDirectory = QString(), ImageCache* images = nullptr);
};

}  // namespace mdeditor
