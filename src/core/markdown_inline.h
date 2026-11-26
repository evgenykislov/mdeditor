#pragma once

#include <QString>

#include <memory>
#include <vector>

#include "core/markdown_parser.h"

// The inline level of the Markdown converter: the nodes that the inline
// content of a block is made of. Internal header: it is not a part of the
// interface of the core library.
namespace mdeditor::md {

struct InlineNode {
    enum class Type {
        Text,        // literal: the characters of the text. No markup is left in them.
        Code,        // literal: the code.
        SoftBreak,   // A line break inside a paragraph.
        HardBreak,   // A line break that also breaks the line in the output.
        RawHtml,     // literal: the HTML as it is written.
        Emphasis,    // level: 1 is <em> and 2 is <strong>.
        Strikethrough,
        Link,        // destination and title. The children are the text of the link.
        Image,       // destination and title. The children are the alternate text.
    };

    Type type = Type::Text;
    QString literal;
    int level = 0;
    QString destination;
    QString title;

    InlineNode* parent = nullptr;
    InlineNode* firstChild = nullptr;
    InlineNode* lastChild = nullptr;
    InlineNode* previous = nullptr;
    InlineNode* next = nullptr;
};

// A tree of inline nodes. The tree owns all of its nodes, wherever in the
// tree they are.
class InlineTree {
public:
    InlineTree();
    ~InlineTree();
    InlineTree(const InlineTree&) = delete;
    InlineTree& operator=(const InlineTree&) = delete;

    // The node that the content of the block belongs to.
    InlineNode* root() { return &root_; }
    const InlineNode* root() const { return &root_; }

    InlineNode* create(InlineNode::Type type, const QString& literal = QString());
    static void appendChild(InlineNode* parent, InlineNode* node);
    static void insertAfter(InlineNode* position, InlineNode* node);
    // Detaches the node from its parent. The tree keeps owning it.
    static void unlink(InlineNode* node);
    // Moves the nodes that follow first in its list up to last (not included)
    // to the children of the container. All the nodes that follow first are
    // moved when last is null.
    static void moveNodesBetween(InlineNode* first, InlineNode* last, InlineNode* container);

private:
    InlineNode root_;
    std::vector<std::unique_ptr<InlineNode>> nodes_;
};

// Scanners and helpers: markdown_inline_scan.cpp.
bool decodeEntity(const QString& text, int position, QString* decoded, int* end);
QString escapeHtml(const QString& text);
QString encodeUrl(const QString& url);
QString unescapeText(const QString& text);
// The length of the name of an HTML tag that starts at position, or 0.
int tagNameLength(const QString& text, int position);
// The length of the raw HTML construct that starts with '<' at position
// (a tag, comment, processing instruction, declaration or CDATA section),
// or 0 when there is none.
int htmlLength(const QString& text, int position);
// The length of the autolink that starts with '<' at position, or 0. The
// address gets the "mailto:" prefix when the autolink is an email address.
int autolinkLength(const QString& text, int position, bool* isEmail);
// The length of the run of delimiter characters that starts at position, and
// whether the run can open and close an emphasis.
int delimiterRunLength(const QString& text, int position, bool* canOpen, bool* canClose);
// The destination of a link that starts at position: in angle brackets or a
// sequence of characters without whitespace. Returns its length, or -1 when
// there is no destination.
int scanLinkDestination(const QString& text, int position, QString* destination);
// The length of the title of a link that starts at position: text in double
// quotes, single quotes or parentheses. Returns 0 when there is no title.
int scanLinkTitle(const QString& text, int position);
// The length of the link label "[label]" that starts at position, or 0. The
// label is returned without the brackets and without the whitespace around it.
int scanLinkLabel(const QString& text, int position, QString* label);
// The key that link labels are compared by: without whitespace around them,
// with runs of whitespace collapsed to one space, and without case.
QString normalizeLabel(const QString& label);

// The parser: markdown_inline_parse.cpp and the files next to it.
void parseInlines(const QString& text, const InlineContext& context, InlineTree* tree);
// The tree as HTML.
QString renderInlineHtml(const InlineTree& tree);
// The text of the node and of all that is inside it, without any markup.
QString renderInlinePlainText(const InlineNode* node);

}  // namespace mdeditor::md
