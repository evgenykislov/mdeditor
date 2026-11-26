#pragma once

#include <QAbstractButton>
#include <QApplication>
#include <QDialog>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QObject>
#include <QTimer>
#include <QUrl>

#include <QList>
#include <QString>

// The dialogs that the window opens are modal: they wait for the user inside
// an event loop of their own. This object answers them from there by a timer:
// the timer keeps running while a dialog waits.
class DialogScript : public QObject {
    Q_OBJECT

public:
    explicit DialogScript(QObject* parent = nullptr) : QObject(parent) {
        timer_.setInterval(20);
        connect(&timer_, &QTimer::timeout, this, &DialogScript::answer);
    }

    // Starts watching for dialogs.
    void arm() { timer_.start(); }

    // The file dialog that opens next gets the file and is accepted.
    void expectFile(const QString& path) {
        steps_.append(Step{Kind::File, path, QMessageBox::NoButton});
    }

    // The file dialog that opens next is cancelled.
    void expectCancel() { steps_.append(Step{Kind::Cancel, QString(), QMessageBox::NoButton}); }

    // The message box that opens next gets the button.
    void expectMessage(QMessageBox::StandardButton button) {
        steps_.append(Step{Kind::Message, QString(), button});
    }

    // Whether every dialog that was expected has been answered.
    bool isDone() const { return steps_.isEmpty(); }

    // Whether a dialog opened that was not expected. Such a dialog is closed
    // at once, so that the test goes on and fails instead of waiting forever.
    bool sawUnexpected() const { return sawUnexpected_; }

private:
    void answer() {
        QWidget* modal = QApplication::activeModalWidget();
        if (modal == nullptr) return;
        // A question of the file dialog itself, such as whether a file may be
        // replaced, is answered with yes: it is a part of choosing the file.
        auto* box = qobject_cast<QMessageBox*>(modal);
        if (box != nullptr && qobject_cast<QFileDialog*>(box->parentWidget()) != nullptr &&
            box->button(QMessageBox::Yes) != nullptr) {
            box->button(QMessageBox::Yes)->click();
            return;
        }
        if (steps_.isEmpty()) {
            sawUnexpected_ = true;
            if (auto* dialog = qobject_cast<QDialog*>(modal)) {
                static_cast<QDialog*>(dialog)->reject();
            } else {
                modal->close();
            }
            return;
        }
        const Step step = steps_.takeFirst();
        if (step.kind == Kind::Message) {
            QAbstractButton* button = box != nullptr ? box->button(step.button) : nullptr;
            if (button != nullptr) {
                button->click();
            } else if (box != nullptr) {
                box->reject();
            }
            return;
        }
        auto* dialog = qobject_cast<QFileDialog*>(modal);
        if (dialog == nullptr) {
            modal->close();
        } else if (step.kind == Kind::File) {
            // The path is written into the field of the name as the user
            // would write it. The whole path is written, so that the
            // directory of the dialog does not matter.
            dialog->findChild<QLineEdit*>(QStringLiteral("fileNameEdit"))->setText(step.path);
            static_cast<QDialog*>(dialog)->accept();
        } else {
            static_cast<QDialog*>(dialog)->reject();
        }
    }

    enum class Kind { File, Cancel, Message };
    struct Step {
        Kind kind;
        QString path;
        QMessageBox::StandardButton button;
    };

    QTimer timer_;
    QList<Step> steps_;
    bool sawUnexpected_ = false;
};

// Collects the addresses that the program asks the system to open.
class UrlReceiver : public QObject {
    Q_OBJECT

public:
    QList<QUrl> opened;

public slots:
    void openUrl(const QUrl& url) { opened << url; }
};
