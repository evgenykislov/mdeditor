#pragma once

#include <QChar>
#include <QString>

#include <memory>
#include <vector>

#include "core/markdown_inline.h"

// The parser of the inline content of a block. Internal header: it is not a
// part of the interface of the core library.
namespace mdeditor::md {

// Whitespace as the scanners of links and HTML tags know it: a space, a
// tabulation, a line break or a page break.
bool isSpace(QChar character);
bool isAsciiAlphanumeric(QChar character);
bool isAsciiLetter(QChar character);
// A character that the host of a bare address can be made of: ASCII, and
// neither whitespace nor punctuation.
bool isHostCharacter(QChar character);
QString rtrimmed(QString text);

// A run of delimiter characters that can open or close an emphasis.
struct Delimiter {
    InlineNode* node = nullptr;
    QChar character;
    // The length of the run as it was found. It does not change: the rules
    // of pairing depend on it, not on the part that is left.
    int length = 0;
    bool canOpen = false;
    bool canClose = false;
    // Where the run ends in the text. The runs inside the text of a link
    // are paired separately from the runs outside of it.
    int position = 0;
    Delimiter* previous = nullptr;
    Delimiter* next = nullptr;
};

// An opening bracket of a link or of an image.
struct Bracket {
    InlineNode* node = nullptr;
    bool image = false;
    // Another opening bracket follows inside the text of this link.
    bool bracketAfter = false;
    // Where the text of the link starts.
    int position = 0;
};

class InlineParser {
public:
    InlineParser(const QString& text, const LinkDefinitions* links, InlineTree* tree)
        : text_(text), links_(links), tree_(tree), root_(tree->root()) {}

    void parse();

private:
    QChar peek() const { return position_ < text_.size() ? text_.at(position_) : QChar(); }

    // Adds a node with the content of the block.
    InlineNode* append(InlineNode::Type type, const QString& literal = QString()) {
        InlineNode* node = tree_->create(type, literal);
        InlineTree::appendChild(root_, node);
        return node;
    }

    // Adds a link to the content of the block. Its text is the node after it.
    InlineNode* appendLink(const QString& destination, InlineNode::Type type = InlineNode::Type::Link);

    void parseOne();
    void handleNewline();
    void handleBackticks();
    void handleBackslash();
    void handleEntity();
    void handlePointyBrace();
    void handleDelimiter(QChar character);

    // Handles the ']' of a link or of an image: markdown_inline_links.cpp.
    void handleCloseBracket();
    // The autolinks without angle brackets (an extension of GitHub Flavored Markdown).
    bool matchWwwAutolink();
    bool matchUrlAutolink();
    void linkEmailAddresses();
    void linkEmailAddressesIn(InlineNode* parent);
    void linkEmailAddressInText(InlineNode*& node);
    // The length of the valid domain that starts at start, or 0.
    int domainLength(int start, bool allowShort) const;
    // The end of a bare address: the punctuation at the end is not a part of it.
    int addressEnd(int start, int end) const;

    // Pairs the delimiter runs: markdown_inline_emphasis.cpp.
    void processEmphasis(int stackBottom);
    Delimiter* insertEmphasis(Delimiter* opener, Delimiter* closer);
    Delimiter* insertStrikethrough(Delimiter* opener, Delimiter* closer);
    Delimiter* pushDelimiter(InlineNode* node, QChar character, int length, bool canOpen,
                             bool canClose);
    void removeDelimiter(Delimiter* delimiter);

    const QString& text_;
    const LinkDefinitions* links_;
    InlineTree* tree_;
    InlineNode* root_;
    int position_ = 0;
    // The parser owns the delimiters. A delimiter that is removed from the
    // list stays allocated until the parser is gone.
    std::vector<std::unique_ptr<Delimiter>> delimiters_;
    Delimiter* lastDelimiter_ = nullptr;
    std::vector<Bracket> brackets_;
    // A link was made, so no other link can be made until a new bracket opens.
    bool noLinkOpeners_ = false;
};

}  // namespace mdeditor::md
