#pragma once

#include <QTextBrowser>
#include <QUrl>

#include "core/rich_text_document.h"

class QLabel;
class QResizeEvent;
class QWheelEvent;

namespace mdeditor {

// The panel that shows the Markdown text as a rendered document. The document
// is made anew each time the text is shown.
class MarkdownPreview : public QTextBrowser {
    Q_OBJECT

public:
    explicit MarkdownPreview(QWidget* parent = nullptr);

    // Shows the Markdown text. The relative paths of the images are looked
    // for in sourceDirectory.
    void showMarkdown(const QString& markdown, const QString& sourceDirectory);
    void setStyle(const TextStyle& style);

    // Makes the text of the preview larger or smaller by steps of one point.
    void changeZoom(int steps);
    int zoom() const { return zoom_; }

protected:
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    // Makes the document of the Markdown text and shows it.
    void render();
    // Fits the images that are wider than the panel into it.
    void fitImages();
    // Opens the link with the program of the system. A relative address
    // stands for a file next to the Markdown file.
    void openLink(const QUrl& link);

    QString markdown_;
    QString sourceDirectory_;
    TextStyle style_;
    int zoom_ = 0;
    ImageCache images_;
    // What the panel says when there is no text to show.
    QLabel* placeholder_;
};

}  // namespace mdeditor
