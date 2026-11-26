#include "core/markdown_structure.h"

#include <QChar>

#include <utility>

namespace mdeditor::md {
namespace {

// The content of a line of a block quote: without the marker and one column
// of whitespace after it.
QString quoteContent(const QString& line) {
    // The marker is replaced with a space and removed together with one
    // column of whitespace after it. That way the tab stops of the result are
    // the tab stops of the line.
    const int indent = indentation(line);
    return stripColumns(QString(indent + 1, u' ') + line.mid(indent + 1), indent + 2);
}

// A Setext heading underline cannot continue a paragraph lazily: it is
// paragraph text. The characters are escaped, so that the line stays text for
// the renderer too.
QString asTextLine(const QString& line) {
    const int indent = indentation(line);
    QString text = line.left(indent);
    for (const QChar c : stripTrailing(line.mid(indent))) {
        text += QChar(u'\\');
        text += c;
    }
    return text;
}

// The content of a block quote that starts at from. The lines that continue
// a paragraph of the quote lazily, without the marker, are a part of it.
QStringList collectQuote(const QStringList& lines, int from, int* end) {
    QStringList inner;
    // Whether it is known that the lines collected so far end with a
    // paragraph that is not closed. Finding it out is costly, so it is done
    // only when a lazy continuation line makes it necessary.
    bool paragraphIsOpen = false;
    int index = from;
    while (index < lines.size()) {
        const QString& line = lines.at(index);
        if (isQuoteLine(line)) {
            const QString content = quoteContent(line);
            // A blank line or a new block closes the paragraph.
            if (isBlank(content) || interruptsParagraph(content)) paragraphIsOpen = false;
            inner << content;
            ++index;
            continue;
        }
        // A blank line ends the quote: block quotes that are separated by
        // blank lines are separate. A blank line of the quote itself has the
        // marker.
        if (isBlank(line)) break;
        // The lines below have neither the marker nor the indentation of a
        // block of code: they can only continue a paragraph.
        if (!paragraphIsOpen && !endsWithOpenParagraph(inner)) break;
        // A line that starts a block begins it outside the quote. A Setext
        // heading underline is not a block start: it is text. The same line
        // of dashes may be a thematic break, and then it is one.
        const bool isUnderline = !isThematicBreak(line) && isSetextUnderline(line);
        if (!isUnderline && startsBlock(line)) break;
        inner << (isUnderline ? asTextLine(line) : line);
        paragraphIsOpen = true;
        ++index;
    }
    *end = index;
    return inner;
}

// Whether the lines have a blank line between their blocks. The blank lines
// inside a block, such as a fenced code block or a nested list, are a part of
// that block and do not count.
bool hasBlankLineBetweenBlocks(QStringList lines) {
    // The empty first line of a list item is not between two blocks.
    while (!lines.isEmpty() && isBlank(lines.first())) lines.removeFirst();
    int index = 0;
    while (index < lines.size()) {
        if (isBlank(lines.at(index))) return true;
        index = nextBlock(lines, index).end;
    }
    return false;
}

// The lines of the list item that starts at from. Returns the index of the
// first line that does not belong to the item.
int collectListItem(const QStringList& lines, int from, const ListMarker& marker,
                    ListItem* item) {
    // The marker itself is not a part of the content. The indentation of the
    // item is counted from the start of the line: the marker is replaced
    // with spaces, so that the tab stops of the line do not move.
    item->lines << stripColumns(QString(marker.end, u' ') + lines.at(from).mid(marker.end),
                                marker.contentIndent);
    // A list item can begin with at most one blank line: when its first line
    // is empty, the next blank line already ends it.
    const bool beginsBlank = isBlank(item->lines.first());
    bool hasContent = !beginsBlank;
    bool paragraphIsOpen = false;
    int blankLines = 0;
    int next = from + 1;
    while (next < lines.size()) {
        const QString& line = lines.at(next);
        if (isBlank(line)) {
            if (!hasContent) break;
            ++blankLines;
            ++next;
            continue;
        }
        if (indentation(line) >= marker.contentIndent) {
            // An indented line is a part of the item. The blank lines before
            // it are a part of it too.
            for (int blank = 0; blank < blankLines; ++blank) item->lines << QString();
            item->lines << stripColumns(line, marker.contentIndent);
            blankLines = 0;
            hasContent = true;
            paragraphIsOpen = false;
            ++next;
            continue;
        }
        const QString content = line.mid(indentation(line));
        // A thematic break ends the list, and any list marker starts another
        // item or another list.
        if (isThematicBreak(content) || matchListMarker(line)) break;
        // A paragraph cannot continue after a blank line.
        if (blankLines > 0) break;
        // The lines below are not indented enough: they can only continue a
        // paragraph of the item.
        if (!paragraphIsOpen && !endsWithOpenParagraph(item->lines)) break;
        // A line that starts a block begins it outside the item. A Setext
        // heading underline is not a block start: it is text.
        const bool isUnderline = isSetextUnderline(line);
        if (!isUnderline && startsBlock(line)) break;
        item->lines << (isUnderline ? asTextLine(line) : line);
        paragraphIsOpen = true;
        ++next;
    }
    item->blankLinesAfter = blankLines;
    item->blankLineInside = hasBlankLineBetweenBlocks(item->lines);
    return next;
}

// The items of the list that starts at from. The list ends at a line that is
// not a marker of the same kind or at a thematic break.
std::vector<ListItem> collectList(const QStringList& lines, int from, int* end) {
    const ListMarker first = *matchListMarker(lines.at(from));
    std::vector<ListItem> items;
    int blankLines = 0;
    int index = from;
    while (index < lines.size()) {
        const QString& line = lines.at(index);
        if (isBlank(line)) {
            ++blankLines;
            ++index;
            continue;
        }
        const auto marker = matchListMarker(line);
        if (!marker || !isSameList(first, *marker) ||
            isThematicBreak(line.mid(indentation(line)))) {
            break;
        }
        ListItem item;
        item.blankLineBefore = blankLines > 0 && !items.empty();
        index = collectListItem(lines, index, *marker, &item);
        blankLines = item.blankLinesAfter;
        items.push_back(std::move(item));
    }
    // The blank lines after the last item are not a part of the list.
    *end = index - blankLines;
    return items;
}

// The text block that starts at from: link reference definitions, a
// paragraph, a Setext heading or a table.
TextBlock scanTextBlock(const QStringList& lines, int from) {
    const int count = lines.size();
    // Link reference definitions give targets to reference links. They are
    // removed from the document: when the text begins with them, the block
    // consists of them. A definition cannot leave the paragraph: it ends
    // where the first blank line is.
    int paragraphEnd = from;
    while (paragraphEnd < count && !isBlank(lines.at(paragraphEnd))) ++paragraphEnd;
    const int definitions = leadingDefinitionLines(lines, from, paragraphEnd);
    if (definitions > 0) return {TextBlockKind::Definitions, from + definitions, 0};

    int index = from;

    TextBlock block;
    int textLines = 0;
    while (index < count) {
        const QString& line = lines.at(index);
        if (isBlank(line)) break;
        // A header row followed by a delimiter row makes a table. The table
        // takes precedence over a Setext heading.
        if (textLines == 1 && lines.at(from).contains(u'|')) {
            const QStringList header = splitTableRow(lines.at(from));
            if (matchTableDelimiterRow(line, header.size())) {
                ++index;
                while (index < count && !isBlank(lines.at(index)) &&
                       !interruptsParagraph(lines.at(index))) {
                    ++index;
                }
                block.kind = TextBlockKind::Table;
                break;
            }
        }
        if (textLines > 0) {
            if (isSetextUnderline(line)) {
                block.kind = TextBlockKind::SetextHeading;
                block.headingLevel = setextLevel(stripTrailing(line.mid(indentation(line))));
                ++index;
                break;
            }
            if (interruptsParagraph(line)) break;
        }
        ++textLines;
        ++index;
    }
    block.end = index;
    return block;
}

}  // namespace

Block nextBlock(const QStringList& lines, int from) {
    const int count = lines.size();
    const QString& line = lines.at(from);
    Block block;

    // An indented code block: four columns of indentation or more.
    if (indentation(line) >= kTabWidth) {
        block.kind = Block::Kind::IndentedCode;
        block.end = indentedCodeEnd(lines, from, count);
        return block;
    }

    const QString content = line.mid(indentation(line));
    if (matchFence(line)) {
        block.kind = Block::Kind::Fence;
        const Fence fence = *matchFence(line);
        int end = from + 1;
        while (end < count && !isClosingFence(lines.at(end), fence)) ++end;
        // A fence that is not closed runs to the end of the document.
        block.end = end < count ? end + 1 : count;
        return block;
    }
    if (matchHeading(content)) {
        block.kind = Block::Kind::Heading;
        block.end = from + 1;
        return block;
    }
    if (isThematicBreak(content)) {
        block.kind = Block::Kind::ThematicBreak;
        block.end = from + 1;
        return block;
    }
    if (isQuoteLine(line)) {
        block.kind = Block::Kind::Quote;
        block.quoteLines = collectQuote(lines, from, &block.end);
        return block;
    }
    if (matchListMarker(line)) {
        block.kind = Block::Kind::List;
        block.items = collectList(lines, from, &block.end);
        return block;
    }
    if (const HtmlBlockStart html = matchHtmlBlock(content);
        html.kind != HtmlBlock::None) {
        block.kind = Block::Kind::Html;
        block.end = htmlBlockEnd(lines, from, count, html);
        return block;
    }
    block.kind = Block::Kind::Text;
    block.text = scanTextBlock(lines, from);
    block.end = block.text.end;
    return block;
}

bool endsWithOpenParagraph(const QStringList& lines) {
    bool isOpen = false;
    int index = 0;
    while (index < lines.size()) {
        if (isBlank(lines.at(index))) {
            isOpen = false;
            ++index;
            continue;
        }
        const Block block = nextBlock(lines, index);
        switch (block.kind) {
        case Block::Kind::Quote:
            isOpen = endsWithOpenParagraph(block.quoteLines);
            break;
        case Block::Kind::List:
            isOpen = !block.items.empty() && endsWithOpenParagraph(block.items.back().lines);
            break;
        case Block::Kind::Text:
            isOpen = block.text.kind == TextBlockKind::Paragraph;
            break;
        default:
            isOpen = false;
            break;
        }
        index = block.end;
    }
    return isOpen;
}

}  // namespace mdeditor::md
