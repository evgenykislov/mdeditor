#include "core/markdown_renderer.h"

#include <QtTest>

using mdeditor::documentTitle;
using mdeditor::markdownToHtml;
using mdeditor::MarkdownRenderOptions;
using mdeditor::RenderTarget;

// The Markdown converter.
class TestMarkdown : public QObject {
    Q_OBJECT

private slots:
    // What the Markdown text is turned into.
    void html_data();
    void html();

    // The examples of the specifications of CommonMark and of GitHub Flavored
    // Markdown.
    void specExamples_data();
    void specExamples();

    // The differences that the preview and the PDF export need.
    void richText_data();
    void richText();

    void title_data();
    void title();

    void emptyText();
    void plainTextOfLink();
    // Nothing makes the converter hang or crash.
    void hostileText_data();
    void hostileText();
};

void TestMarkdown::html_data() {
    QTest::addColumn<QString>("markdown");
    QTest::addColumn<QString>("html");

    QTest::newRow("heading of the first level")
        << QStringLiteral("# Заголовок") << QStringLiteral("<h1>Заголовок</h1>");
    QTest::newRow("heading with closing marks")
        << QStringLiteral("## Второй ##") << QStringLiteral("<h2>Второй</h2>");
    QTest::newRow("heading of the sixth level")
        << QStringLiteral("###### Шестой") << QStringLiteral("<h6>Шестой</h6>");
    QTest::newRow("seven marks are not a heading")
        << QStringLiteral("####### Семь решёток")
        << QStringLiteral("<p>####### Семь решёток</p>");
    QTest::newRow("mark without a space is not a heading")
        << QStringLiteral("#Решётка") << QStringLiteral("<p>#Решётка</p>");
    QTest::newRow("setext heading of the first level")
        << QStringLiteral("Заголовок\n===") << QStringLiteral("<h1>Заголовок</h1>");
    QTest::newRow("setext heading of the second level")
        << QStringLiteral("Подзаголовок\n---") << QStringLiteral("<h2>Подзаголовок</h2>");

    QTest::newRow("one paragraph of two lines")
        << QStringLiteral("Первая строка\nвторая строка")
        << QStringLiteral("<p>Первая строка\nвторая строка</p>");
    QTest::newRow("two paragraphs")
        << QStringLiteral("Первая.\n\nВторая.") << QStringLiteral("<p>Первая.</p>\n<p>Вторая.</p>");

    QTest::newRow("emphasis")
        << QStringLiteral("*курсив* **жирный** ***оба***")
        << QStringLiteral("<p><em>курсив</em> <strong>жирный</strong> "
                          "<em><strong>оба</strong></em></p>");
    QTest::newRow("emphasis with underscores")
        << QStringLiteral("_курсив_ __жирный__")
        << QStringLiteral("<p><em>курсив</em> <strong>жирный</strong></p>");
    QTest::newRow("emphasis inside a word")
        << QStringLiteral("под**черк**нутый")
        << QStringLiteral("<p>под<strong>черк</strong>нутый</p>");
    QTest::newRow("stars with spaces around them are not marks of emphasis")
        << QStringLiteral("5 * 3 = 15, 2 * 2 = 4")
        << QStringLiteral("<p>5 * 3 = 15, 2 * 2 = 4</p>");

    QTest::newRow("code span keeps HTML as text")
        << QStringLiteral("`<тег>`") << QStringLiteral("<p><code>&lt;тег&gt;</code></p>");
    QTest::newRow("code span with backticks inside")
        << QStringLiteral("`` `код` ``") << QStringLiteral("<p><code>`код`</code></p>");
    QTest::newRow("code span without a closing mark")
        << QStringLiteral("`нет пары") << QStringLiteral("<p>`нет пары</p>");
    QTest::newRow("code block with a language")
        << QStringLiteral("```cpp\nint x;\n```")
        << QStringLiteral("<pre><code class=\"language-cpp\">int x;\n</code></pre>");
    QTest::newRow("code block keeps HTML as text")
        << QStringLiteral("```\n<b>не жирный</b>\n```")
        << QStringLiteral("<pre><code>&lt;b&gt;не жирный&lt;/b&gt;\n</code></pre>");
    QTest::newRow("code block with tildes and blank lines")
        << QStringLiteral("~~~\nпервая\n\nвторая\n~~~")
        << QStringLiteral("<pre><code>первая\n\nвторая\n</code></pre>");
    QTest::newRow("indented code block")
        << QStringLiteral("    отступ") << QStringLiteral("<pre><code>отступ\n</code></pre>");
    QTest::newRow("nothing of the text is parsed inside a code block")
        << QStringLiteral("```\n# не заголовок\n*не курсив*\n```")
        << QStringLiteral("<pre><code># не заголовок\n*не курсив*\n</code></pre>");

    QTest::newRow("bulleted list")
        << QStringLiteral("- один\n- два") << QStringLiteral("<ul>\n<li>один</li>\n<li>два</li>\n</ul>");
    QTest::newRow("ordered list")
        << QStringLiteral("1. один\n2. два")
        << QStringLiteral("<ol>\n<li>один</li>\n<li>два</li>\n</ol>");
    QTest::newRow("ordered list does not begin with one")
        << QStringLiteral("3. три") << QStringLiteral("<ol start=\"3\">\n<li>три</li>\n</ol>");
    QTest::newRow("nested list")
        << QStringLiteral("- один\n  - вложенный\n- два")
        << QStringLiteral("<ul>\n<li>один\n<ul>\n<li>вложенный</li>\n</ul>\n</li>\n<li>два</li>\n</ul>");
    QTest::newRow("loose list has paragraphs")
        << QStringLiteral("- один\n\n- два")
        << QStringLiteral("<ul>\n<li>\n<p>один</p>\n</li>\n<li>\n<p>два</p>\n</li>\n</ul>");
    QTest::newRow("a text line continues the last item of a list")
        << QStringLiteral("1. один\n2. два\nне пункт")
        << QStringLiteral("<ol>\n<li>один</li>\n<li>два\nне пункт</li>\n</ol>");
    QTest::newRow("task list")
        << QStringLiteral("- [ ] купить хлеб\n- [x] сделать дело")
        << QStringLiteral("<ul>\n<li><input disabled=\"\" type=\"checkbox\"> купить хлеб</li>\n"
                          "<li><input checked=\"\" disabled=\"\" type=\"checkbox\"> сделать дело</li>\n</ul>");
    QTest::newRow("strikethrough")
        << QStringLiteral("~~удалено~~") << QStringLiteral("<p><del>удалено</del></p>");

    QTest::newRow("quotation")
        << QStringLiteral("> цитата") << QStringLiteral("<blockquote>\n<p>цитата</p>\n</blockquote>");
    QTest::newRow("quotation inside a quotation")
        << QStringLiteral("> внешняя\n> > внутренняя")
        << QStringLiteral("<blockquote>\n<p>внешняя</p>\n<blockquote>\n<p>внутренняя</p>\n"
                          "</blockquote>\n</blockquote>");
    QTest::newRow("thematic break") << QStringLiteral("---") << QStringLiteral("<hr>");
    QTest::newRow("thematic break of stars")
        << QStringLiteral("* * *") << QStringLiteral("<hr>");

    QTest::newRow("link with a title")
        << QStringLiteral("[текст](http://example.com/ \"Заголовок\")")
        << QStringLiteral("<p><a href=\"http://example.com/\" title=\"Заголовок\">текст</a></p>");
    QTest::newRow("address is encoded: a space and letters outside ASCII get codes")
        << QStringLiteral("[текст](</путь с пробелом>)")
        << QStringLiteral("<p><a href=\"/%D0%BF%D1%83%D1%82%D1%8C%20%D1%81%20%D0%BF%D1%80%D0"
                          "%BE%D0%B1%D0%B5%D0%BB%D0%BE%D0%BC\">текст</a></p>");
    QTest::newRow("link by a reference")
        << QStringLiteral("[текст][куда]\n\n[куда]: http://example.com \"Заголовок\"")
        << QStringLiteral("<p><a href=\"http://example.com\" title=\"Заголовок\">текст</a></p>");
    QTest::newRow("link by a short reference")
        << QStringLiteral("[текст]\n\n[текст]: /страница")
        << QStringLiteral("<p><a href=\"/%D1%81%D1%82%D1%80%D0%B0%D0%BD%D0%B8%D1%86%D0%B0\">текст</a></p>");
    QTest::newRow("reference without a definition stays as it is")
        << QStringLiteral("[текст]") << QStringLiteral("<p>[текст]</p>");
    QTest::newRow("the first definition of a label wins")
        << QStringLiteral("[текст]\n\n[текст]: /первый\n[текст]: /второй")
        << QStringLiteral("<p><a href=\"/%D0%BF%D0%B5%D1%80%D0%B2%D1%8B%D0%B9\">текст</a></p>");
    QTest::newRow("definition inside a code block is not a definition")
        << QStringLiteral("[текст]\n\n    [текст]: /внутри-кода")
        << QStringLiteral("<p>[текст]</p>\n<pre><code>[текст]: /внутри-кода\n</code></pre>");
    QTest::newRow("emphasis inside link text")
        << QStringLiteral("[*жирный* текст](/адрес)")
        << QStringLiteral("<p><a href=\"/%D0%B0%D0%B4%D1%80%D0%B5%D1%81\"><em>жирный</em> текст</a></p>");

    QTest::newRow("image with a title")
        << QStringLiteral("![описание](картина.png \"Название\")")
        << QStringLiteral("<p><img src=\"%D0%BA%D0%B0%D1%80%D1%82%D0%B8%D0%BD%D0%B0.png\" "
                          "alt=\"описание\" title=\"Название\"></p>");
    QTest::newRow("image by a reference")
        << QStringLiteral("![описание][рисунок]\n\n[рисунок]: a.png")
        << QStringLiteral("<p><img src=\"a.png\" alt=\"описание\"></p>");
    QTest::newRow("the alternate text of an image has no markup")
        << QStringLiteral("![*жирный* `код`](a.png)")
        << QStringLiteral("<p><img src=\"a.png\" alt=\"жирный код\"></p>");

    QTest::newRow("autolink in angle brackets")
        << QStringLiteral("<https://example.com>")
        << QStringLiteral("<p><a href=\"https://example.com\">https://example.com</a></p>");
    QTest::newRow("email autolink")
        << QStringLiteral("<user@example.com>")
        << QStringLiteral("<p><a href=\"mailto:user@example.com\">user@example.com</a></p>");
    QTest::newRow("bare address becomes a link")
        << QStringLiteral("Сайт: https://example.com/страница.")
        << QStringLiteral("<p>Сайт: <a href=\"https://example.com/%D1%81%D1%82%D1%80%D0%B0%D0"
                          "%BD%D0%B8%D1%86%D0%B0\">https://example.com/страница</a>.</p>");
    QTest::newRow("bare address with www")
        << QStringLiteral("(www.example.org)")
        << QStringLiteral("<p>(<a href=\"http://www.example.org\">www.example.org</a>)</p>");
    QTest::newRow("bare email address")
        << QStringLiteral("Пишите на user@example.com.")
        << QStringLiteral("<p>Пишите на <a href=\"mailto:user@example.com\">user@example.com</a>.</p>");

    QTest::newRow("named character references")
        << QStringLiteral("&copy; 2024 &amp; &mdash;")
        << QStringLiteral("<p>© 2024 &amp; —</p>");
    QTest::newRow("references of the HTML5 table")
        << QStringLiteral("&HilbertSpace; &ngE;") << QStringLiteral("<p>\u210B \u2267\u0338</p>");
    QTest::newRow("numeric character references")
        << QStringLiteral("&#169; &#x41;") << QStringLiteral("<p>© A</p>");
    QTest::newRow("a reference without a semicolon is text")
        << QStringLiteral("&copy") << QStringLiteral("<p>&amp;copy</p>");
    QTest::newRow("an unknown reference is text")
        << QStringLiteral("&MadeUpEntity;") << QStringLiteral("<p>&amp;MadeUpEntity;</p>");

    QTest::newRow("backslash escape")
        << QStringLiteral("\\*не курсив\\*") << QStringLiteral("<p>*не курсив*</p>");
    QTest::newRow("characters of HTML are escaped in the text")
        << QStringLiteral("a < b && c > d \"e\"")
        << QStringLiteral("<p>a &lt; b &amp;&amp; c &gt; d &quot;e&quot;</p>");
    QTest::newRow("raw HTML passes through")
        << QStringLiteral("<div class=\"x\">\n  <b>текст</b>\n</div>")
        << QStringLiteral("<div class=\"x\">\n  <b>текст</b>\n</div>");
    QTest::newRow("an opening bracket by itself is text")
        << QStringLiteral("[квадратная скобка") << QStringLiteral("<p>[квадратная скобка</p>");

    QTest::newRow("two spaces at the end of a line make a hard line break")
        << QStringLiteral("строка  \nвторая") << QStringLiteral("<p>строка<br>\nвторая</p>");
    QTest::newRow("backslash at the end of a line makes a hard line break")
        << QStringLiteral("строка\\\nвторая") << QStringLiteral("<p>строка<br>\nвторая</p>");

    QTest::newRow("table")
        << QStringLiteral("| Имя | Значение |\n| :--- | ---: |\n| a \\| b | **1** |\n| пусто | |")
        << QStringLiteral("<table>\n<thead>\n<tr>\n<th align=\"left\">Имя</th>\n"
                          "<th align=\"right\">Значение</th>\n</tr>\n</thead>\n<tbody>\n<tr>\n"
                          "<td align=\"left\">a | b</td>\n<td align=\"right\"><strong>1</strong></td>\n"
                          "</tr>\n<tr>\n<td align=\"left\">пусто</td>\n"
                          "<td align=\"right\"></td>\n</tr>\n</tbody>\n</table>");
    QTest::newRow("table with a pipe inside a code span")
        << QStringLiteral("| a |\n| - |\n| `x|y` |")
        << QStringLiteral("<table>\n<thead>\n<tr>\n<th>a</th>\n</tr>\n</thead>\n<tbody>\n<tr>\n"
                          "<td><code>x|y</code></td>\n</tr>\n</tbody>\n</table>");

    QTest::newRow("line breaks of all kinds")
        << QStringLiteral("# Заголовок\r\n\r\nТекст\rвторая строка")
        << QStringLiteral("<h1>Заголовок</h1>\n<p>Текст\nвторая строка</p>");
    QTest::newRow("emoji and other scripts")
        << QStringLiteral("Привет! こんにちは 🎉")
        << QStringLiteral("<p>Привет! こんにちは 🎉</p>");

    QTest::newRow("a document of everything")
        << QStringLiteral("# Отчёт\n\n**Важно.** Смотри [сайт](https://example.com).\n\n"
                          "- первый\n- второй\n\n> Цитата\n\n```python\nprint('Привет')\n```\n\n"
                          "| a | b |\n| - | - |\n| 1 | 2 |")
        << QStringLiteral("<h1>Отчёт</h1>\n<p><strong>Важно.</strong> Смотри "
                          "<a href=\"https://example.com\">сайт</a>.</p>\n<ul>\n<li>первый</li>\n"
                          "<li>второй</li>\n</ul>\n<blockquote>\n<p>Цитата</p>\n</blockquote>\n"
                          "<pre><code class=\"language-python\">print('Привет')\n</code></pre>\n"
                          "<table>\n<thead>\n<tr>\n<th>a</th>\n<th>b</th>\n</tr>\n</thead>\n"
                          "<tbody>\n<tr>\n<td>1</td>\n<td>2</td>\n</tr>\n</tbody>\n</table>");
}

void TestMarkdown::html() {
    QFETCH(QString, markdown);
    QFETCH(QString, html);
    QCOMPARE(markdownToHtml(markdown), html);
}

void TestMarkdown::specExamples_data() {
    QTest::addColumn<QString>("markdown");
    QTest::addColumn<QString>("html");
    // The rows are generated from the specifications.
    #include "spec_examples.inc"
}

void TestMarkdown::specExamples() {
    QFETCH(QString, markdown);
    QFETCH(QString, html);
    QCOMPARE(markdownToHtml(markdown), html);
}

void TestMarkdown::richText_data() {
    QTest::addColumn<QString>("markdown");
    QTest::addColumn<QString>("html");

    QTest::newRow("an open task has an empty box")
        << QStringLiteral("- [ ] купить хлеб")
        << QStringLiteral("<ul>\n<li>☐ купить хлеб</li>\n</ul>");
    QTest::newRow("a done task has a marked box")
        << QStringLiteral("- [x] сделать дело")
        << QStringLiteral("<ul>\n<li>☑ сделать дело</li>\n</ul>");
    QTest::newRow("the rest is the same")
        << QStringLiteral("# Заголовок\n\n*курсив* ~~зачёркнуто~~ [ссылка](/адрес)")
        << QStringLiteral("<h1>Заголовок</h1>\n<p><em>курсив</em> <del>зачёркнуто</del> "
                          "<a href=\"/%D0%B0%D0%B4%D1%80%D0%B5%D1%81\">ссылка</a></p>");
}

void TestMarkdown::richText() {
    QFETCH(QString, markdown);
    QFETCH(QString, html);
    QCOMPARE(markdownToHtml(markdown, MarkdownRenderOptions{RenderTarget::RichText}), html);
}

void TestMarkdown::title_data() {
    QTest::addColumn<QString>("markdown");
    QTest::addColumn<QString>("title");

    QTest::newRow("heading of the first level")
        << QStringLiteral("# Отчёт за год") << QStringLiteral("Отчёт за год");
    QTest::newRow("setext heading") << QStringLiteral("Отчёт\n===") << QStringLiteral("Отчёт");
    QTest::newRow("markup of the heading is not a part of the title")
        << QStringLiteral("# **Жирный** и `код` [ссылка](/адрес)")
        << QStringLiteral("Жирный и код ссылка");
    QTest::newRow("a heading of another level is not the title")
        << QStringLiteral("## Подзаголовок") << QStringLiteral("");
    QTest::newRow("a line of a code block is not the title")
        << QStringLiteral("```\n# не заголовок\n```") << QStringLiteral("");
    QTest::newRow("a document of paragraphs has no title")
        << QStringLiteral("Первый абзац\n\nВторой") << QStringLiteral("");
}

void TestMarkdown::title() {
    QFETCH(QString, markdown);
    QFETCH(QString, title);
    const QString fallback = QStringLiteral("Без названия");
    const QString expected = title.isEmpty() ? fallback : title;
    QCOMPARE(documentTitle(markdown, fallback), expected);
}

void TestMarkdown::emptyText() {
    QCOMPARE(markdownToHtml(QString()), QString());
    QCOMPARE(markdownToHtml(QStringLiteral("\n\n  \n")), QString());
    QCOMPARE(documentTitle(QString(), QStringLiteral("запасной")), QStringLiteral("запасной"));
}

void TestMarkdown::plainTextOfLink() {
    // The bare address and the address in angle brackets give the same link.
    const QString expected = QStringLiteral(
        "<p><a href=\"https://example.com\">https://example.com</a></p>");
    QCOMPARE(markdownToHtml(QStringLiteral("https://example.com")), expected);
    QCOMPARE(markdownToHtml(QStringLiteral("<https://example.com>")), expected);
}

void TestMarkdown::hostileText_data() {
    QTest::addColumn<QString>("markdown");

    QTest::newRow("many opening brackets") << QString(5000, QChar(u'['));
    QTest::newRow("many stars") << QString(5000, QChar(u'*'));
    QTest::newRow("many ampersands") << QString(5000, QChar(u'&'));
    QTest::newRow("many angle brackets") << QString(3000, QChar(u'<'));
    QTest::newRow("a long word") << QString(20000, QChar(u'x'));
    QTest::newRow("many short lines") << QStringLiteral("a\n").repeated(5000);
    QTest::newRow("an unclosed code block") << QStringLiteral("```\n") + QString(3000, QChar(u'`'));
    QTest::newRow("a lone surrogate") << QString(1, QChar(0xD800));
    QTest::newRow("a null character") << QString(1, QChar(0x0000));
}

void TestMarkdown::hostileText() {
    QFETCH(QString, markdown);
    QElapsedTimer timer;
    timer.start();
    const QString html = markdownToHtml(markdown);
    QVERIFY(timer.elapsed() < 2000);
    // Nothing of the text is lost: the result is not empty for a text that is not empty.
    QVERIFY(!html.isEmpty() || markdown.isEmpty());
}

QTEST_GUILESS_MAIN(TestMarkdown)
#include "tst_markdown.moc"
