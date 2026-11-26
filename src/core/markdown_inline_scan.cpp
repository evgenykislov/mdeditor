#include "core/markdown_inline.h"

#include <QChar>

namespace mdeditor::md {
namespace {

// The HTML 5 named character references. The definitions are generated.
#include "core/html_entities.inc"

constexpr int kHtmlEntityCount = static_cast<int>(sizeof(kHtmlEntities) / sizeof(kHtmlEntities[0]));

bool isInRange(uint value, char first, char last) {
    return value >= static_cast<uint>(first) && value <= static_cast<uint>(last);
}

bool isAsciiLetter(uint code) { return isInRange(code, 'a', 'z') || isInRange(code, 'A', 'Z'); }

bool isAsciiDigit(uint code) { return isInRange(code, '0', '9'); }

bool isAsciiAlphanumeric(uint code) { return isAsciiLetter(code) || isAsciiDigit(code); }

bool isAsciiPunctuation(uint code) {
    return isInRange(code, '!', '/') || isInRange(code, ':', '@') ||
           isInRange(code, '[', '`') || isInRange(code, '{', '~');
}

// Whitespace as the scanners of links and HTML tags know it: a space, a
// tabulation, a line break or a page break.
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

// A whitespace character as the rules of emphasis know it: a space of the
// Unicode category Zs, a tabulation or a line break.
bool isWhitespace(uint code) {
    if (code >= 0x09 && code <= 0x0D) return true;
    return QChar::category(code) == QChar::Separator_Space;
}

// A punctuation or symbol character as the rules of emphasis know it: ASCII
// punctuation or a Unicode character of the categories P or S.
bool isPunctuationOrSymbol(uint code) {
    if (code < 0x80) return isAsciiPunctuation(code);
    switch (QChar::category(code)) {
    case QChar::Punctuation_Connector:
    case QChar::Punctuation_Dash:
    case QChar::Punctuation_Open:
    case QChar::Punctuation_Close:
    case QChar::Punctuation_InitialQuote:
    case QChar::Punctuation_FinalQuote:
    case QChar::Punctuation_Other:
    case QChar::Symbol_Math:
    case QChar::Symbol_Currency:
    case QChar::Symbol_Modifier:
    case QChar::Symbol_Other:
        return true;
    default:
        return false;
    }
}

// The character before position as a code point. The beginning of the text
// counts as a line break.
uint characterBefore(const QString& text, int position) {
    if (position == 0) return u'\n';
    const uint code = text.at(position - 1).unicode();
    // A character outside the Basic Multilingual Plane takes two units.
    if (QChar::isLowSurrogate(code) && position >= 2) {
        return QChar::surrogateToUcs4(text.at(position - 2), text.at(position - 1));
    }
    return code;
}

// The character at position as a code point. The end of the text counts as
// a line break.
uint characterAfter(const QString& text, int position) {
    if (position >= text.size()) return u'\n';
    const uint code = text.at(position).unicode();
    if (QChar::isHighSurrogate(code) && position + 1 < text.size()) {
        return QChar::surrogateToUcs4(text.at(position), text.at(position + 1));
    }
    return code;
}

void appendCodePoint(QString* text, uint code) {
    if (QChar::requiresSurrogates(code)) {
        text->append(QChar::highSurrogate(code));
        text->append(QChar::lowSurrogate(code));
    } else {
        text->append(QChar(static_cast<ushort>(code)));
    }
}

int compareNames(const QString& name, const char* entity) {
    for (int index = 0; index < name.size(); ++index) {
        const int difference = static_cast<int>(name.at(index).unicode()) -
                               static_cast<int>(static_cast<uchar>(entity[index]));
        if (difference != 0) return difference;
    }
    // The names are equal when the name of the entity ends here too.
    return entity[name.size()] == '\0' ? 0 : -1;
}

// The code points of the named character reference, or nullptr when the name is not known.
const uint* findEntity(const QString& name) {
    int low = 0;
    int high = kHtmlEntityCount - 1;
    while (low <= high) {
        const int middle = low + (high - low) / 2;
        const int difference = compareNames(name, kHtmlEntities[middle].name);
        if (difference == 0) return kHtmlEntities[middle].codePoints;
        if (difference < 0) {
            high = middle - 1;
        } else {
            low = middle + 1;
        }
    }
    return nullptr;
}

// The length of "scheme:" that starts at position, or 0. A scheme has from
// two to thirty two characters.
int uriSchemeLength(const QString& text, int position) {
    const int size = text.size();
    if (position >= size || !isAsciiLetter(text.at(position).unicode())) return 0;
    int current = position + 1;
    while (current < size) {
        const uint code = text.at(current).unicode();
        if (isAsciiAlphanumeric(code) || code == static_cast<uint>('.') ||
            code == static_cast<uint>('+') || code == static_cast<uint>('-')) {
            ++current;
        } else {
            break;
        }
    }
    const int length = current - position;
    if (length < 2 || length > 32 || current >= size || text.at(current) != u':') return 0;
    return length + 1;
}

// The length of the label of a domain name that starts at position, or 0. A
// label has from one to sixty three letters and digits, with hyphens in
// between.
int domainLabelLength(const QString& text, int position) {
    const int size = text.size();
    if (position >= size || !isAsciiAlphanumeric(text.at(position).unicode())) return 0;
    int current = position + 1;
    while (current < size && current - position < 63) {
        const uint code = text.at(current).unicode();
        if (isAsciiAlphanumeric(code) || code == static_cast<uint>('-')) {
            ++current;
        } else {
            break;
        }
    }
    return isAsciiAlphanumeric(text.at(current - 1).unicode()) ? current - position : 0;
}

// The length of the name of an attribute of an HTML tag, or 0.
int attributeNameLength(const QString& text, int position) {
    const int size = text.size();
    if (position >= size) return 0;
    const uint first = text.at(position).unicode();
    if (!isAsciiLetter(first) && first != static_cast<uint>('_') &&
        first != static_cast<uint>(':')) {
        return 0;
    }
    int current = position + 1;
    while (current < size) {
        const uint code = text.at(current).unicode();
        if (isAsciiAlphanumeric(code) || code == static_cast<uint>(':') ||
            code == static_cast<uint>('.') || code == static_cast<uint>('_') ||
            code == static_cast<uint>('-')) {
            ++current;
        } else {
            break;
        }
    }
    return current - position;
}

// The characters that cannot be a part of the value of an attribute when the
// value is not quoted.
bool endsUnquotedValue(QChar character) {
    return isSpace(character) || QStringLiteral("\"'=<>`").contains(character);
}

// The length of the value of an attribute that starts at position, or 0. The
// value is quoted or is a sequence of characters without whitespace.
int attributeValueLength(const QString& text, int position) {
    const int size = text.size();
    if (position >= size) return 0;
    const QChar first = text.at(position);
    if (first == u'"' || first == u'\'') {
        const int end = text.indexOf(first, position + 1);
        return end == -1 ? 0 : end + 1 - position;
    }
    int current = position;
    while (current < size && !endsUnquotedValue(text.at(current))) ++current;
    return current - position;
}

// The length of one attribute of an open tag: whitespace, a name and an
// optional value. Returns 0 when there is no attribute at position.
int attributeLength(const QString& text, int position) {
    const int size = text.size();
    int current = position;
    while (current < size && isSpace(text.at(current))) ++current;
    // There has to be whitespace between the parts of a tag.
    if (current == position) return 0;
    const int name = attributeNameLength(text, current);
    if (name == 0) return 0;
    current += name;
    // The value is optional.
    int afterName = current;
    while (afterName < size && isSpace(text.at(afterName))) ++afterName;
    if (afterName < size && text.at(afterName) == u'=') {
        int value = afterName + 1;
        while (value < size && isSpace(text.at(value))) ++value;
        const int length = attributeValueLength(text, value);
        // Without a value the whole tag is not valid, so the '=' is left
        // where the tag has to end.
        if (length > 0) current = value + length;
    }
    return current - position;
}

// <tagname attribute* space* [/]>
int openTagLength(const QString& text, int position) {
    const int size = text.size();
    int current = position + 1;
    const int name = tagNameLength(text, current);
    if (name == 0) return 0;
    current += name;
    for (int attribute = attributeLength(text, current); attribute > 0;
         attribute = attributeLength(text, current)) {
        current += attribute;
    }
    while (current < size && isSpace(text.at(current))) ++current;
    if (current < size && text.at(current) == u'/') ++current;
    return current < size && text.at(current) == u'>' ? current + 1 - position : 0;
}

// </tagname space*>
int closeTagLength(const QString& text, int position) {
    const int size = text.size();
    int current = position + 2;
    const int name = tagNameLength(text, current);
    if (name == 0) return 0;
    current += name;
    while (current < size && isSpace(text.at(current))) ++current;
    return current < size && text.at(current) == u'>' ? current + 1 - position : 0;
}

// "<!--" + text + "-->". The text does not start with ">" or "->" and does
// not end with "-". The position is at "<!".
int commentLength(const QString& text, int position) {
    const int size = text.size();
    if (position + 3 >= size || text.at(position + 2) != u'-' || text.at(position + 3) != u'-') {
        return 0;
    }
    int current = position + 4;
    // The shortest comments are "<!-->" and "<!--->".
    if (current < size && text.at(current) == u'>') return current + 1 - position;
    if (current + 1 < size && text.at(current) == u'-' && text.at(current + 1) == u'>') {
        return current + 2 - position;
    }
    while (current < size) {
        // Anything that is not a hyphen is a part of the comment.
        while (current < size && text.at(current) != u'-') ++current;
        if (current == size) return 0;
        // One hyphen followed by a character that is not a hyphen is a part
        // of the comment.
        if (current + 1 < size && text.at(current + 1) != u'-') {
            current += 2;
            continue;
        }
        // Two hyphens followed by a character that is not the end of the
        // comment are a part of the comment too.
        if (current + 2 < size && text.at(current + 2) != u'>') {
            current += 3;
            continue;
        }
        // Only "-->" can follow now. The comment cannot end with a hyphen.
        return current + 2 < size && text.at(current + 2) == u'>' ? current + 3 - position : 0;
    }
    return 0;
}

// The length of the text up to the marker, starting from position.
int upToMarker(const QString& text, int position, const QString& marker) {
    const int found = text.indexOf(marker, position);
    return found == -1 ? 0 : found + marker.size() - position;
}

// "<![CDATA[" + text + "]]>". The text cannot contain "]]>". The position is at "<!".
int cdataLength(const QString& text, int position) {
    if (text.mid(position, 9) != QStringLiteral("<![CDATA[")) return 0;
    const int rest = upToMarker(text, position + 9, QStringLiteral("]]>"));
    return rest == 0 ? 0 : 9 + rest;
}

// The length of the email autolink that follows the '<' at position, or 0.
int emailAutolinkLength(const QString& text, int position) {
    const int size = text.size();
    const QString localCharacters = QStringLiteral(".!#$%&'*+/=?^_`{|}~-");
    int current = position;
    while (current < size) {
        const uint code = text.at(current).unicode();
        if (isAsciiAlphanumeric(code) ||
            localCharacters.contains(QChar(static_cast<uchar>(code)))) {
            ++current;
        } else {
            break;
        }
    }
    if (current == position || current >= size || text.at(current) != u'@') return 0;
    ++current;
    while (true) {
        const int label = domainLabelLength(text, current);
        if (label == 0) return 0;
        current += label;
        if (current < size && text.at(current) == u'.') {
            ++current;
            continue;
        }
        break;
    }
    if (current >= size || text.at(current) != u'>') return 0;
    return current + 1 - position;
}

}  // namespace

bool decodeEntity(const QString& text, int position, QString* decoded, int* end) {
    const int size = text.size();
    int current = position + 1;
    if (current >= size) return false;

    if (text.at(current) == u'#') {
        ++current;
        const bool isHex = current < size && (text.at(current) == u'x' || text.at(current) == u'X');
        if (isHex) ++current;
        const uint base = isHex ? 16 : 10;
        const int digitsStart = current;
        uint code = 0;
        while (current < size && current - digitsStart < 8) {
            const QChar c = text.at(current);
            int digit = -1;
            if (isHex && c.isLetter()) {
                // The digits of a hexadecimal number may be letters.
                const uint letter = c.toLower().unicode();
                digit = letter >= u'a' && letter <= u'f' ? letter - u'a' + 10 : -1;
            } else if (isAsciiDigit(c.unicode())) {
                digit = c.digitValue();
            }
            if (digit < 0) break;
            code = code * base + static_cast<uint>(digit);
            ++current;
        }
        if (current == digitsStart || current >= size || text.at(current) != u';') return false;
        // A number that is too large for a character does not make a
        // character reference. The loop stops after eight digits, which is
        // more than the largest character needs, so such a number does not
        // reach the semicolon.
        if (code > 0x10FFFF) return false;
        // Zero and the surrogates do not stand for characters.
        if (code == 0 || (code >= 0xD800 && code <= 0xDFFF)) code = 0xFFFD;
        decoded->clear();
        appendCodePoint(decoded, code);
        *end = current + 1;
        return true;
    }

    // A named reference: its name has to be known and to end with a semicolon.
    const int semicolon = text.indexOf(u';', current);
    // The longest name of the table has thirty one characters.
    if (semicolon == -1 || semicolon - current > 31) return false;
    const uint* codePoints = findEntity(text.mid(current, semicolon - current));
    if (codePoints == nullptr) return false;
    decoded->clear();
    appendCodePoint(decoded, codePoints[0]);
    if (codePoints[1] != 0) appendCodePoint(decoded, codePoints[1]);
    *end = semicolon + 1;
    return true;
}

QString escapeHtml(const QString& text) {
    QString escaped;
    escaped.reserve(text.size());
    for (const QChar c : text) {
        switch (c.unicode()) {
        case '&':
            escaped += QStringLiteral("&amp;");
            break;
        case '<':
            escaped += QStringLiteral("&lt;");
            break;
        case '>':
            escaped += QStringLiteral("&gt;");
            break;
        case '"':
            escaped += QStringLiteral("&quot;");
            break;
        default:
            escaped += c;
        }
    }
    return escaped;
}

QString encodeUrl(const QString& url) {
    static const char* const hexDigits = "0123456789ABCDEF";
    // The characters that stay as they are in an address.
    const QString kept = QStringLiteral("!#$%&'()*+,-./:;=?@_~");
    QString encoded;
    encoded.reserve(url.size());
    for (int index = 0; index < url.size(); ++index) {
        uint code = url.at(index).unicode();
        QString character;
        if (QChar::isHighSurrogate(code) && index + 1 < url.size() &&
            QChar::isLowSurrogate(url.at(index + 1).unicode())) {
            character = url.mid(index, 2);
            code = QChar::surrogateToUcs4(url.at(index), url.at(index + 1));
            ++index;
        } else {
            character = QChar(static_cast<ushort>(code));
        }
        if (code < 0x80) {
            const QChar ascii(static_cast<uchar>(code));
            if (isAsciiAlphanumeric(code) || kept.contains(ascii)) {
                encoded += character;
                continue;
            }
        }
        for (const char byte : character.toUtf8()) {
            const auto value = static_cast<uchar>(byte);
            encoded += QChar(u'%');
            encoded += QChar(hexDigits[value >> 4]);
            encoded += QChar(hexDigits[value & 0x0F]);
        }
    }
    return encoded;
}

QString cleanUrl(const QString& url) { return unescapeText(url.trimmed()); }

QString cleanTitle(const QString& title) {
    if (title.isEmpty()) return QString();
    const QChar first = title.at(0);
    const QChar last = title.at(title.size() - 1);
    // The quotes around the title are not a part of it.
    const bool isQuoted =
        (first == u'"' && last == u'"') || (first == u'\'' && last == u'\'') ||
        (first == u'(' && last == u')');
    return unescapeText(isQuoted ? title.mid(1, title.size() - 2) : title);
}

QString unescapeText(const QString& text) {
    QString result;
    for (int position = 0; position < text.size();) {
        const QChar c = text.at(position);
        if (c == u'\\' && position + 1 < text.size() &&
            isAsciiPunctuation(text.at(position + 1).unicode())) {
            result += text.at(position + 1);
            position += 2;
            continue;
        }
        if (c == u'&') {
            QString decoded;
            int end = 0;
            if (decodeEntity(text, position, &decoded, &end)) {
                result += decoded;
                position = end;
                continue;
            }
        }
        result += c;
        ++position;
    }
    return result;
}

int tagNameLength(const QString& text, int position) {
    const int size = text.size();
    if (position >= size || !isAsciiLetter(text.at(position).unicode())) return 0;
    int current = position + 1;
    while (current < size) {
        const uint code = text.at(current).unicode();
        if (isAsciiAlphanumeric(code) || code == static_cast<uint>('-')) {
            ++current;
        } else {
            break;
        }
    }
    return current - position;
}

int htmlLength(const QString& text, int position) {
    const int size = text.size();
    if (position + 1 >= size || text.at(position) != u'<') return 0;
    const QChar second = text.at(position + 1);
    if (second == u'/') return closeTagLength(text, position);
    if (second == u'?') {
        // A processing instruction: "<?" + text + "?>". The text may contain
        // '>' and '?' of their own.
        const int found = text.indexOf(QStringLiteral("?>"), position + 2);
        return found == -1 ? 0 : found + 2 - position;
    }
    if (second != u'!') return openTagLength(text, position);
    if (position + 2 >= size) return 0;
    const QChar third = text.at(position + 2);
    if (third == u'-') return commentLength(text, position);
    if (third == u'[') return cdataLength(text, position);
    // A declaration: "<!" + letters + text + ">".
    if (!isAsciiLetter(third.unicode())) return 0;
    const int found = text.indexOf(u'>', position + 2);
    return found == -1 ? 0 : found + 1 - position;
}

int autolinkLength(const QString& text, int position, bool* isEmail) {
    const int size = text.size();
    // A URI: "<scheme:" + text + ">". The text has no whitespace and no angle brackets.
    const int scheme = uriSchemeLength(text, position + 1);
    if (scheme > 0) {
        int current = position + 1 + scheme;
        while (current < size && text.at(current) != u'>' && text.at(current).unicode() > 0x20) {
            ++current;
        }
        if (current < size && text.at(current) == u'>') {
            *isEmail = false;
            return current + 1 - position;
        }
    }
    // An email address: "<" + local part + "@" + domain + ">".
    const int address = emailAutolinkLength(text, position + 1);
    *isEmail = address > 0;
    return address > 0 ? address + 1 : 0;
}

int delimiterRunLength(const QString& text, int position, bool* canOpen, bool* canClose) {
    const int size = text.size();
    const QChar character = text.at(position);
    int end = position;
    while (end < size && text.at(end) == character) ++end;
    const uint before = characterBefore(text, position);
    const uint after = characterAfter(text, end);
    const bool beforeIsWhitespace = isWhitespace(before);
    const bool beforeIsPunctuation = isPunctuationOrSymbol(before);
    const bool afterIsWhitespace = isWhitespace(after);
    const bool afterIsPunctuation = isPunctuationOrSymbol(after);
    const bool leftFlanking = !afterIsWhitespace &&
                              (!afterIsPunctuation || beforeIsWhitespace || beforeIsPunctuation);
    const bool rightFlanking = !beforeIsWhitespace &&
                               (!beforeIsPunctuation || afterIsWhitespace || afterIsPunctuation);
    if (character == u'_') {
        *canOpen = leftFlanking && (!rightFlanking || beforeIsPunctuation);
        *canClose = rightFlanking && (!leftFlanking || afterIsPunctuation);
    } else {
        *canOpen = leftFlanking;
        *canClose = rightFlanking;
    }
    return end - position;
}

int codeSpanEnd(const QString& text, int from, int runLength) {
    int position = from;
    while (position < text.size()) {
        if (text.at(position) != u'`') {
            ++position;
            continue;
        }
        int end = position;
        while (end < text.size() && text.at(end) == u'`') ++end;
        if (end - position == runLength) return position;
        position = end;
    }
    return -1;
}

int htmlTagEnd(const QString& text, int position) {
    const int length = htmlLength(text, position);
    return length == 0 ? -1 : position + length;
}

int scanLinkDestination(const QString& text, int position, QString* destination) {
    const int size = text.size();
    int current = position;
    if (current < size && text.at(current) == u'<') {
        ++current;
        bool isClosed = false;
        while (current < size) {
            const QChar c = text.at(current);
            if (c == u'>') {
                isClosed = true;
                ++current;
                break;
            }
            if (c == u'\\') {
                // An escape may cover the next character, but a line ending
                // cannot be inside the angle brackets.
                if (current + 1 < size && text.at(current + 1) != u'\n') {
                    current += 2;
                } else {
                    return -1;
                }
            } else if (c == u'\n' || c == u'<') {
                return -1;
            } else {
                ++current;
            }
        }
        if (!isClosed) return -1;
        *destination = text.mid(position + 1, current - position - 2);
        return current - position;
    }

    int depth = 0;
    while (current < size) {
        const QChar c = text.at(current);
        if (c == u'\\' && current + 1 < size &&
            isAsciiPunctuation(text.at(current + 1).unicode())) {
            current += 2;
        } else if (c == u'(') {
            // Parentheses are a part of the destination when they are balanced.
            ++depth;
            ++current;
            if (depth > 32) return -1;
        } else if (c == u')') {
            if (depth == 0) break;
            --depth;
            ++current;
        } else if (isSpace(c)) {
            // The destination cannot be empty.
            if (current == position) return -1;
            break;
        } else {
            ++current;
        }
    }
    // The destination cannot run to the end of the text: a definition or
    // an inline link always has something after it.
    if (depth != 0 || current == size) return -1;
    *destination = text.mid(position, current - position);
    return current - position;
}

int scanLinkTitle(const QString& text, int position) {
    const int size = text.size();
    if (position >= size) return 0;
    const QChar opening = text.at(position);
    QChar closing;
    if (opening == u'"' || opening == u'\'') {
        closing = opening;
    } else if (opening == u'(') {
        closing = u')';
    } else {
        return 0;
    }
    int current = position + 1;
    while (current < size) {
        const QChar c = text.at(current);
        if (c == u'\\' && current + 1 < size) {
            current += 2;
            continue;
        }
        if (c == closing) return current + 1 - position;
        // A title in parentheses may not contain an opening parenthesis.
        if (c == opening) return 0;
        ++current;
    }
    return 0;
}

int scanLinkLabel(const QString& text, int position, QString* label) {
    const int size = text.size();
    if (position >= size || text.at(position) != u'[') return 0;
    int current = position + 1;
    int length = 0;
    while (current < size) {
        const QChar c = text.at(current);
        // The label ends at the first closing bracket. An opening bracket
        // cannot be a part of it.
        if (c == u']') {
            *label = text.mid(position + 1, current - position - 1).trimmed();
            return current + 1 - position;
        }
        if (c == u'[') return 0;
        if (c == u'\\' && current + 1 < size &&
            isAsciiPunctuation(text.at(current + 1).unicode())) {
            ++length;
            ++current;
        }
        ++length;
        ++current;
        if (length > 999) return 0;
    }
    return 0;
}

QString normalizeLabel(const QString& label) {
    QString normalized = label.toCaseFolded();
    // The full case folding of the sharp s is "ss". Case folding alone
    // leaves it as the small sharp s.
    normalized.replace(QChar(0x00DF), QStringLiteral("ss"));
    return normalized.simplified();
}

}  // namespace mdeditor::md
