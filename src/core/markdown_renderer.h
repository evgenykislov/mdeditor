#pragma once

#include <QString>

namespace mdeditor {

// Where the HTML goes: a file for web browsers, or the rich text engine of Qt
// that draws the preview and makes the PDF export.
enum class RenderTarget { WebPage, RichText };

struct MarkdownRenderOptions {
    RenderTarget target = RenderTarget::WebPage;
};

// Converts Markdown into a semantic HTML fragment: everything that goes
// between <body> and </body>, without the surrounding document.
QString markdownToHtml(const QString& markdown,
                       const MarkdownRenderOptions& options = {});

// The title of the document: the text of its first level one heading. The
// fallback is returned when the document has no such heading.
QString documentTitle(const QString& markdown, const QString& fallback);

}  // namespace mdeditor
