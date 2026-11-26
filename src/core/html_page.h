#pragma once

#include <QString>

namespace mdeditor {

// A standalone HTML page with the rendered Markdown inside. The page carries
// its own style sheet and does not depend on any other file.
QString wrapHtmlPage(const QString& bodyHtml, const QString& title);

// The style sheet of the page for web browsers.
QString webStyleSheet();

// The style sheet for the rich text engine of Qt, which draws the preview and
// makes the PDF export. The engine understands only a part of CSS, so the
// page gets a style sheet of its own. codeFontFamily is the font of the code.
QString richTextStyleSheet(const QString& codeFontFamily);

}  // namespace mdeditor
