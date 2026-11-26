#include "core/html_page.h"

#include "core/markdown_parser.h"

namespace mdeditor {

QString webStyleSheet() {
    return QStringLiteral(R"css(
:root { color-scheme: light; }
html { font-size: 16px; }
body {
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "Noto Sans", Helvetica, Arial, sans-serif;
  line-height: 1.5;
  color: #1f2328;
  background: #ffffff;
  max-width: 60em;
  margin: 2em auto;
  padding: 0 1em;
}
h1, h2, h3, h4, h5, h6 { line-height: 1.25; margin: 1.5em 0 0.6em; }
h1 { font-size: 2em; border-bottom: 1px solid #d0d7de; padding-bottom: 0.3em; }
h2 { font-size: 1.5em; border-bottom: 1px solid #d0d7de; padding-bottom: 0.3em; }
h3 { font-size: 1.25em; }
h4 { font-size: 1em; }
h5 { font-size: 0.875em; }
h6 { font-size: 0.85em; color: #59636e; }
p, ul, ol, blockquote, pre, table { margin: 0 0 1em; }
ul, ol { padding-left: 2em; }
li + li { margin-top: 0.25em; }
a { color: #0969da; text-decoration: none; }
a:hover { text-decoration: underline; }
blockquote { padding: 0 1em; color: #59636e; border-left: 0.25em solid #d0d7de; }
code, pre {
  font-family: ui-monospace, SFMono-Regular, "SF Mono", Menlo, Consolas, "Liberation Mono", monospace;
  font-size: 0.9em;
}
code { background: #eff1f3; padding: 0.15em 0.35em; border-radius: 3px; }
pre { line-height: 1.45; padding: 1em; overflow: auto; background: #f6f8fa; border-radius: 6px; }
pre code { font-size: 1em; background: none; padding: 0; }
table { border-collapse: collapse; border-spacing: 0; display: block; max-width: 100%; overflow: auto; }
th, td { border: 1px solid #d0d7de; padding: 6px 13px; }
th { font-weight: 600; }
tr:nth-child(2n) { background: #f6f8fa; }
img { max-width: 100%; }
hr { border: 0; border-top: 1px solid #d0d7de; margin: 1.5em 0; }
input[type="checkbox"] { margin: 0 0.4em 0 0; }
)css");
}

QString richTextStyleSheet(const QString& codeFontFamily) {
    // The family goes into the style sheet as a quoted name: the characters
    // that would end the name are not a part of it.
    QString family = codeFontFamily;
    family.remove(QChar(u'"'));
    family.remove(QChar(u'\\'));
    return QStringLiteral(R"css(
h1 { font-size: 200%; font-weight: bold; margin-top: 12px; margin-bottom: 8px; }
h2 { font-size: 150%; font-weight: bold; margin-top: 16px; margin-bottom: 6px; }
h3 { font-size: 125%; font-weight: bold; margin-top: 14px; margin-bottom: 6px; }
h4 { font-size: 110%; font-weight: bold; margin-top: 12px; margin-bottom: 4px; }
h5 { font-size: 100%; font-weight: bold; margin-top: 10px; margin-bottom: 4px; }
h6 { font-size: 100%; font-weight: bold; color: #59636e; margin-top: 10px; margin-bottom: 4px; }
p { margin-top: 0px; margin-bottom: 10px; }
ul, ol { margin-top: 0px; margin-bottom: 10px; }
li { margin-top: 0px; margin-bottom: 3px; }
blockquote { margin-top: 10px; margin-bottom: 10px; margin-left: 20px; margin-right: 16px; color: #59636e; }
pre { font-family: "%1"; background-color: #f6f8fa; white-space: pre-wrap; margin-top: 8px; margin-bottom: 12px; }
code { font-family: "%1"; background-color: #eff1f3; }
a { color: #0969da; text-decoration: underline; }
table { border-collapse: collapse; }
th, td { border: 1px solid #d0d7de; padding: 4px 8px; }
th { background-color: #f6f8fa; font-weight: bold; }
del { text-decoration: line-through; }
)css")
        .arg(family);
}

QString wrapHtmlPage(const QString& bodyHtml, const QString& title) {
    QString page = QStringLiteral(
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset=\"utf-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        "<meta name=\"generator\" content=\"mdeditor\">\n"
        "<title>");
    page += md::escapeHtml(title);
    page += QStringLiteral("</title>\n<style>\n");
    page += webStyleSheet();
    page += QStringLiteral("</style>\n</head>\n<body>\n");
    page += bodyHtml;
    page += QStringLiteral("\n</body>\n</html>\n");
    return page;
}

}  // namespace mdeditor
