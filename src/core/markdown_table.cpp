#include "core/markdown_parser.h"

#include <QChar>

#include <optional>
#include <vector>

namespace mdeditor::md {

// Splits a table row into cells. A pipe inside a code span and an escaped
// pipe are parts of a cell. The pipes at both ends of the row are optional.
QStringList splitTableRow(const QString& line) {
    QString row = line.trimmed();
    if (row.startsWith(u'|')) row = row.mid(1);
    QStringList cells;
    QString cell;
    int position = 0;
    while (position < row.size()) {
        const QChar c = row.at(position);
        // An escaped pipe is a part of the cell, and the backslash is not:
        // inside a code span it would be shown.
        if (c == u'\\' && position + 1 < row.size() && row.at(position + 1) == u'|') {
            cell += row.at(position + 1);
            position += 2;
            continue;
        }
        if (c == u'`') {
            int runEnd = position;
            while (runEnd < row.size() && row.at(runEnd) == u'`') ++runEnd;
            const int closing = codeSpanEnd(row, runEnd, runEnd - position);
            if (closing != -1) {
                // The escaped pipe stands for the pipe inside the code span too.
                QString code = row.mid(position, closing + (runEnd - position) - position);
                code.replace(QStringLiteral("\\|"), QStringLiteral("|"));
                cell += code;
                position = closing + (runEnd - position);
                continue;
            }
        }
        if (c == u'|') {
            cells << cell.trimmed();
            cell.clear();
            ++position;
            continue;
        }
        cell += c;
        ++position;
    }
    cells << cell.trimmed();
    // The pipe that closes the row does not make a cell of its own.
    if (cells.size() > 1 && row.endsWith(u'|') && cells.last().isEmpty()) cells.removeLast();
    return cells;
}

namespace {

// A cell of a delimiter row: one or more dashes with an optional colon at
// each end.
bool parseDelimiterCell(const QString& cell, CellAlignment* alignment) {
    int position = 0;
    const bool isLeft = cell.startsWith(u':');
    if (isLeft) position = 1;
    const int dashesStart = position;
    while (position < cell.size() && cell.at(position) == u'-') ++position;
    if (position == dashesStart) return false;
    const bool isRight = position < cell.size() && cell.at(position) == u':';
    if (isRight) ++position;
    if (position != cell.size()) return false;
    *alignment = isLeft && isRight ? CellAlignment::Center
                 : isLeft          ? CellAlignment::Left
                 : isRight         ? CellAlignment::Right
                                   : CellAlignment::None;
    return true;
}

}  // namespace

std::optional<std::vector<CellAlignment>> matchTableDelimiterRow(const QString& line,
                                                                 int columnCount) {
    const QStringList cells = splitTableRow(line);
    if (cells.size() != columnCount) return std::nullopt;
    std::vector<CellAlignment> alignments;
    for (const QString& cell : cells) {
        CellAlignment alignment = CellAlignment::None;
        if (!parseDelimiterCell(cell, &alignment)) return std::nullopt;
        alignments.push_back(alignment);
    }
    return alignments;
}

}  // namespace mdeditor::md
