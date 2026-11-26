#include "core/markdown_inline_parser.h"

namespace mdeditor::md {
namespace {

// Column of a delimiter character in the table of the lowest openers.
int delimiterColumn(QChar character) {
    return character == u'*' ? 0 : character == u'_' ? 1 : 2;
}

}  // namespace

// Pairs the runs of delimiters that end inside the range that starts at
// stackBottom in the text, following the algorithm of CommonMark.
void InlineParser::processEmphasis(int stackBottom) {
    // The lowest delimiter that can still open an emphasis for a closer.
    // There is a boundary for each delimiter character, for the closers that
    // can open too and for those that cannot, and for each length of the
    // closer modulo three: when a closer has no opener, the delimiters before
    // it cannot open an emphasis for the closers of the same kind either.
    int openersBottom[3][2][3];
    for (auto& characters : openersBottom) {
        for (auto& canOpen : characters) {
            for (int& cell : canOpen) cell = stackBottom;
        }
    }

    // The first delimiter of the range is the first closer to look at.
    Delimiter* closer = nullptr;
    for (Delimiter* candidate = lastDelimiter_;
         candidate != nullptr && candidate->position >= stackBottom;
         candidate = candidate->previous) {
        closer = candidate;
    }

    while (closer != nullptr) {
        if (!closer->canClose) {
            closer = closer->next;
            continue;
        }
        const int column = delimiterColumn(closer->character);
        const int row = closer->canOpen ? 1 : 0;
        const int cell = closer->length % 3;
        const int lowestOpener = openersBottom[column][row][cell];

        // The nearest run that can open an emphasis for the closer.
        Delimiter* opener = closer->previous;
        bool isOpened = false;
        while (opener != nullptr && opener->position >= lowestOpener) {
            if (opener->canOpen && opener->character == closer->character) {
                // When one of the two runs can both open and close, the
                // lengths of the runs must not sum up to a multiple of three,
                // unless the length of the closer is a multiple of three.
                const bool isOdd =
                    (closer->canOpen || opener->canClose) && closer->length % 3 != 0 &&
                    (opener->length + closer->length) % 3 == 0;
                if (!isOdd) {
                    isOpened = true;
                    break;
                }
            }
            opener = opener->previous;
        }

        Delimiter* next = nullptr;
        if (isOpened) {
            next = closer->character == u'~' ? insertStrikethrough(opener, closer)
                                             : insertEmphasis(opener, closer);
        } else {
            openersBottom[column][row][cell] = closer->position;
            next = closer->next;
            // A closer that cannot open is of no use anymore.
            if (!closer->canOpen) removeDelimiter(closer);
        }
        closer = next;
    }

    // The delimiters of the range are not needed anymore.
    while (lastDelimiter_ != nullptr && lastDelimiter_->position >= stackBottom) {
        removeDelimiter(lastDelimiter_);
    }
}

// Makes an emphasis from one or two characters of each of the two runs. The
// content of the emphasis is what is between the runs. Returns the closer
// that is looked at next: the same one when characters are left in it.
Delimiter* InlineParser::insertEmphasis(Delimiter* opener, Delimiter* closer) {
    const int use =
        opener->node->literal.size() >= 2 && closer->node->literal.size() >= 2 ? 2 : 1;

    InlineNode* emphasis = tree_->create(InlineNode::Type::Emphasis);
    emphasis->level = use;
    // What is between the runs becomes the content of the emphasis.
    InlineTree::moveNodesBetween(opener->node, closer->node, emphasis);
    InlineTree::insertAfter(opener->node, emphasis);

    // The characters that are left stay outside the emphasis: on the left of
    // it for the opening run, on the right for the closing one.
    opener->node->literal.chop(use);
    closer->node->literal = closer->node->literal.mid(use);

    // The runs between the two are inside the emphasis.
    Delimiter* between = closer->previous;
    while (between != nullptr && between != opener) {
        Delimiter* previous = between->previous;
        removeDelimiter(between);
        between = previous;
    }
    if (opener->node->literal.isEmpty()) {
        InlineTree::unlink(opener->node);
        removeDelimiter(opener);
    }
    if (closer->node->literal.isEmpty()) {
        InlineTree::unlink(closer->node);
        Delimiter* next = closer->next;
        removeDelimiter(closer);
        return next;
    }
    return closer;
}

// A strikethrough is made by two runs of two tildes. Returns the closer that
// is looked at next.
Delimiter* InlineParser::insertStrikethrough(Delimiter* opener, Delimiter* closer) {
    // Both runs have two tildes, so all of them are used.
    InlineNode* strikethrough = opener->node;
    strikethrough->type = InlineNode::Type::Strikethrough;
    strikethrough->literal.clear();
    InlineTree::moveNodesBetween(strikethrough, closer->node, strikethrough);
    InlineTree::unlink(closer->node);

    Delimiter* next = closer->next;
    Delimiter* between = closer->previous;
    while (between != nullptr && between != opener) {
        Delimiter* previous = between->previous;
        removeDelimiter(between);
        between = previous;
    }
    removeDelimiter(closer);
    removeDelimiter(opener);
    return next;
}

}  // namespace mdeditor::md
