#include "core/markdown_inline_parser.h"

namespace mdeditor::md {
namespace {

// The maximum length of a link label.
constexpr int kMaxLabelLength = 999;

// Whether "mailto:" or "xmpp:" stands before the address that ends at position. The rewind characters before it are a part of the address.
bool hasProtocol(const char* protocol, const QString& text, int position, int rewind,
                 int maxRewind) {
    const int length = QLatin1String(protocol).size();
    if (length > maxRewind - rewind) return false;
    if (text.mid(position - rewind - length, length) != QLatin1String(protocol)) return false;
    // The protocol stands at the beginning of the text, or a character that is not a letter or a digit is before it.
    return length == maxRewind - rewind ||
           !isAsciiAlphanumeric(text.at(position - rewind - length - 1));
}

}  // namespace

// Handles the ']' that may close a link or an image. When the text between
// the brackets is not a link, the bracket stays as text.
void InlineParser::handleCloseBracket() {
    // Consume the bracket.
    ++position_;
    const int initialPosition = position_;
    if (brackets_.empty()) {
        append(InlineNode::Type::Text, QStringLiteral("]"));
        return;
    }
    const Bracket opener = brackets_.back();
    const bool isImage = opener.image;
    // A link cannot contain another link.
    if (!isImage && noLinkOpeners_) {
        brackets_.pop_back();
        append(InlineNode::Type::Text, QStringLiteral("]"));
        return;
    }
    const int afterTextPosition = position_;

    QString destination;
    QString title;
    bool isLink = false;

    // An inline link: its destination and its optional title are in parentheses.
    if (peek() == u'(') {
        int current = position_ + 1;
        while (current < text_.size() && isSpace(text_.at(current))) ++current;
        QString rawDestination;
        const int length = scanLinkDestination(text_, current, &rawDestination);
        if (length > -1) {
            const int destinationEnd = current + length;
            int titleStart = destinationEnd;
            while (titleStart < text_.size() && isSpace(text_.at(titleStart))) ++titleStart;
            // The title has to be separated from the destination by whitespace.
            int titleEnd = titleStart;
            if (titleStart != destinationEnd) titleEnd += scanLinkTitle(text_, titleStart);
            int end = titleEnd;
            while (end < text_.size() && isSpace(text_.at(end))) ++end;
            if (end < text_.size() && text_.at(end) == u')') {
                position_ = end + 1;
                destination = cleanUrl(rawDestination);
                title = cleanTitle(text_.mid(titleStart, titleEnd - titleStart));
                isLink = true;
            }
        }
    }

    // A reference link: its label is in brackets after the text of the link,
    // or the text of the link is the label.
    if (!isLink) {
        position_ = afterTextPosition;
        QString label;
        bool hasLabel = false;
        const int labelLength = scanLinkLabel(text_, position_, &label);
        if (labelLength > 0) {
            position_ += labelLength;
            hasLabel = true;
        } else {
            position_ = initialPosition;
        }
        // The text of the link is the label of a shortcut reference. It is
        // not looked up when the text has another bracket in it.
        if ((!hasLabel || label.isEmpty()) && !opener.bracketAfter) {
            label = text_.mid(opener.position, initialPosition - opener.position - 1);
            hasLabel = label.size() <= kMaxLabelLength;
        }
        if (hasLabel && links_ != nullptr && label.size() <= kMaxLabelLength) {
            const auto found = links_->constFind(normalizeLabel(label));
            if (found != links_->constEnd()) {
                destination = found->destination;
                title = found->title;
                isLink = true;
            }
        }
    }

    if (!isLink) {
        // The bracket stays as text, but it cannot open a link anymore.
        brackets_.pop_back();
        position_ = initialPosition;
        append(InlineNode::Type::Text, QStringLiteral("]"));
        return;
    }

    InlineNode* link = appendLink(destination, isImage ? InlineNode::Type::Image
                                                      : InlineNode::Type::Link);
    link->title = title;
    // The text of the link is what follows the bracket. The link itself is
    // at the end of the list until it takes its place after the bracket.
    InlineTree::moveNodesBetween(opener.node, link, link);
    InlineTree::insertAfter(opener.node, link);
    InlineTree::unlink(opener.node);

    // The emphasis inside the text of the link is paired here: it cannot
    // spread outside of the link.
    processEmphasis(opener.position);
    brackets_.pop_back();
    if (!isImage) noLinkOpeners_ = true;
}

// The length of the valid domain that starts at start, or 0 when there is
// none. The domain has to have a dot, unless allowShort is set. An underscore
// is not allowed in one of its last two segments.
int InlineParser::domainLength(int start, bool allowShort) const {
    const int size = text_.size() - start;
    int dots = 0;
    int underscoresBeforeDot = 0;
    int underscoresAfterDot = 0;
    int i = 1;
    for (; i < size - 1; ++i) {
        int index = start + i;
        // An escaped character is a part of the domain.
        if (text_.at(index) == u'\\' && i < size - 2) ++index;
        const QChar c = text_.at(index);
        if (c == u'_') {
            ++underscoresAfterDot;
        } else if (c == u'.') {
            underscoresBeforeDot = underscoresAfterDot;
            underscoresAfterDot = 0;
            ++dots;
        } else if (!isHostCharacter(c) && c != u'-') {
            break;
        }
    }
    if ((underscoresBeforeDot > 0 || underscoresAfterDot > 0) && dots <= 10) return 0;
    if (!allowShort && dots == 0) return 0;
    return i;
}

// The end of a bare address that starts at start and goes on up to end: the
// punctuation at the end of the address is not a part of it.
int InlineParser::addressEnd(int start, int end) const {
    int opening = 0;
    int closing = 0;
    for (int i = start; i < end; ++i) {
        const QChar c = text_.at(i);
        if (c == u'<') {
            end = i;
            break;
        }
        if (c == u'(') {
            ++opening;
        } else if (c == u')') {
            ++closing;
        }
    }
    const QString punctuation = QStringLiteral("?!.,:*_~'\"");
    while (end > start) {
        const QChar c = text_.at(end - 1);
        if (c == u')') {
            // A closing bracket that has no opening one is not a part of the address.
            if (closing <= opening) break;
            --closing;
            --end;
        } else if (punctuation.contains(c)) {
            --end;
        } else if (c == u';') {
            // A character reference, such as "&copy;", is not a part of it either.
            int entity = end - 2;
            while (entity > start && isAsciiLetter(text_.at(entity))) --entity;
            if (entity < end - 2 && text_.at(entity) == u'&') {
                end = entity;
            } else {
                --end;
            }
        } else {
            break;
        }
    }
    return end;
}

// An address that starts with "www.": the link gets the "http://" scheme.
bool InlineParser::matchWwwAutolink() {
    // A bare address is not looked for inside the text of a link.
    if (!brackets_.empty()) return false;
    const int start = position_;
    // The address has to be separated from the text before it. Only the
    // characters that can end markup may stand right before it.
    if (start > 0) {
        const QChar before = text_.at(start - 1);
        if (!QStringLiteral("*_~(").contains(before) && !isSpace(before)) return false;
    }
    if (text_.mid(start, 4) != QLatin1String("www.")) return false;
    const int domain = domainLength(start, false);
    if (domain == 0) return false;
    int end = start + domain;
    while (end < text_.size() && !isSpace(text_.at(end)) && text_.at(end) != u'<') ++end;
    end = addressEnd(start, end);
    if (end == start) return false;

    const QString address = text_.mid(start, end - start);
    InlineNode* link = appendLink(QStringLiteral("http://") + address);
    InlineTree::appendChild(link, append(InlineNode::Type::Text, address));
    position_ = end;
    return true;
}

// An address with the scheme http, https or ftp.
bool InlineParser::matchUrlAutolink() {
    if (!brackets_.empty()) return false;
    const int colon = position_;
    if (text_.mid(colon + 1, 2) != QLatin1String("//")) return false;
    // The scheme has been written as text already: it is taken back from there.
    int rewind = 0;
    while (rewind < colon && isAsciiLetter(text_.at(colon - rewind - 1))) ++rewind;
    const QString scheme = text_.mid(colon - rewind, rewind + 1).toLower();
    if (scheme != QLatin1String("http:") && scheme != QLatin1String("https:") &&
        scheme != QLatin1String("ftp:")) {
        return false;
    }
    const int hostStart = colon + 3;
    if (hostStart >= text_.size() || !isHostCharacter(text_.at(hostStart))) return false;
    const int domain = domainLength(hostStart, true);
    if (domain == 0) return false;
    const int start = colon - rewind;
    int end = hostStart + domain;
    while (end < text_.size() && !isSpace(text_.at(end)) && text_.at(end) != u'<') ++end;
    end = addressEnd(start, end);
    if (end == start) return false;

    InlineNode* last = root_->lastChild;
    if (rewind > 0 && last != nullptr && last->type == InlineNode::Type::Text) {
        last->literal.chop(rewind);
        if (last->literal.isEmpty()) InlineTree::unlink(last);
    }
    const QString address = text_.mid(start, end - start);
    InlineNode* link = appendLink(address);
    InlineTree::appendChild(link, append(InlineNode::Type::Text, address));
    position_ = end;
    return true;
}

// Email addresses in the text: they are not looked for inside links, where
// the text is a part of the link already.
void InlineParser::linkEmailAddresses() { linkEmailAddressesIn(root_); }

void InlineParser::linkEmailAddressesIn(InlineNode* parent) {
    // The text that the markup characters split is joined back: an address
    // may have such characters in it.
    for (InlineNode* node = parent->firstChild; node != nullptr; node = node->next) {
        if (node->type != InlineNode::Type::Text) continue;
        while (node->next != nullptr && node->next->type == InlineNode::Type::Text) {
            node->literal += node->next->literal;
            InlineTree::unlink(node->next);
        }
    }

    InlineNode* node = parent->firstChild;
    while (node != nullptr) {
        InlineNode* next = node->next;
        if (node->type == InlineNode::Type::Text) {
            linkEmailAddressInText(node);
            next = node->next;
        }
        node = next;
    }
}

// Finds the email addresses in the text of the node. The node is split: it
// keeps the text before the first address, and node is replaced by the node
// that holds the text after it.
void InlineParser::linkEmailAddressInText(InlineNode*& node) {
    const QString data = node->literal;
    // The search goes on in the part of the text that starts at start. The
    // '@' is looked for from offset inside it.
    int start = 0;
    int offset = 0;
    int remaining = data.size();
    while (true) {
        if (offset >= remaining) break;
        const int foundAt = data.indexOf(u'@', start + offset);
        if (foundAt == -1) break;
        int maxRewind = foundAt - (start + offset);
        int dots = 0;
        bool addMailto = true;
        bool isXmpp = false;

        // The part before the '@' is scanned again when another '@' is
        // found after it.
        bool restart = true;
        while (restart) {
            restart = false;

            // How many characters before the '@' are a part of the address.
            int rewind = 0;
            for (rewind = 0; rewind < maxRewind; ++rewind) {
                const QChar c = data.at(start + offset + maxRewind - rewind - 1);
                if (isAsciiAlphanumeric(c) || QStringLiteral(".+-_").contains(c)) continue;
                if (c == u':') {
                    const int at = start + offset + maxRewind;
                    if (hasProtocol("mailto:", data, at, rewind, maxRewind)) {
                        addMailto = false;
                        continue;
                    }
                    if (hasProtocol("xmpp:", data, at, rewind, maxRewind)) {
                        addMailto = false;
                        isXmpp = true;
                        continue;
                    }
                }
                break;
            }
            // Nothing before the '@' can be a part of an address.
            if (rewind == 0) {
                offset += maxRewind + 1;
                break;
            }

            // The address goes on after the '@' up to a character that it cannot have.
            const int at = start + offset + maxRewind;
            int length = 1;
            for (; length < remaining - offset - maxRewind; ++length) {
                const QChar c = data.at(at + length);
                if (isAsciiAlphanumeric(c)) continue;
                if (c == u'@') {
                    // The address is looked for again after this '@'.
                    offset += maxRewind + 1;
                    maxRewind = length - 1;
                    restart = true;
                    break;
                }
                if (c == u'.' && length < remaining - offset - maxRewind - 1 &&
                    isAsciiAlphanumeric(data.at(at + length + 1))) {
                    ++dots;
                } else if (c == '/' && isXmpp) {
                    continue;
                } else if (c != u'-' && c != u'_') {
                    break;
                }
            }
            if (restart) continue;

            // The domain of the address has a dot and does not end with a
            // hyphen or with an underscore.
            const QChar last = data.at(at + length - 1);
            if (length < 2 || dots == 0 || (!isAsciiLetter(last) && last != u'.')) {
                offset += maxRewind + length;
                break;
            }
            const int linkLength = addressEnd(at, at + length) - at;
            if (linkLength == 0) {
                offset += maxRewind + 1;
                break;
            }

            const QString address = data.mid(at - rewind, linkLength + rewind);
            InlineNode* link =
                appendLink(addMailto ? QStringLiteral("mailto:") + address : address);
            InlineTree::appendChild(link, append(InlineNode::Type::Text, address));
            InlineTree::insertAfter(node, link);
            InlineNode* rest = append(InlineNode::Type::Text, data.mid(at + linkLength));
            InlineTree::insertAfter(link, rest);

            node->literal = data.left(at - rewind);
            node = rest;
            start += offset + maxRewind + linkLength;
            remaining -= offset + maxRewind + linkLength;
            offset = 0;
        }
    }
}

}  // namespace mdeditor::md
