#pragma once

#include <QHash>
#include <QString>
#include <QStringList>

#include <optional>
#include <vector>

#include "core/markdown_renderer.h"

// Internal declarations shared by the translation units of the Markdown
// converter. This header is not a part of the interface of the core library.
namespace mdeditor::md {

// Width of a tab stop, in columns.
constexpr int kTabWidth = 4;

// Block markers are recognized only when the line is indented by at most
// this many columns.
constexpr int kMaxMarkerIndent = 3;

struct LinkDefinition {
    QString destination;
    QString title;
};

// Link reference definitions indexed by normalized label.
using LinkDefinitions = QHash<QString, LinkDefinition>;

struct InlineContext {
    const LinkDefinitions* links = nullptr;
    // When set, all markup is stripped and only the plain text is produced.
    bool plainText = false;
};

// Lines: markdown_block.cpp.
bool isBlank(const QString& line);
int indentation(const QString& line);
// The line without the first columns of its indentation. The indentation that
// is left is returned as spaces, so that the tab stops of the line do not
// move when the text goes deeper into the structure of the document.
QString stripColumns(const QString& line, int columns);
// The same for the content of a code block, where the tabs are a part of the
// content and stay as they are.
QString stripCodeIndentation(const QString& line, int columns);
QString stripLeadingWhitespace(const QString& line);
QString stripTrailing(const QString& line);

// Text, entities, escapes and addresses: markdown_inline_scan.cpp.
// The characters that have a special meaning in HTML are replaced with
// character references. It serves for the text and for the values of the
// attributes alike.
QString escapeHtml(const QString& text);
// The address of a link in the form that an href attribute takes: the
// characters that cannot be there are percent-encoded.
QString encodeUrl(const QString& url);
// The text without the backslash escapes and with the character references resolved.
QString unescapeText(const QString& text);
// The address and the title of a link as they were written, made clean.
QString cleanUrl(const QString& url);
QString cleanTitle(const QString& title);
int tagNameLength(const QString& text, int position);
// Returns the index just after the raw HTML construct that starts with '<'
// at position (a tag, comment, processing instruction, declaration or CDATA
// section), or -1 when there is none.
int htmlTagEnd(const QString& text, int position);
// Returns the index of the backtick run that closes a code span which starts
// with a run of runLength backticks, or -1 when there is no closing run.
int codeSpanEnd(const QString& text, int from, int runLength);
// The text with markup as HTML, or as plain text when the context asks for it.
QString renderInline(const QString& text, const InlineContext& context);

// Link reference definitions: markdown_link.cpp.
// The number of lines at the beginning of lines[from..to) that link reference
// definitions take.
int leadingDefinitionLines(const QStringList& lines, int from, int to);
// Adds the link reference definitions that the lines begin with.
void addLeadingDefinitions(const QStringList& lines, int from, int to,
                           LinkDefinitions* definitions);
// The definitions of the whole document. They are looked for inside the
// containers too, but not inside the code blocks.
LinkDefinitions collectLinkDefinitions(const QStringList& lines);

// Block recognition: markdown_block.cpp.
struct Fence {
    QChar marker;
    int length = 0;
    int indent = 0;
    QString infoString;
};
std::optional<Fence> matchFence(const QString& line);
bool isClosingFence(const QString& line, const Fence& fence);

struct Heading {
    int level = 0;
    QString text;
};
std::optional<Heading> matchHeading(const QString& content);
// The level of a Setext heading underline, or 0 when the content is not one.
int setextLevel(const QString& content);
// Whether the line is a Setext heading underline.
bool isSetextUnderline(const QString& line);
bool isThematicBreak(const QString& content);
bool isQuoteLine(const QString& line);

struct ListMarker {
    bool ordered = false;
    QChar bullet;     // Unordered lists.
    QChar delimiter;  // Ordered lists: '.' or ')'.
    int number = 0;   // Ordered lists.
    int indent = 0;   // Columns before the marker.
    int end = 0;      // Index just after the marker.
    // Columns that are removed from the lines that belong to the item.
    int contentIndent = 0;
};
std::optional<ListMarker> matchListMarker(const QString& line);
bool isSameList(const ListMarker& first, const ListMarker& other);
// Whether a list item can interrupt a paragraph above it: an ordered list
// item can do it only when it is numbered with 1, and the first line of the
// item cannot be blank.
bool canInterruptParagraph(const ListMarker& marker, const QString& line);
// Whether the line starts a task list item: "[ ]" or "[x]" followed by
// whitespace. The result tells whether the task is done.
std::optional<bool> matchTaskMarker(const QString& line);

enum class HtmlBlock {
    None,
    Comment,
    ProcessingInstruction,
    Cdata,
    Declaration,
    RawText,     // <script>, <pre>, <style> or <textarea>: ends with the closing tag.
    Structural,  // A block level tag: ends with a blank line.
    Other,       // Any other tag alone on the line: ends with a blank line.
};
struct HtmlBlockStart {
    HtmlBlock kind = HtmlBlock::None;
    QString endMarker;  // Empty for blocks that end with a blank line.
};
HtmlBlockStart matchHtmlBlock(const QString& content);
int htmlBlockEnd(const QStringList& lines, int from, int to, const HtmlBlockStart& start);

// Whether the line starts a block of its own, so that it cannot be a lazy
// continuation line of a paragraph.
bool startsBlock(const QString& line);
// Whether the line ends the paragraph above it by starting a new block.
bool interruptsParagraph(const QString& line);
int indentedCodeEnd(const QStringList& lines, int from, int to);
QStringList indentedCodeLines(const QStringList& lines, int from, int to);

// Tables: markdown_table.cpp.
enum class CellAlignment { None, Left, Center, Right };
QStringList splitTableRow(const QString& line);
// A delimiter row of a table: cells of dashes with optional colons. The
// number of the cells has to be equal to columnCount.
std::optional<std::vector<CellAlignment>> matchTableDelimiterRow(const QString& line,
                                                                 int columnCount);

}  // namespace mdeditor::md
