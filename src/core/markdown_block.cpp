#include "core/markdown_parser.h"

#include <QChar>

#include <algorithm>
#include <optional>

namespace mdeditor::md {
namespace {

bool isSpace(QChar character) { return character == u' ' || character == u'\t'; }

bool isAsciiDigit(QChar character) {
    const uint code = character.unicode();
    return code >= u'0' && code <= u'9';
}

bool isTagBoundary(const QString& text, int position) {
    if (position >= text.size()) return true;
    const QChar c = text.at(position);
    return c.isSpace() || c == u'/' || c == u'>';
}

bool isStructuralTag(const QString& name) {
    static const QStringList tags = {
        "address",    "article",  "aside",    "base",     "basefont", "blockquote", "body",
        "caption",    "center",   "col",      "colgroup", "dd",       "details",     "dialog",
        "dir",        "div",      "dl",       "dt",       "fieldset", "figcaption",  "figure",
        "footer",     "form",     "frame",     "frameset", "h1",       "h2",          "h3",
        "h4",         "h5",       "h6",       "head",     "header",   "hr",          "html",
        "iframe",     "legend",   "li",       "link",     "main",     "menu",        "menuitem",
        "nav",        "noframes", "ol",       "optgroup", "option",   "p",           "param",
        "search",     "section",  "summary",  "table",    "tbody",    "td",          "tfoot",
        "th",         "thead",    "title",    "tr",       "track",    "ul",
    };
    return tags.contains(name);
}

bool startsWithTag(const QString& content, int nameStart, const char* rawName) {
    const QString name = QLatin1String(rawName);
    return content.mid(nameStart, name.size()).compare(name, Qt::CaseInsensitive) == 0 &&
           isTagBoundary(content, nameStart + name.size());
}

// The number of spaces that stand for the leading whitespace of the line
// when tabs are counted with the tab stops of the line, and the index of the
// first character that is not whitespace.
void expandedIndentation(const QString& line, int* spaces, int* contentStart) {
    int index = 0;
    int column = 0;
    while (index < line.size() && isSpace(line.at(index))) {
        column += line.at(index) == u'\t' ? kTabWidth - column % kTabWidth : 1;
        ++index;
    }
    *spaces = column;
    *contentStart = index;
}

}  // namespace

bool isBlank(const QString& line) {
    for (const QChar c : line) {
        if (!c.isSpace()) return false;
    }
    return true;
}

// The number of columns that the line is indented by. A tab ends at the
// next tab stop.
int indentation(const QString& line) {
    int spaces = 0;
    int contentStart = 0;
    expandedIndentation(line, &spaces, &contentStart);
    return spaces;
}

// The line without the first columns of its indentation. This is the
// indentation that belongs to the structure of the document, so the rest of
// it is returned as spaces: that way the tab stops of the line stay where
// they were, wherever the text goes next.
QString stripColumns(const QString& line, int columns) {
    int spaces = 0;
    int contentStart = 0;
    expandedIndentation(line, &spaces, &contentStart);
    return QString(std::max(spaces - columns, 0), u' ') + line.mid(contentStart);
}

// The same for the content of a code block, where the tabs are a part of the
// content. Only a tab that has to be split is replaced by the spaces that
// its remaining columns stand for.
QString stripCodeIndentation(const QString& line, int columns) {
    int index = 0;
    int passed = 0;
    while (index < line.size() && passed < columns) {
        const QChar c = line.at(index);
        if (c == u' ') {
            ++passed;
            ++index;
        } else if (c == u'\t') {
            const int width = kTabWidth - passed % kTabWidth;
            if (passed + width > columns) {
                return QString(passed + width - columns, u' ') + line.mid(index + 1);
            }
            passed += width;
            ++index;
        } else {
            break;
        }
    }
    return line.mid(index);
}

QString stripLeadingWhitespace(const QString& line) {
    int index = 0;
    while (index < line.size() && line.at(index).isSpace()) ++index;
    return line.mid(index);
}

QString stripTrailing(const QString& line) {
    int end = line.size();
    while (end > 0 && line.at(end - 1).isSpace()) --end;
    return line.left(end);
}

std::optional<Fence> matchFence(const QString& line) {
    const int indent = indentation(line);
    if (indent > kMaxMarkerIndent || indent >= line.size()) return std::nullopt;
    const QChar marker = line.at(indent);
    if (marker != u'`' && marker != u'~') return std::nullopt;
    int end = indent;
    while (end < line.size() && line.at(end) == marker) ++end;
    const int length = end - indent;
    if (length < 3) return std::nullopt;
    const QString infoString = line.mid(end).trimmed();
    // Backticks cannot be used in the info string of a backtick fence.
    if (marker == u'`' && infoString.contains(u'`')) return std::nullopt;
    return Fence{marker, length, indent, infoString};
}

bool isClosingFence(const QString& line, const Fence& fence) {
    const int indent = indentation(line);
    if (indent > kMaxMarkerIndent || indent >= line.size() || line.at(indent) != fence.marker) {
        return false;
    }
    int end = indent;
    while (end < line.size() && line.at(end) == fence.marker) ++end;
    return end - indent >= fence.length && isBlank(line.mid(end));
}

std::optional<Heading> matchHeading(const QString& content) {
    int level = 0;
    while (level < content.size() && content.at(level) == u'#') ++level;
    if (level == 0 || level > 6) return std::nullopt;
    if (level < content.size() && !isSpace(content.at(level))) return std::nullopt;
    QString text = content.mid(level).trimmed();
    // The sequence of '#' characters at the end is optional. It is removed
    // when a space separates it from the title.
    int end = text.size();
    while (end > 0 && text.at(end - 1) == u'#') --end;
    if (end == 0) {
        text.clear();
    } else if (text.at(end - 1).isSpace()) {
        text.truncate(end);
        while (text.endsWith(u' ')) text.chop(1);
    }
    return Heading{level, text};
}

int setextLevel(const QString& content) {
    if (content.isEmpty()) return 0;
    const QChar marker = content.at(0);
    if (marker != u'=' && marker != u'-') return 0;
    for (const QChar c : content) {
        if (c != marker) return 0;
    }
    return marker == u'=' ? 1 : 2;
}

bool isSetextUnderline(const QString& line) {
    const int indent = indentation(line);
    // Spaces and tabs at the end of the line do not matter.
    return indent <= kMaxMarkerIndent && setextLevel(stripTrailing(line.mid(indent))) != 0;
}

bool isThematicBreak(const QString& content) {
    if (content.isEmpty()) return false;
    const QChar marker = content.at(0);
    if (marker != u'*' && marker != u'-' && marker != u'_') return false;
    int count = 0;
    for (const QChar c : content) {
        if (c == marker) {
            ++count;
        } else if (!c.isSpace()) {
            return false;
        }
    }
    return count >= 3;
}

bool isQuoteLine(const QString& line) {
    const int indent = indentation(line);
    return indent <= kMaxMarkerIndent && indent < line.size() && line.at(indent) == u'>';
}

std::optional<ListMarker> matchListMarker(const QString& line) {
    const int indent = indentation(line);
    if (indent > kMaxMarkerIndent || indent >= line.size()) return std::nullopt;
    ListMarker marker;
    marker.indent = indent;
    int position = indent;
    const QChar first = line.at(indent);
    if (first == u'-' || first == u'*' || first == u'+') {
        marker.bullet = first;
        ++position;
    } else {
        if (!isAsciiDigit(first)) return std::nullopt;
        int digits = 0;
        while (position < line.size() && isAsciiDigit(line.at(position))) {
            marker.number = marker.number * 10 + line.at(position).digitValue();
            ++position;
            ++digits;
        }
        if (digits > 9 || position >= line.size() ||
            (line.at(position) != u'.' && line.at(position) != u')')) {
            return std::nullopt;
        }
        marker.ordered = true;
        marker.delimiter = line.at(position);
        ++position;
    }
    // The marker has to be followed by whitespace: only an empty item ends
    // right after the marker. Only a space or a tab count as whitespace here.
    if (position < line.size() && !isSpace(line.at(position))) return std::nullopt;
    marker.end = position;
    // The columns between the marker and the content. No more than five of
    // them make a difference.
    int column = position;
    while (position < line.size() && isSpace(line.at(position))) {
        column += line.at(position) == u'\t' ? kTabWidth - column % kTabWidth : 1;
        ++position;
        if (column - marker.end > kTabWidth) break;
    }
    // The content starts right after one column when the first line of the
    // item is empty or is indented by five columns or more: such content is
    // an indented code block.
    const int spaces = column - marker.end;
    const bool isEmpty = isBlank(line.mid(marker.end));
    marker.contentIndent = marker.end + (isEmpty || spaces > kTabWidth ? 1 : spaces);
    return marker;
}

bool isSameList(const ListMarker& first, const ListMarker& other) {
    if (first.ordered != other.ordered) return false;
    return first.ordered ? first.delimiter == other.delimiter : first.bullet == other.bullet;
}

bool canInterruptParagraph(const ListMarker& marker, const QString& line) {
    return (!marker.ordered || marker.number == 1) && !isBlank(line.mid(marker.end));
}

std::optional<bool> matchTaskMarker(const QString& line) {
    if (line.size() < 4 || line.at(0) != u'[' || line.at(2) != u']') return std::nullopt;
    const QChar state = line.at(1);
    if (state != u' ' && state != u'x' && state != u'X') return std::nullopt;
    if (!isSpace(line.at(3))) return std::nullopt;
    return state != u' ';
}

HtmlBlockStart matchHtmlBlock(const QString& content) {
    if (!content.startsWith(u'<')) return {};
    if (content.startsWith(QStringLiteral("<!--"))) {
        return {HtmlBlock::Comment, QStringLiteral("-->")};
    }
    if (content.startsWith(QStringLiteral("<?"))) {
        return {HtmlBlock::ProcessingInstruction, QStringLiteral("?>")};
    }
    if (content.startsWith(QStringLiteral("<![CDATA["))) {
        return {HtmlBlock::Cdata, QStringLiteral("]]>")};
    }
    if (content.startsWith(QStringLiteral("<!"))) {
        const QChar first = content.size() > 2 ? content.at(2) : QChar();
        if (first.unicode() < 0x80 && first.isLetter()) {
            return {HtmlBlock::Declaration, QStringLiteral(">")};
        }
        return {};
    }
    const bool isClosing = content.startsWith(QStringLiteral("</"));
    const int nameStart = isClosing ? 2 : 1;
    if (!isClosing) {
        for (const char* name : {"script", "pre", "style", "textarea"}) {
            if (startsWithTag(content, nameStart, name)) {
                return {HtmlBlock::RawText,
                        QStringLiteral("</") + QLatin1String(name) + QStringLiteral(">")};
            }
        }
    }
    const int nameLength = tagNameLength(content, nameStart);
    if (nameLength == 0 || !isTagBoundary(content, nameStart + nameLength)) return {};
    if (isStructuralTag(content.mid(nameStart, nameLength).toLower())) {
        return {HtmlBlock::Structural, QString()};
    }
    // Any other tag, but only when it is alone on the line.
    const int tagEnd = htmlTagEnd(content, 0);
    if (tagEnd != -1 && isBlank(content.mid(tagEnd))) return {HtmlBlock::Other, QString()};
    return {};
}

// The index of the first line after the HTML block that starts at from.
int htmlBlockEnd(const QStringList& lines, int from, int to, const HtmlBlockStart& start) {
    if (start.endMarker.isEmpty()) {
        int end = from + 1;
        while (end < to && !isBlank(lines.at(end))) ++end;
        return end;
    }
    for (int line = from; line < to; ++line) {
        if (lines.at(line).contains(start.endMarker, Qt::CaseInsensitive)) return line + 1;
    }
    return to;
}

bool startsBlock(const QString& line) {
    const int indent = indentation(line);
    if (indent > kMaxMarkerIndent || indent >= line.size()) return false;
    const QString content = line.mid(indent);
    return matchFence(line) || matchHeading(content) || isThematicBreak(content) ||
           isQuoteLine(line) || matchListMarker(line).has_value() ||
           matchHtmlBlock(content).kind != HtmlBlock::None;
}

bool interruptsParagraph(const QString& line) {
    const int indent = indentation(line);
    if (indent > kMaxMarkerIndent || indent >= line.size()) return false;
    const QString content = line.mid(indent);
    if (matchFence(line) || matchHeading(content) || isThematicBreak(content) ||
        isQuoteLine(line)) {
        return true;
    }
    const HtmlBlockStart htmlBlock = matchHtmlBlock(content);
    // A tag that is alone on its line cannot interrupt a paragraph.
    if (htmlBlock.kind != HtmlBlock::None && htmlBlock.kind != HtmlBlock::Other) return true;
    const auto marker = matchListMarker(line);
    return marker && canInterruptParagraph(*marker, line);
}

// An indented code block ends before the first line that is indented by less
// than four columns. Blank lines inside the block are skipped: the index of
// the first line after the block is returned.
int indentedCodeEnd(const QStringList& lines, int from, int to) {
    int end = from;
    for (int index = from; index < to; ++index) {
        if (isBlank(lines.at(index))) continue;
        if (indentation(lines.at(index)) < kTabWidth) break;
        end = index + 1;
    }
    return end;
}

QStringList indentedCodeLines(const QStringList& lines, int from, int to) {
    QStringList code;
    for (int index = from; index < to; ++index) {
        code << stripCodeIndentation(lines.at(index), kTabWidth);
    }
    // Trailing blank lines are not a part of the code block.
    while (!code.isEmpty() && code.last().isEmpty()) code.removeLast();
    return code;
}

}  // namespace mdeditor::md
