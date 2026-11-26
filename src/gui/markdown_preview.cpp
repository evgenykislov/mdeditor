#include "markdown_preview.h"

#include <QAbstractTextDocumentLayout>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QPalette>
#include <QResizeEvent>
#include <QScrollBar>
#include <QWheelEvent>

#include <algorithm>

#include "core/document_io.h"
#include "core/markdown_parser.h"

namespace mdeditor {

MarkdownPreview::MarkdownPreview(QWidget* parent) : QTextBrowser(parent) {
    setObjectName(QStringLiteral("markdownPreview"));
    // The links are opened by the panel itself: a relative address stands for
    // a file next to the Markdown file.
    setOpenLinks(false);
    setOpenExternalLinks(false);
    connect(this, &QTextBrowser::anchorClicked, this, &MarkdownPreview::openLink);

    placeholder_ = new QLabel(this);
    placeholder_->setObjectName(QStringLiteral("previewPlaceholder"));
    placeholder_->setText(QStringLiteral("Результат разметки появится здесь,\n"
                                         "как только в редакторе появится текст."));
    placeholder_->setAlignment(Qt::AlignCenter);
    placeholder_->setWordWrap(true);
    QPalette palette = placeholder_->palette();
    palette.setColor(QPalette::WindowText, placeholder_->palette().color(QPalette::PlaceholderText));
    placeholder_->setPalette(palette);
    placeholder_->hide();

    style_ = defaultTextStyle();
}

void MarkdownPreview::showMarkdown(const QString& markdown, const QString& sourceDirectory) {
    markdown_ = markdown;
    sourceDirectory_ = sourceDirectory;
    render();
}

void MarkdownPreview::setStyle(const TextStyle& style) {
    style_ = style;
    render();
}

void MarkdownPreview::changeZoom(int steps) {
    const int zoom = std::clamp(zoom_ + steps, -6, 14);
    if (zoom == zoom_) return;
    zoom_ = zoom;
    render();
}

void MarkdownPreview::render() {
    placeholder_->setVisible(markdown_.trimmed().isEmpty());
    // The place that the reader looks at stays in view.
    const int scroll = verticalScrollBar()->value();

    TextStyle style = style_;
    style.fontSize += zoom_;
    auto made = DocumentIO::createPreviewDocument(markdown_, sourceDirectory_, style, &images_);
    setDocument(made.release());
    fitImages();

    // The layout has to know the length of the document before the scroll
    // position can be set.
    document()->documentLayout()->documentSize();
    verticalScrollBar()->setValue(scroll);
}

void MarkdownPreview::fitImages() {
    QTextDocument* shown = document();
    const qreal width = viewport()->width() - 2 * shown->documentMargin();
    fitImagesToWidth(*shown, width, viewport());
}

void MarkdownPreview::resizeEvent(QResizeEvent* event) {
    QTextBrowser::resizeEvent(event);
    placeholder_->setGeometry(viewport()->rect().adjusted(24, 24, -24, -24));
    // The images are fitted again: the panel has become wider or narrower.
    fitImages();
}

void MarkdownPreview::wheelEvent(QWheelEvent* event) {
    // The wheel with Ctrl changes the size of the text.
    if (event->modifiers() & Qt::ControlModifier) {
        changeZoom(event->angleDelta().y() > 0 ? 1 : -1);
        event->accept();
        return;
    }
    QTextBrowser::wheelEvent(event);
}

void MarkdownPreview::openLink(const QUrl& link) {
    // The links inside the document are not used: the document has no anchors.
    if (link.toString().startsWith(QLatin1Char('#'))) return;
    QUrl target = link;
    if (link.isRelative() && !sourceDirectory_.isEmpty()) {
        const QString path =
            QDir(sourceDirectory_).filePath(QUrl::fromPercentEncoding(link.toEncoded()));
        if (!QFileInfo::exists(path)) {
            QMessageBox::warning(this, QStringLiteral("Не удалось открыть ссылку"),
                                 QStringLiteral("Файл не найден:") + QStringLiteral("\n") + path);
            return;
        }
        target = QUrl::fromLocalFile(path);
    }
    if (!QDesktopServices::openUrl(target)) {
        QMessageBox::warning(this, QStringLiteral("Не удалось открыть ссылку"),
                             QStringLiteral("Системе не удалось открыть:") + QStringLiteral("\n") +
                                 link.toString());
    }
}

}  // namespace mdeditor
