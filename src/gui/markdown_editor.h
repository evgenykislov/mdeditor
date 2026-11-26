#pragma once

#include <QPlainTextEdit>

class QKeyEvent;

namespace mdeditor {

// The panel where the Markdown text is edited.
class MarkdownEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit MarkdownEditor(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    // Adds columns spaces to the beginning of the lines that the selection
    // touches. The indentation of the lines is made shorter by columns when
    // the number is negative.
    void indentBy(int columns);

    // The line that has the cursor gets a color of its own.
    void highlightCurrentLine();
};

}  // namespace mdeditor
