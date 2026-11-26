#include "core/markdown_renderer.h"

#include <QChar>
#include <QStringList>

#include <cstddef>
#include <optional>
#include <vector>

#include "core/markdown_parser.h"
#include "core/markdown_structure.h"

namespace mdeditor {
namespace {

using md::Block;
using md::CellAlignment;
using md::Fence;
using md::HtmlBlockStart;
using md::InlineContext;
using md::LinkDefinitions;
using md::ListItem;
using md::TextBlock;
using md::kMaxMarkerIndent;
using md::kTabWidth;

// What the renderer has to know about the blocks around the one it renders.
struct BlockStyle {
    const LinkDefinitions* links = nullptr;
    const MarkdownRenderOptions* options = nullptr;
    // Inside a tight list item paragraphs are not wrapped into <p>.
    bool tight = false;
    // Emitted before the first text of the item: the checkbox of a task.
    QString leadingMarkup;
    // What the rendered blocks are. The renderer of a list item uses it to
    // decide where the line breaks inside <li> go.
    int blockCount = 0;
    bool onlyParagraphs = true;
    bool startsWithParagraph = false;
    bool endsWithParagraph = false;
};

QString renderBlockSequence(const QStringList& lines, BlockStyle* style);

InlineContext inlineContext(const BlockStyle& style) {
    InlineContext context;
    context.links = style.links;
    return context;
}

// "<tag>content</tag>" followed by a line break.
QString element(const QString& tag, const QString& content) {
    return QStringLiteral("<") + tag + QStringLiteral(">") + content +
           QStringLiteral("</") + tag + QStringLiteral(">\n");
}

QString codeBlock(const QStringList& lines, const QString& language) {
    QString attributes;
    if (!language.isEmpty()) {
        attributes = QStringLiteral(" class=\"language-") + md::escapeHtml(language) +
                     QStringLiteral("\"");
    }
    QString text;
    if (!lines.isEmpty()) text = md::escapeHtml(lines.join(u'\n')) + QChar(u'\n');
    return QStringLiteral("<pre><code") + attributes + QStringLiteral(">") + text +
           QStringLiteral("</code></pre>\n");
}

QString taskListMarker(bool checked, RenderTarget target) {
    // The rich text engine of Qt cannot draw <input> elements.
    if (target == RenderTarget::RichText) {
        return checked ? QStringLiteral("☑ ") : QStringLiteral("☐ ");
    }
    return QStringLiteral("<input ") + (checked ? QStringLiteral("checked=\"\" ") : QString()) +
           QStringLiteral("disabled=\"\" type=\"checkbox\"> ");
}

QString tableRow(const QString& tag, const std::vector<CellAlignment>& alignments,
                 const QStringList& cells, const InlineContext& context) {
    QString row = QStringLiteral("<tr>\n");
    for (std::size_t column = 0; column < alignments.size(); ++column) {
        // A row with fewer cells than the header is padded with empty cells.
        const QString content = static_cast<int>(column) < cells.size()
                                    ? md::renderInline(cells.at(static_cast<int>(column)),
                                                       context)
                                    : QString();
        QString attributes;
        switch (alignments[column]) {
        case CellAlignment::Left:
            attributes = QStringLiteral(" align=\"left\"");
            break;
        case CellAlignment::Center:
            attributes = QStringLiteral(" align=\"center\"");
            break;
        case CellAlignment::Right:
            attributes = QStringLiteral(" align=\"right\"");
            break;
        case CellAlignment::None:
            break;
        }
        row += QStringLiteral("<") + tag + attributes + QStringLiteral(">") + content +
               QStringLiteral("</") + tag + QStringLiteral(">\n");
    }
    return row + QStringLiteral("</tr>\n");
}

// The first line of the block is the header row, the second one is the
// delimiter row, the rest of them are the rows of the table.
QString renderTable(const QStringList& lines, int from, int end, const BlockStyle& style) {
    const InlineContext context = inlineContext(style);
    const QStringList header = md::splitTableRow(lines.at(from));
    const std::vector<CellAlignment> alignments =
        *md::matchTableDelimiterRow(lines.at(from + 1), header.size());
    QString html = QStringLiteral("<table>\n<thead>\n");
    html += tableRow(QStringLiteral("th"), alignments, header, context);
    html += QStringLiteral("</thead>\n");
    if (from + 2 < end) {
        html += QStringLiteral("<tbody>\n");
        for (int index = from + 2; index < end; ++index) {
            html += tableRow(QStringLiteral("td"), alignments, md::splitTableRow(lines.at(index)),
                             context);
        }
        html += QStringLiteral("</tbody>\n");
    }
    html += QStringLiteral("</table>\n");
    return html;
}

// The lines of a paragraph are joined with line breaks. The indentation of
// the lines is not a part of the text.
QString joinParagraphLines(const QStringList& lines) {
    QString text;
    for (const QString& line : lines) {
        if (!text.isEmpty()) text += QChar(u'\n');
        text += md::stripLeadingWhitespace(line);
    }
    return text;
}

QString renderTextBlock(const QStringList& lines, int from, const TextBlock& block,
                        BlockStyle* style) {
    switch (block.kind) {
    case md::TextBlockKind::Definitions:
        // Link reference definitions do not have an output of their own.
        return QString();
    case md::TextBlockKind::Paragraph: {
        const QString text =
            md::renderInline(joinParagraphLines(lines.mid(from, block.end - from)),
                             inlineContext(*style));
        if (style->tight) {
            const QString markup = style->leadingMarkup;
            style->leadingMarkup.clear();
            return markup + text + QStringLiteral("\n");
        }
        QString html = QStringLiteral("<p>");
        if (!style->leadingMarkup.isEmpty()) {
            html += style->leadingMarkup;
            style->leadingMarkup.clear();
        }
        return html + text + QStringLiteral("</p>\n");
    }
    case md::TextBlockKind::SetextHeading:
        // The underline is the last line of the block.
        return element(QStringLiteral("h%1").arg(block.headingLevel),
                       md::renderInline(joinParagraphLines(lines.mid(from, block.end - from - 1)),
                                        inlineContext(*style)));
    case md::TextBlockKind::Table:
        return renderTable(lines, from, block.end, *style);
    }
    return QString();
}

QString renderList(const QStringList& lines, int from, const Block& block,
                   const BlockStyle& style) {
    const md::ListMarker first = *md::matchListMarker(lines.at(from));

    // The list is loose when its items are separated by blank lines or an
    // item contains blocks separated by a blank line.
    bool isLoose = false;
    for (const ListItem& item : block.items) isLoose |= item.blankLineBefore || item.blankLineInside;

    QString listTag = QStringLiteral("ul");
    QString attributes;
    if (first.ordered) {
        listTag = QStringLiteral("ol");
        if (first.number != 1) attributes = QStringLiteral(" start=\"%1\"").arg(first.number);
    }
    QString html = QStringLiteral("<") + listTag + attributes + QStringLiteral(">\n");
    for (const ListItem& item : block.items) {
        BlockStyle itemStyle = style;
        itemStyle.tight = !isLoose;
        itemStyle.blockCount = 0;
        itemStyle.onlyParagraphs = true;
        itemStyle.startsWithParagraph = false;
        itemStyle.endsWithParagraph = false;
        QStringList itemLines = item.lines;
        // A task list item starts with a checkbox.
        if (const auto isChecked = md::matchTaskMarker(itemLines.first())) {
            itemLines.first() = md::stripLeadingWhitespace(itemLines.first().mid(3));
            itemStyle.leadingMarkup = taskListMarker(*isChecked, style.options->target);
        }
        QString body = renderBlockSequence(itemLines, &itemStyle);

        // A block element is written on lines of its own inside the item. The
        // text of a paragraph of a tight list is written right after <li>, and
        // </li> follows it on the same line.
        const bool startsWithText = !isLoose && itemStyle.startsWithParagraph;
        if (!isLoose && itemStyle.endsWithParagraph) body.chop(1);
        const QString afterOpenTag = body.isEmpty() || startsWithText
                                         ? QString()
                                         : QStringLiteral("\n");
        html += QStringLiteral("<li>") + afterOpenTag + itemStyle.leadingMarkup + body +
                QStringLiteral("</li>\n");
    }
    html += QStringLiteral("</") + listTag + QStringLiteral(">\n");
    return html;
}

QString renderBlock(const QStringList& lines, int from, const Block& block, BlockStyle* style) {
    const QString& line = lines.at(from);
    const QString content = line.mid(md::indentation(line));
    switch (block.kind) {
    case Block::Kind::IndentedCode:
        return codeBlock(md::indentedCodeLines(lines, from, block.end), QString());
    case Block::Kind::Fence: {
        const Fence fence = *md::matchFence(line);
        const bool isClosed =
            block.end > from + 1 && md::isClosingFence(lines.at(block.end - 1), fence);
        QStringList body;
        const int contentEnd = block.end - (isClosed ? 1 : 0);
        for (int index = from + 1; index < contentEnd; ++index) {
            body << md::stripCodeIndentation(lines.at(index), fence.indent);
        }
        return codeBlock(body, md::unescapeText(fence.infoString.section(u' ', 0, 0)));
    }
    case Block::Kind::Heading:
        return element(QStringLiteral("h%1").arg(md::matchHeading(content)->level),
                       md::renderInline(md::matchHeading(content)->text, inlineContext(*style)));
    case Block::Kind::ThematicBreak:
        return QStringLiteral("<hr>\n");
    case Block::Kind::Quote: {
        BlockStyle innerStyle = *style;
        innerStyle.tight = false;
        innerStyle.leadingMarkup.clear();
        return QStringLiteral("<blockquote>\n") +
               renderBlockSequence(block.quoteLines, &innerStyle) +
               QStringLiteral("</blockquote>\n");
    }
    case Block::Kind::List:
        return renderList(lines, from, block, *style);
    case Block::Kind::Html:
        // Raw HTML is passed through unchanged.
        return QStringList(lines.mid(from, block.end - from)).join(u'\n') + QChar(u'\n');
    case Block::Kind::Text:
        return renderTextBlock(lines, from, block.text, style);
    }
    return QString();
}

QString renderBlockSequence(const QStringList& lines, BlockStyle* style) {
    QString html;
    int index = 0;
    while (index < lines.size()) {
        if (md::isBlank(lines.at(index))) {
            ++index;
            continue;
        }
        const Block block = md::nextBlock(lines, index);
        const bool isParagraph = block.kind == Block::Kind::Text &&
                                 block.text.kind == md::TextBlockKind::Paragraph;
        // Link reference definitions have no output: they are not counted.
        const bool hasOutput =
            block.kind != Block::Kind::Text || block.text.kind != md::TextBlockKind::Definitions;
        if (hasOutput) {
            if (style->blockCount == 0) style->startsWithParagraph = isParagraph;
            ++style->blockCount;
            style->onlyParagraphs &= isParagraph;
            style->endsWithParagraph = isParagraph;
        }
        html += renderBlock(lines, index, block, style);
        index = block.end;
    }
    return html;
}

// The lines of the Markdown text. Line breaks of all kinds become LF, and the
// line break at the very end of the text does not make an empty line.
QStringList splitLines(const QString& text) {
    QString normalized = text;
    if (normalized.startsWith(QChar(0xFEFF))) normalized.remove(0, 1);
    normalized.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    normalized.replace(u'\r', u'\n');
    QStringList lines = normalized.split(u'\n');
    if (lines.size() > 1 && lines.last().isEmpty()) lines.removeLast();
    return lines;
}

QString plainText(const QString& text) {
    InlineContext context;
    context.plainText = true;
    return md::renderInline(text, context);
}

}  // namespace

QString markdownToHtml(const QString& markdown, const MarkdownRenderOptions& options) {
    const QStringList lines = splitLines(markdown);
    // Link reference definitions are collected from the whole document first:
    // a reference link may come before its definition.
    const LinkDefinitions links = md::collectLinkDefinitions(lines);
    BlockStyle style;
    style.links = &links;
    style.options = &options;
    QString html = renderBlockSequence(lines, &style);
    // Only the line breaks between the blocks are a part of the result.
    while (html.endsWith(u'\n')) html.chop(1);
    return html;
}

QString documentTitle(const QString& markdown, const QString& fallback) {
    const QStringList lines = splitLines(markdown);
    std::optional<Fence> fence;
    for (int index = 0; index < lines.size(); ++index) {
        const QString& line = lines.at(index);
        if (fence) {
            if (md::isClosingFence(line, *fence)) fence.reset();
            continue;
        }
        if (md::isBlank(line)) continue;
        if (const auto opening = md::matchFence(line)) {
            fence = opening;
            continue;
        }
        const int indent = md::indentation(line);
        // An indented code block cannot contain a heading.
        if (indent > kMaxMarkerIndent) continue;
        const QString content = md::stripTrailing(line.mid(indent));
        const auto heading = md::matchHeading(content);
        if (heading && heading->level == 1 && !heading->text.isEmpty()) {
            return plainText(heading->text);
        }
        // A line followed by a line of '=' characters is a level one heading too.
        if (index + 1 < lines.size() && md::isSetextUnderline(lines.at(index + 1)) &&
            md::setextLevel(md::stripTrailing(lines.at(index + 1).mid(
                                 md::indentation(lines.at(index + 1))))) == 1 &&
            !md::isThematicBreak(content)) {
            return plainText(content);
        }
    }
    return fallback;
}

}  // namespace mdeditor
