#include "core/markdown_inline.h"

#include <QChar>

#include "core/markdown_inline_parser.h"
#include "core/markdown_structure.h"

namespace mdeditor::md {
namespace {

// The text of the lines[from..to) as one text: the lines are joined with
// line breaks, and the whitespace at the beginning of a line is not a part
// of the text.
QString paragraphText(const QStringList& lines, int from, int to) {
    QString text;
    for (int index = from; index < to; ++index) {
        text += stripLeadingWhitespace(lines.at(index));
        text += QChar(u'\n');
    }
    return text;
}

int skipSpaces(const QString& text, int position) {
    while (position < text.size() &&
           (text.at(position) == u' ' || text.at(position) == u'\t')) {
        ++position;
    }
    return position;
}

// Skips the spaces and at most one line break.
int skipSpacesAndLineBreak(const QString& text, int position) {
    position = skipSpaces(text, position);
    if (position < text.size() && text.at(position) == u'\n') {
        position = skipSpaces(text, position + 1);
    }
    return position;
}

// Whether only the end of the line or the end of the text follows position.
bool isLineEnd(const QString& text, int position) {
    position = skipSpaces(text, position);
    return position == text.size() || text.at(position) == u'\n';
}

// Parses the link reference definition that starts at position:
// "[label]: destination \"title\"". The label and the destination with the
// title are returned without escapes and character references. Returns the
// position after the definition, which is the beginning of the next line, or
// position when there is no definition there.
int parseReference(const QString& text, int position, QString* label,
                   LinkDefinition* definition) {
    int current = position;
    const int labelLength = scanLinkLabel(text, current, label);
    if (labelLength == 0 || label->isEmpty()) return position;
    current += labelLength;

    // The colon after the label.
    if (current >= text.size() || text.at(current) != u':') return position;
    ++current;

    // The destination: it may be on the next line.
    current = skipSpacesAndLineBreak(text, current);
    QString destination;
    const int destinationLength = scanLinkDestination(text, current, &destination);
    if (destinationLength < 0) return position;
    current += destinationLength;

    // The title is optional: it has to be separated from the destination by
    // whitespace, and it may be on the next line.
    const int beforeTitle = current;
    current = skipSpacesAndLineBreak(text, current);
    int titleLength = 0;
    if (current != beforeTitle) {
        titleLength = scanLinkTitle(text, current);
        // What follows the destination is not a title: the definition has none.
        current = titleLength > 0 ? current + titleLength : beforeTitle;
    }
    if (!isLineEnd(text, current)) {
        // Something else follows the title. The definition is still valid
        // when the destination alone goes with the end of the line.
        if (titleLength == 0) return position;
        current = beforeTitle;
        if (!isLineEnd(text, current)) return position;
        titleLength = 0;
    }

    definition->destination = cleanUrl(destination);
    definition->title = titleLength > 0 ? cleanTitle(text.mid(current - titleLength, titleLength))
                                        : QString();
    // The definition takes the rest of the line.
    current = skipSpaces(text, current);
    if (current < text.size() && text.at(current) == u'\n') ++current;
    return current;
}

}  // namespace

int leadingDefinitionLines(const QStringList& lines, int from, int to) {
    const QString text = paragraphText(lines, from, to);
    int position = 0;
    while (position < text.size() && text.at(position) == u'[') {
        QString label;
        LinkDefinition definition;
        const int next = parseReference(text, position, &label, &definition);
        if (next == position) break;
        position = next;
    }
    // Every definition ends with the line break after it.
    return text.left(position).count(u'\n');
}

void addLeadingDefinitions(const QStringList& lines, int from, int to,
                           LinkDefinitions* definitions) {
    const QString text = paragraphText(lines, from, to);
    int position = 0;
    while (position < text.size() && text.at(position) == u'[') {
        QString label;
        LinkDefinition definition;
        const int next = parseReference(text, position, &label, &definition);
        if (next == position) break;
        // The first definition of a label is the one that counts.
        const QString key = normalizeLabel(label);
        if (!definitions->contains(key)) definitions->insert(key, definition);
        position = next;
    }
}

namespace {

// The definitions are collected from the whole document, the containers
// included, but not from the blocks where the text is not Markdown: the code
// blocks.
void collectDefinitions(const QStringList& lines, LinkDefinitions* definitions) {
    int index = 0;
    while (index < lines.size()) {
        if (isBlank(lines.at(index))) {
            ++index;
            continue;
        }
        const Block block = nextBlock(lines, index);
        switch (block.kind) {
        case Block::Kind::Text:
            if (block.text.kind == TextBlockKind::Definitions) {
                addLeadingDefinitions(lines, index, block.end, definitions);
            }
            break;
        case Block::Kind::Quote:
            collectDefinitions(block.quoteLines, definitions);
            break;
        case Block::Kind::List:
            for (const ListItem& item : block.items) collectDefinitions(item.lines, definitions);
            break;
        default:
            break;
        }
        index = block.end;
    }
}

}  // namespace

LinkDefinitions collectLinkDefinitions(const QStringList& lines) {
    LinkDefinitions definitions;
    collectDefinitions(lines, &definitions);
    return definitions;
}

}  // namespace mdeditor::md
