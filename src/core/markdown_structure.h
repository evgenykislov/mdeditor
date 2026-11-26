#pragma once

#include <QStringList>

#include <vector>

#include "core/markdown_parser.h"

// The structure of a Markdown document as the converter understands it: which
// lines form a block and what is inside the containers. Internal header: it is
// not a part of the interface of the core library.
namespace mdeditor::md {

// How the lines of a text block are understood.
enum class TextBlockKind {
    Definitions,     // Link reference definitions only.
    Paragraph,
    SetextHeading,   // A paragraph followed by an underline.
    Table,
};

struct TextBlock {
    TextBlockKind kind = TextBlockKind::Paragraph;
    int end = 0;           // Index of the first line after the block.
    int headingLevel = 0;  // SetextHeading: the level of the heading.
};

struct ListItem {
    // The content of the item without the indentation that belongs to the item.
    QStringList lines;
    // A blank line separates the item from the previous one.
    bool blankLineBefore = false;
    // The item has blocks that are separated by a blank line.
    bool blankLineInside = false;
    int blankLinesAfter = 0;
};

struct Block {
    enum class Kind {
        IndentedCode,
        Fence,
        Heading,
        ThematicBreak,
        Quote,
        List,
        Html,
        Text,
    };

    Kind kind = Kind::Text;
    // Index of the first line after the block.
    int end = 0;
    TextBlock text;             // Text
    QStringList quoteLines;     // Quote: the content without the markers.
    std::vector<ListItem> items;  // List
};

// The block that starts at the line from. The line has to be non-blank and
// indented by less than four columns.
Block nextBlock(const QStringList& lines, int from);

// Whether the last block of the lines is a paragraph that is not closed. Only
// such a paragraph takes lazy continuation lines: lines without the marker of
// the block quote or without the indentation of the list item.
bool endsWithOpenParagraph(const QStringList& lines);

}  // namespace mdeditor::md
