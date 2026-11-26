#include "core/markdown_inline.h"

#include <QChar>

#include "core/markdown_inline_parser.h"

namespace mdeditor::md {
namespace {

QString quote(const QString& text) {
    return QStringLiteral("\"") + text + QStringLiteral("\"");
}

// href="…" with the optional title="…". The address is percent-encoded.
QString linkAttributes(const InlineNode* node) {
    QString attributes =
        QStringLiteral(" href=") + quote(escapeHtml(encodeUrl(node->destination)));
    if (!node->title.isEmpty()) attributes += QStringLiteral(" title=") + quote(escapeHtml(node->title));
    return attributes;
}

void renderHtml(const InlineNode* parent, QString* out) {
    for (const InlineNode* node = parent->firstChild; node != nullptr; node = node->next) {
        switch (node->type) {
        case InlineNode::Type::Text:
            *out += escapeHtml(node->literal);
            break;
        case InlineNode::Type::Code:
            *out += QStringLiteral("<code>") + escapeHtml(node->literal) +
                    QStringLiteral("</code>");
            break;
        case InlineNode::Type::SoftBreak:
            *out += QChar(u'\n');
            break;
        case InlineNode::Type::HardBreak:
            *out += QStringLiteral("<br>\n");
            break;
        case InlineNode::Type::RawHtml:
            // Raw HTML is passed through unchanged.
            *out += node->literal;
            break;
        case InlineNode::Type::Emphasis: {
            const QString tag = node->level == 1 ? QStringLiteral("em") : QStringLiteral("strong");
            *out += QStringLiteral("<") + tag + QStringLiteral(">");
            renderHtml(node, out);
            *out += QStringLiteral("</") + tag + QStringLiteral(">");
            break;
        }
        case InlineNode::Type::Strikethrough:
            *out += QStringLiteral("<del>");
            renderHtml(node, out);
            *out += QStringLiteral("</del>");
            break;
        case InlineNode::Type::Link:
            *out += QStringLiteral("<a") + linkAttributes(node) + QStringLiteral(">");
            renderHtml(node, out);
            *out += QStringLiteral("</a>");
            break;
        case InlineNode::Type::Image:
            // The alternate text is what the content of the image says when it is read as plain text.
            *out += QStringLiteral("<img src=") + quote(escapeHtml(encodeUrl(node->destination))) +
                    QStringLiteral(" alt=") + quote(escapeHtml(renderInlinePlainText(node))) +
                    (node->title.isEmpty() ? QString()
                                          : QStringLiteral(" title=") + quote(escapeHtml(node->title))) +
                    QChar(u'>');
            break;
        }
    }
}

// The text without any markup: it is used for the alternate text of images
// and for the titles of the blocks.
void renderPlainText(const InlineNode* parent, QString* out) {
    for (const InlineNode* node = parent->firstChild; node != nullptr; node = node->next) {
        switch (node->type) {
        case InlineNode::Type::Text:
        case InlineNode::Type::Code:
        case InlineNode::Type::RawHtml:
            *out += node->literal;
            break;
        case InlineNode::Type::SoftBreak:
        case InlineNode::Type::HardBreak:
            *out += QChar(u'\n');
            break;
        default:
            renderPlainText(node, out);
            break;
        }
    }
}

}  // namespace

void parseInlines(const QString& text, const InlineContext& context, InlineTree* tree) {
    InlineParser parser(text, context.links, tree);
    parser.parse();
}

QString renderInlineHtml(InlineTree& tree) {
    QString html;
    renderHtml(tree.root(), &html);
    return html;
}

QString renderInlinePlainText(const InlineNode* node) {
    QString text;
    renderPlainText(node, &text);
    return text;
}

QString renderInline(const QString& text, const InlineContext& context) {
    InlineTree tree;
    parseInlines(text, context, &tree);
    return context.plainText ? renderInlinePlainText(tree.root()) : renderInlineHtml(tree);
}

}  // namespace mdeditor::md
