#include "core/markdown_renderer.h"

#include <QFile>
#include <QTextStream>

// Development tool: reads Markdown from standard input and prints the HTML
// fragment and the title that the converter makes from it.
int main() {
    QTextStream input(stdin, QIODevice::ReadOnly);
    input.setCodec("UTF-8");
    const QString markdown = input.readAll();
    QTextStream out(stdout, QIODevice::WriteOnly);
    out.setCodec("UTF-8");
    out << mdeditor::markdownToHtml(markdown) << "\n";
    out << "TITLE: " << mdeditor::documentTitle(markdown, "<fallback>") << "\n";
    return 0;
}
