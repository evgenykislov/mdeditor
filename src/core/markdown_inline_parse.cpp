#include "core/markdown_inline_parser.h"

#include "core/markdown_parser.h"

namespace mdeditor::md {

// ---- The tree of nodes ----

InlineTree::InlineTree() = default;
InlineTree::~InlineTree() = default;

InlineNode* InlineTree::create(InlineNode::Type type, const QString& literal) {
    auto node = std::make_unique<InlineNode>();
    node->type = type;
    node->literal = literal;
    InlineNode* pointer = node.get();
    nodes_.push_back(std::move(node));
    return pointer;
}

void InlineTree::appendChild(InlineNode* parent, InlineNode* node) {
    // A node that is in a list already leaves it first.
    unlink(node);
    node->parent = parent;
    node->previous = parent->lastChild;
    node->next = nullptr;
    if (parent->lastChild != nullptr) {
        parent->lastChild->next = node;
    } else {
        parent->firstChild = node;
    }
    parent->lastChild = node;
}

void InlineTree::insertAfter(InlineNode* position, InlineNode* node) {
    // A node that is in a list already leaves it first.
    unlink(node);
    node->parent = position->parent;
    node->previous = position;
    node->next = position->next;
    if (position->next != nullptr) {
        position->next->previous = node;
    } else {
        position->parent->lastChild = node;
    }
    position->next = node;
}

void InlineTree::unlink(InlineNode* node) {
    if (node->parent == nullptr) return;
    if (node->previous != nullptr) {
        node->previous->next = node->next;
    } else {
        node->parent->firstChild = node->next;
    }
    if (node->next != nullptr) {
        node->next->previous = node->previous;
    } else {
        node->parent->lastChild = node->previous;
    }
    node->parent = nullptr;
    node->previous = nullptr;
    node->next = nullptr;
}

void InlineTree::moveNodesBetween(InlineNode* first, InlineNode* last, InlineNode* container) {
    InlineNode* node = first->next;
    while (node != nullptr && node != last) {
        InlineNode* next = node->next;
        unlink(node);
        appendChild(container, node);
        node = next;
    }
}

namespace {

// Whether the character makes the parser stop a run of plain text. The colon
// is where the autolinks with a scheme are recognized, and 'w' is where the
// autolinks that start with "www." are.
bool isSpecial(QChar character) {
    return character == u'\n' || character == u'\r' ||
           QStringLiteral("\\`&_*[]<!~:w").contains(character);
}

}  // namespace

bool isSpace(QChar character) {
    switch (character.unicode()) {
    case u' ':
    case u'\t':
    case u'\n':
    case u'\v':
    case u'\f':
    case u'\r':
        return true;
    default:
        return false;
    }
}

bool isAsciiAlphanumeric(QChar character) {
    const uint code = character.unicode();
    return (code >= u'a' && code <= u'z') || (code >= u'A' && code <= u'Z') ||
           (code >= u'0' && code <= u'9');
}

bool isAsciiLetter(QChar character) {
    const uint code = character.unicode();
    return (code >= u'a' && code <= u'z') || (code >= u'A' && code <= u'Z');
}

bool isHostCharacter(QChar character) {
    if (character.unicode() >= 0x80) return false;
    return !isSpace(character) &&
           !QStringLiteral("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~").contains(character);
}

QString rtrimmed(QString text) {
    while (!text.isEmpty() && isSpace(text.at(text.size() - 1))) text.chop(1);
    return text;
}

InlineNode* InlineParser::appendLink(const QString& destination, InlineNode::Type type) {
    InlineNode* link = append(type);
    link->destination = destination;
    return link;
}

void InlineParser::parse() {
    while (position_ < text_.size()) parseOne();
    processEmphasis(0);
    linkEmailAddresses();
}

void InlineParser::parseOne() {
    const QChar c = peek();
    switch (c.unicode()) {
    case u'\n':
        handleNewline();
        break;
    case u'`':
        handleBackticks();
        break;
    case u'\\':
        handleBackslash();
        break;
    case u'&':
        handleEntity();
        break;
    case u'<':
        handlePointyBrace();
        break;
    case u'*':
    case u'_':
    case u'~':
        handleDelimiter(c);
        break;
    case u'[':
        ++position_;
        if (!brackets_.empty()) brackets_.back().bracketAfter = true;
        brackets_.push_back({append(InlineNode::Type::Text, QStringLiteral("[")), false, false,
                             position_});
        noLinkOpeners_ = false;
        break;
    case u']':
        handleCloseBracket();
        break;
    case u'!':
        ++position_;
        if (peek() == u'[') {
            ++position_;
            if (!brackets_.empty()) brackets_.back().bracketAfter = true;
            brackets_.push_back(
                {append(InlineNode::Type::Text, QStringLiteral("![")), true, false, position_});
            noLinkOpeners_ = false;
        } else {
            append(InlineNode::Type::Text, QStringLiteral("!"));
        }
        break;
    case u'w':
        if (matchWwwAutolink()) break;
        Q_FALLTHROUGH();
    case u':':
        if (c == u':' && matchUrlAutolink()) break;
        Q_FALLTHROUGH();
    default: {
        // A run of plain text up to the next character that has a meaning.
        int end = position_ + 1;
        while (end < text_.size() && !isSpecial(text_.at(end))) ++end;
        QString literal = text_.mid(position_, end - position_);
        // The whitespace at the end of a line is not a part of the text.
        if (end == text_.size() || text_.at(end) == u'\n') literal = rtrimmed(literal);
        append(InlineNode::Type::Text, literal);
        position_ = end;
        break;
    }
    }
}

void InlineParser::handleNewline() {
    // Two spaces at the end of the line make a hard line break.
    const bool isHard = position_ >= 2 && text_.at(position_ - 1) == u' ' &&
                        text_.at(position_ - 2) == u' ';
    ++position_;
    // The whitespace at the beginning of the next line is not a part of the text.
    while (position_ < text_.size() &&
           (text_.at(position_) == u' ' || text_.at(position_) == u'\t')) {
        ++position_;
    }
    append(isHard ? InlineNode::Type::HardBreak : InlineNode::Type::SoftBreak);
}

void InlineParser::handleBackticks() {
    const int size = text_.size();
    int runEnd = position_;
    while (runEnd < size && text_.at(runEnd) == u'`') ++runEnd;
    const int runLength = runEnd - position_;
    const int closing = codeSpanEnd(text_, runEnd, runLength);
    // Without a closing run the backticks are plain text.
    if (closing == -1) {
        append(InlineNode::Type::Text, text_.mid(position_, runLength));
        position_ = runEnd;
        return;
    }
    QString code = text_.mid(runEnd, closing - runEnd);
    code.replace(u'\n', u' ');
    // A space is removed from both ends when the code has them and is not
    // made of spaces only.
    if (code.size() >= 2 && code.startsWith(u' ') && code.endsWith(u' ') &&
        !code.trimmed().isEmpty()) {
        code = code.mid(1, code.size() - 2);
    }
    append(InlineNode::Type::Code, code);
    position_ = closing + runLength;
}

void InlineParser::handleBackslash() {
    const int size = text_.size();
    if (position_ + 1 < size) {
        const QChar next = text_.at(position_ + 1);
        // A backslash at the end of a line is a hard line break.
        if (next == u'\n') {
            position_ += 2;
            while (position_ < size &&
                   (text_.at(position_) == u' ' || text_.at(position_) == u'\t')) {
                ++position_;
            }
            append(InlineNode::Type::HardBreak);
            return;
        }
        // A backslash escapes an ASCII punctuation character.
        const uint code = next.unicode();
        const bool isPunctuation = (code >= 0x21 && code <= 0x2F) || (code >= 0x3A && code <= 0x40) ||
                                   (code >= 0x5B && code <= 0x60) || (code >= 0x7B && code <= 0x7E);
        if (isPunctuation) {
            append(InlineNode::Type::Text, QString(next));
            position_ += 2;
            return;
        }
    }
    append(InlineNode::Type::Text, QStringLiteral("\\"));
    ++position_;
}

void InlineParser::handleEntity() {
    QString decoded;
    int end = 0;
    // A character reference that is not complete or is not known is plain text.
    if (decodeEntity(text_, position_, &decoded, &end)) {
        append(InlineNode::Type::Text, decoded);
        position_ = end;
        return;
    }
    append(InlineNode::Type::Text, QStringLiteral("&"));
    ++position_;
}

void InlineParser::handlePointyBrace() {
    const int start = position_;
    ++position_;
    bool isEmail = false;
    const int autolink = autolinkLength(text_, start, &isEmail);
    // An autolink in angle brackets: the address is both the destination and the text of the link.
    if (autolink > 0) {
        // The angle brackets are not a part of the address: the position is already after the opening one.
        const QString address = text_.mid(position_, autolink - 2);
        position_ = start + autolink;
        InlineNode* link = appendLink(isEmail ? QStringLiteral("mailto:") + address : address);
        InlineTree::appendChild(link, append(InlineNode::Type::Text, address));
        return;
    }
    const int html = htmlLength(text_, start);
    if (html > 0) {
        append(InlineNode::Type::RawHtml, text_.mid(start, html));
        position_ = start + html;
        return;
    }
    append(InlineNode::Type::Text, QStringLiteral("<"));
}

void InlineParser::handleDelimiter(QChar character) {
    bool canOpen = false;
    bool canClose = false;
    const int length = delimiterRunLength(text_, position_, &canOpen, &canClose);
    InlineNode* node = append(InlineNode::Type::Text, text_.mid(position_, length));
    position_ += length;
    // A strikethrough is made by runs of exactly two tildes.
    const bool isDelimiter = character != u'~' || length == 2;
    if (isDelimiter && (canOpen || canClose)) {
        pushDelimiter(node, character, length, canOpen, canClose);
    }
}

Delimiter* InlineParser::pushDelimiter(InlineNode* node, QChar character, int length, bool canOpen,
                                       bool canClose) {
    auto delimiter = std::make_unique<Delimiter>();
    delimiter->node = node;
    delimiter->character = character;
    delimiter->length = length;
    delimiter->canOpen = canOpen;
    delimiter->canClose = canClose;
    delimiter->position = position_;
    delimiter->previous = lastDelimiter_;
    Delimiter* pointer = delimiter.get();
    if (lastDelimiter_ != nullptr) lastDelimiter_->next = pointer;
    lastDelimiter_ = pointer;
    delimiters_.push_back(std::move(delimiter));
    return pointer;
}

void InlineParser::removeDelimiter(Delimiter* delimiter) {
    if (delimiter->previous != nullptr) delimiter->previous->next = delimiter->next;
    if (delimiter->next != nullptr) delimiter->next->previous = delimiter->previous;
    if (lastDelimiter_ == delimiter) lastDelimiter_ = delimiter->previous;
    delimiter->previous = nullptr;
    delimiter->next = nullptr;
}

}  // namespace mdeditor::md
