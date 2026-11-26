#include "markdown_editor.h"

#include <QFont>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QPalette>
#include <QTextBlock>
#include <QTextCharFormat>

#include "core/markdown_parser.h"

namespace mdeditor {

MarkdownEditor::MarkdownEditor(QWidget* parent) : QPlainTextEdit(parent) {
    setObjectName(QStringLiteral("markdownEditor"));
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(11);
    document()->setDefaultFont(font);
    // One tab stop is as wide as the Markdown structure counts it: four columns.
    setTabStopDistance(QFontMetricsF(font).horizontalAdvance(QStringLiteral("    ")));
    setPlaceholderText(QStringLiteral("Введите текст в формате Markdown…"));
    connect(this, &QPlainTextEdit::cursorPositionChanged, this,
            &MarkdownEditor::highlightCurrentLine);
    highlightCurrentLine();
}

void MarkdownEditor::keyPressEvent(QKeyEvent* event) {
    // Tab and Backtab change the indentation of the text instead of moving
    // the focus out of the editor.
    if (event->key() == Qt::Key_Tab && !(event->modifiers() & Qt::ShiftModifier)) {
        indentBy(md::kTabWidth);
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Backtab) {
        indentBy(-md::kTabWidth);
        event->accept();
        return;
    }
    QPlainTextEdit::keyPressEvent(event);
}

void MarkdownEditor::indentBy(int columns) {
    const QTextCursor cursor = textCursor();
    // All the lines that the selection touches are indented. When nothing is
    // selected, it is the line of the cursor.
    const int firstBlock = document()->findBlock(cursor.selectionStart()).blockNumber();
    const int lastBlock = document()->findBlock(cursor.selectionEnd()).blockNumber();
    QTextCursor line(document());
    line.beginEditBlock();
    for (int number = firstBlock; number <= lastBlock; ++number) {
        const QTextBlock block = document()->findBlockByNumber(number);
        line.setPosition(block.position());
        if (columns > 0) {
            line.insertText(QString(columns, QChar(u' ')));
        } else {
            // Only the indentation goes. The text stays where it is.
            const QString text = block.text();
            int spaces = 0;
            while (spaces < -columns && spaces < text.size() && text.at(spaces) == u' ') ++spaces;
            for (; spaces > 0; --spaces) line.deleteChar();
        }
    }
    line.endEditBlock();
}

void MarkdownEditor::highlightCurrentLine() {
    // The selection of the text has to stay visible: the line is painted under it.
    QTextEdit::ExtraSelection line;
    line.format.setProperty(QTextFormat::FullWidthSelection, true);
    line.format.setBackground(palette().color(QPalette::AlternateBase));
    line.cursor = textCursor();
    line.cursor.clearSelection();
    setExtraSelections(QList<QTextEdit::ExtraSelection>{line});
}

}  // namespace mdeditor
