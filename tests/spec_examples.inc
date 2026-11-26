// Generated from the examples of the CommonMark specification
// (https://spec.commonmark.org) and of the GitHub Flavored Markdown
// specification (https://github.github.com/gfm/). Both are published under
// the license CC BY-SA 4.0, and so is this file.
//
// Every row is one example: the Markdown text and the HTML that the
// specification expects of it. The notation of the specification is brought
// to the form that the program writes: <hr>, <br>, <img> and <input> have
// no slash before the closing angle bracket. The tabulation is written as
// \t, so that an editor cannot replace it with spaces by mistake.
//
// The examples where GitHub Flavored Markdown extends CommonMark with
// autolinks without angle brackets are left out: the program makes links of
// bare addresses.

//   the name of the row, the Markdown text, the HTML
    QTest::newRow("CommonMark: Tabs 1")
        << QString::fromUtf8("\tfoo\tbaz\t\tbim\n")
        << QString::fromUtf8("<pre><code>foo\tbaz\t\tbim\n</code></pre>");
    QTest::newRow("CommonMark: Tabs 2")
        << QString::fromUtf8("  \tfoo\tbaz\t\tbim\n")
        << QString::fromUtf8("<pre><code>foo\tbaz\t\tbim\n</code></pre>");
    QTest::newRow("CommonMark: Tabs 3")
        << QString::fromUtf8("    a\ta\n    ὐ\ta\n")
        << QString::fromUtf8("<pre><code>a\ta\nὐ\ta\n</code></pre>");
    QTest::newRow("CommonMark: Tabs 4")
        << QString::fromUtf8("  - foo\n\n\tbar\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>foo</p>\n<p>bar</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Tabs 5")
        << QString::fromUtf8("- foo\n\n\t\tbar\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>foo</p>\n<pre><code>  bar\n</code></pre>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Tabs 6")
        << QString::fromUtf8(">\t\tfoo\n")
        << QString::fromUtf8("<blockquote>\n<pre><code>  foo\n</code></pre>\n</blockquote>");
    QTest::newRow("CommonMark: Tabs 7")
        << QString::fromUtf8("-\t\tfoo\n")
        << QString::fromUtf8("<ul>\n<li>\n<pre><code>  foo\n</code></pre>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Tabs 8")
        << QString::fromUtf8("    foo\n\tbar\n")
        << QString::fromUtf8("<pre><code>foo\nbar\n</code></pre>");
    QTest::newRow("CommonMark: Tabs 9")
        << QString::fromUtf8(" - foo\n   - bar\n\t - baz\n")
        << QString::fromUtf8("<ul>\n<li>foo\n<ul>\n<li>bar\n<ul>\n<li>baz</li>\n</ul>\n</li>\n</ul>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Tabs 10")
        << QString::fromUtf8("#\tFoo\n")
        << QString::fromUtf8("<h1>Foo</h1>");
    QTest::newRow("CommonMark: Tabs 11")
        << QString::fromUtf8("*\t*\t*\t\n")
        << QString::fromUtf8("<hr>");
    QTest::newRow("CommonMark: Backslash escapes 1")
        << QString::fromUtf8("\\!\\\"\\#\\$\\%\\&\\'\\(\\)\\*\\+\\,\\-\\.\\/\\:\\;\\<\\=\\>\\\?\\@\\[\\\\\\]\\^\\_\\`\\{\\|\\}\\~\n")
        << QString::fromUtf8("<p>!&quot;#$%&amp;'()*+,-./:;&lt;=&gt;\?@[\\]^_`{|}~</p>");
    QTest::newRow("CommonMark: Backslash escapes 2")
        << QString::fromUtf8("\\\t\\A\\a\\ \\3\\φ\\«\n")
        << QString::fromUtf8("<p>\\\t\\A\\a\\ \\3\\φ\\«</p>");
    QTest::newRow("CommonMark: Backslash escapes 3")
        << QString::fromUtf8("\\*not emphasized*\n\\<br/> not a tag\n\\[not a link](/foo)\n\\`not code`\n1\\. not a list\n\\* not a list\n\\# not a heading\n\\[foo]: /url \"not a reference\"\n\\&ouml; not a character entity\n")
        << QString::fromUtf8("<p>*not emphasized*\n&lt;br/&gt; not a tag\n[not a link](/foo)\n`not code`\n1. not a list\n* not a list\n# not a heading\n[foo]: /url &quot;not a reference&quot;\n&amp;ouml; not a character entity</p>");
    QTest::newRow("CommonMark: not a heading 1")
        << QString::fromUtf8("\\\\*emphasis*\n")
        << QString::fromUtf8("<p>\\<em>emphasis</em></p>");
    QTest::newRow("CommonMark: not a heading 2")
        << QString::fromUtf8("foo\\\nbar\n")
        << QString::fromUtf8("<p>foo<br>\nbar</p>");
    QTest::newRow("CommonMark: not a heading 3")
        << QString::fromUtf8("`` \\[\\` ``\n")
        << QString::fromUtf8("<p><code>\\[\\`</code></p>");
    QTest::newRow("CommonMark: not a heading 4")
        << QString::fromUtf8("    \\[\\]\n")
        << QString::fromUtf8("<pre><code>\\[\\]\n</code></pre>");
    QTest::newRow("CommonMark: not a heading 5")
        << QString::fromUtf8("~~~\n\\[\\]\n~~~\n")
        << QString::fromUtf8("<pre><code>\\[\\]\n</code></pre>");
    QTest::newRow("CommonMark: not a heading 6")
        << QString::fromUtf8("<https://example.com\?find=\\*>\n")
        << QString::fromUtf8("<p><a href=\"https://example.com\?find=%5C*\">https://example.com\?find=\\*</a></p>");
    QTest::newRow("CommonMark: not a heading 7")
        << QString::fromUtf8("<a href=\"/bar\\/)\">\n")
        << QString::fromUtf8("<a href=\"/bar\\/)\">");
    QTest::newRow("CommonMark: not a heading 8")
        << QString::fromUtf8("[foo](/bar\\* \"ti\\*tle\")\n")
        << QString::fromUtf8("<p><a href=\"/bar*\" title=\"ti*tle\">foo</a></p>");
    QTest::newRow("CommonMark: not a heading 9")
        << QString::fromUtf8("[foo]\n\n[foo]: /bar\\* \"ti\\*tle\"\n")
        << QString::fromUtf8("<p><a href=\"/bar*\" title=\"ti*tle\">foo</a></p>");
    QTest::newRow("CommonMark: not a heading 10")
        << QString::fromUtf8("``` foo\\+bar\nfoo\n```\n")
        << QString::fromUtf8("<pre><code class=\"language-foo+bar\">foo\n</code></pre>");
    QTest::newRow("CommonMark: Entity and numeric character references 1")
        << QString::fromUtf8("&nbsp; &amp; &copy; &AElig; &Dcaron;\n&frac34; &HilbertSpace; &DifferentialD;\n&ClockwiseContourIntegral; &ngE;\n")
        << QString::fromUtf8("<p>  &amp; © Æ Ď\n¾ ℋ ⅆ\n∲ ≧̸</p>");
    QTest::newRow("CommonMark: Entity and numeric character references 2")
        << QString::fromUtf8("&#35; &#1234; &#992; &#0;\n")
        << QString::fromUtf8("<p># Ӓ Ϡ �</p>");
    QTest::newRow("CommonMark: Entity and numeric character references 3")
        << QString::fromUtf8("&#X22; &#XD06; &#xcab;\n")
        << QString::fromUtf8("<p>&quot; ആ ಫ</p>");
    QTest::newRow("CommonMark: Entity and numeric character references 4")
        << QString::fromUtf8("&nbsp &x; &#; &#x;\n&#87654321;\n&#abcdef0;\n&ThisIsNotDefined; &hi\?;\n")
        << QString::fromUtf8("<p>&amp;nbsp &amp;x; &amp;#; &amp;#x;\n&amp;#87654321;\n&amp;#abcdef0;\n&amp;ThisIsNotDefined; &amp;hi\?;</p>");
    QTest::newRow("CommonMark: Entity and numeric character references 5")
        << QString::fromUtf8("&copy\n")
        << QString::fromUtf8("<p>&amp;copy</p>");
    QTest::newRow("CommonMark: Entity and numeric character references 6")
        << QString::fromUtf8("&MadeUpEntity;\n")
        << QString::fromUtf8("<p>&amp;MadeUpEntity;</p>");
    QTest::newRow("CommonMark: Entity and numeric character references 7")
        << QString::fromUtf8("<a href=\"&ouml;&ouml;.html\">\n")
        << QString::fromUtf8("<a href=\"&ouml;&ouml;.html\">");
    QTest::newRow("CommonMark: Entity and numeric character references 8")
        << QString::fromUtf8("[foo](/f&ouml;&ouml; \"f&ouml;&ouml;\")\n")
        << QString::fromUtf8("<p><a href=\"/f%C3%B6%C3%B6\" title=\"föö\">foo</a></p>");
    QTest::newRow("CommonMark: Entity and numeric character references 9")
        << QString::fromUtf8("[foo]\n\n[foo]: /f&ouml;&ouml; \"f&ouml;&ouml;\"\n")
        << QString::fromUtf8("<p><a href=\"/f%C3%B6%C3%B6\" title=\"föö\">foo</a></p>");
    QTest::newRow("CommonMark: Entity and numeric character references 10")
        << QString::fromUtf8("``` f&ouml;&ouml;\nfoo\n```\n")
        << QString::fromUtf8("<pre><code class=\"language-föö\">foo\n</code></pre>");
    QTest::newRow("CommonMark: Entity and numeric character references 11")
        << QString::fromUtf8("`f&ouml;&ouml;`\n")
        << QString::fromUtf8("<p><code>f&amp;ouml;&amp;ouml;</code></p>");
    QTest::newRow("CommonMark: Entity and numeric character references 12")
        << QString::fromUtf8("    f&ouml;f&ouml;\n")
        << QString::fromUtf8("<pre><code>f&amp;ouml;f&amp;ouml;\n</code></pre>");
    QTest::newRow("CommonMark: Entity and numeric character references 13")
        << QString::fromUtf8("&#42;foo&#42;\n*foo*\n")
        << QString::fromUtf8("<p>*foo*\n<em>foo</em></p>");
    QTest::newRow("CommonMark: Entity and numeric character references 14")
        << QString::fromUtf8("&#42; foo\n\n* foo\n")
        << QString::fromUtf8("<p>* foo</p>\n<ul>\n<li>foo</li>\n</ul>");
    QTest::newRow("CommonMark: Entity and numeric character references 15")
        << QString::fromUtf8("foo&#10;&#10;bar\n")
        << QString::fromUtf8("<p>foo\n\nbar</p>");
    QTest::newRow("CommonMark: Entity and numeric character references 16")
        << QString::fromUtf8("&#9;foo\n")
        << QString::fromUtf8("<p>\tfoo</p>");
    QTest::newRow("CommonMark: Entity and numeric character references 17")
        << QString::fromUtf8("[a](url &quot;tit&quot;)\n")
        << QString::fromUtf8("<p>[a](url &quot;tit&quot;)</p>");
    QTest::newRow("CommonMark: Precedence 1")
        << QString::fromUtf8("- `one\n- two`\n")
        << QString::fromUtf8("<ul>\n<li>`one</li>\n<li>two`</li>\n</ul>");
    QTest::newRow("CommonMark: Thematic breaks 1")
        << QString::fromUtf8("***\n---\n___\n")
        << QString::fromUtf8("<hr>\n<hr>\n<hr>");
    QTest::newRow("CommonMark: Thematic breaks 2")
        << QString::fromUtf8("+++\n")
        << QString::fromUtf8("<p>+++</p>");
    QTest::newRow("CommonMark: Thematic breaks 3")
        << QString::fromUtf8("===\n")
        << QString::fromUtf8("<p>===</p>");
    QTest::newRow("CommonMark: Thematic breaks 4")
        << QString::fromUtf8("--\n**\n__\n")
        << QString::fromUtf8("<p>--\n**\n__</p>");
    QTest::newRow("CommonMark: Thematic breaks 5")
        << QString::fromUtf8(" ***\n  ***\n   ***\n")
        << QString::fromUtf8("<hr>\n<hr>\n<hr>");
    QTest::newRow("CommonMark: Thematic breaks 6")
        << QString::fromUtf8("    ***\n")
        << QString::fromUtf8("<pre><code>***\n</code></pre>");
    QTest::newRow("CommonMark: Thematic breaks 7")
        << QString::fromUtf8("Foo\n    ***\n")
        << QString::fromUtf8("<p>Foo\n***</p>");
    QTest::newRow("CommonMark: Thematic breaks 8")
        << QString::fromUtf8("_____________________________________\n")
        << QString::fromUtf8("<hr>");
    QTest::newRow("CommonMark: Thematic breaks 9")
        << QString::fromUtf8(" - - -\n")
        << QString::fromUtf8("<hr>");
    QTest::newRow("CommonMark: Thematic breaks 10")
        << QString::fromUtf8(" **  * ** * ** * **\n")
        << QString::fromUtf8("<hr>");
    QTest::newRow("CommonMark: Thematic breaks 11")
        << QString::fromUtf8("-     -      -      -\n")
        << QString::fromUtf8("<hr>");
    QTest::newRow("CommonMark: Thematic breaks 12")
        << QString::fromUtf8("- - - -    \n")
        << QString::fromUtf8("<hr>");
    QTest::newRow("CommonMark: Thematic breaks 13")
        << QString::fromUtf8("_ _ _ _ a\n\na------\n\n---a---\n")
        << QString::fromUtf8("<p>_ _ _ _ a</p>\n<p>a------</p>\n<p>---a---</p>");
    QTest::newRow("CommonMark: Thematic breaks 14")
        << QString::fromUtf8(" *-*\n")
        << QString::fromUtf8("<p><em>-</em></p>");
    QTest::newRow("CommonMark: Thematic breaks 15")
        << QString::fromUtf8("- foo\n***\n- bar\n")
        << QString::fromUtf8("<ul>\n<li>foo</li>\n</ul>\n<hr>\n<ul>\n<li>bar</li>\n</ul>");
    QTest::newRow("CommonMark: Thematic breaks 16")
        << QString::fromUtf8("Foo\n***\nbar\n")
        << QString::fromUtf8("<p>Foo</p>\n<hr>\n<p>bar</p>");
    QTest::newRow("CommonMark: Thematic breaks 17")
        << QString::fromUtf8("Foo\n---\nbar\n")
        << QString::fromUtf8("<h2>Foo</h2>\n<p>bar</p>");
    QTest::newRow("CommonMark: Thematic breaks 18")
        << QString::fromUtf8("* Foo\n* * *\n* Bar\n")
        << QString::fromUtf8("<ul>\n<li>Foo</li>\n</ul>\n<hr>\n<ul>\n<li>Bar</li>\n</ul>");
    QTest::newRow("CommonMark: Thematic breaks 19")
        << QString::fromUtf8("- Foo\n- * * *\n")
        << QString::fromUtf8("<ul>\n<li>Foo</li>\n<li>\n<hr>\n</li>\n</ul>");
    QTest::newRow("CommonMark: ATX headings 1")
        << QString::fromUtf8("# foo\n## foo\n### foo\n#### foo\n##### foo\n###### foo\n")
        << QString::fromUtf8("<h1>foo</h1>\n<h2>foo</h2>\n<h3>foo</h3>\n<h4>foo</h4>\n<h5>foo</h5>\n<h6>foo</h6>");
    QTest::newRow("CommonMark: foo 1")
        << QString::fromUtf8("####### foo\n")
        << QString::fromUtf8("<p>####### foo</p>");
    QTest::newRow("CommonMark: foo 2")
        << QString::fromUtf8("#5 bolt\n\n#hashtag\n")
        << QString::fromUtf8("<p>#5 bolt</p>\n<p>#hashtag</p>");
    QTest::newRow("CommonMark: foo 3")
        << QString::fromUtf8("\\## foo\n")
        << QString::fromUtf8("<p>## foo</p>");
    QTest::newRow("CommonMark: foo 4")
        << QString::fromUtf8("# foo *bar* \\*baz\\*\n")
        << QString::fromUtf8("<h1>foo <em>bar</em> *baz*</h1>");
    QTest::newRow("CommonMark: foo *bar* \\*baz\\* 1")
        << QString::fromUtf8("#                  foo                     \n")
        << QString::fromUtf8("<h1>foo</h1>");
    QTest::newRow("CommonMark: foo 5")
        << QString::fromUtf8(" ### foo\n  ## foo\n   # foo\n")
        << QString::fromUtf8("<h3>foo</h3>\n<h2>foo</h2>\n<h1>foo</h1>");
    QTest::newRow("CommonMark: foo 6")
        << QString::fromUtf8("    # foo\n")
        << QString::fromUtf8("<pre><code># foo\n</code></pre>");
    QTest::newRow("CommonMark: foo 7")
        << QString::fromUtf8("foo\n    # bar\n")
        << QString::fromUtf8("<p>foo\n# bar</p>");
    QTest::newRow("CommonMark: bar</p> 1")
        << QString::fromUtf8("## foo ##\n  ###   bar    ###\n")
        << QString::fromUtf8("<h2>foo</h2>\n<h3>bar</h3>");
    QTest::newRow("CommonMark: foo ## 1")
        << QString::fromUtf8("# foo ##################################\n##### foo ##\n")
        << QString::fromUtf8("<h1>foo</h1>\n<h5>foo</h5>");
    QTest::newRow("CommonMark: foo ## 2")
        << QString::fromUtf8("### foo ###     \n")
        << QString::fromUtf8("<h3>foo</h3>");
    QTest::newRow("CommonMark: foo ### 1")
        << QString::fromUtf8("### foo ### b\n")
        << QString::fromUtf8("<h3>foo ### b</h3>");
    QTest::newRow("CommonMark: foo ### b 1")
        << QString::fromUtf8("# foo#\n")
        << QString::fromUtf8("<h1>foo#</h1>");
    QTest::newRow("CommonMark: foo# 1")
        << QString::fromUtf8("### foo \\###\n## foo #\\##\n# foo \\#\n")
        << QString::fromUtf8("<h3>foo ###</h3>\n<h2>foo ###</h2>\n<h1>foo #</h1>");
    QTest::newRow("CommonMark: foo \\# 1")
        << QString::fromUtf8("****\n## foo\n****\n")
        << QString::fromUtf8("<hr>\n<h2>foo</h2>\n<hr>");
    QTest::newRow("CommonMark: foo 8")
        << QString::fromUtf8("Foo bar\n# baz\nBar foo\n")
        << QString::fromUtf8("<p>Foo bar</p>\n<h1>baz</h1>\n<p>Bar foo</p>");
    QTest::newRow("CommonMark: baz 1")
        << QString::fromUtf8("## \n#\n### ###\n")
        << QString::fromUtf8("<h2></h2>\n<h1></h1>\n<h3></h3>");
    QTest::newRow("CommonMark: Setext headings 1")
        << QString::fromUtf8("Foo *bar*\n=========\n\nFoo *bar*\n---------\n")
        << QString::fromUtf8("<h1>Foo <em>bar</em></h1>\n<h2>Foo <em>bar</em></h2>");
    QTest::newRow("CommonMark: Setext headings 2")
        << QString::fromUtf8("Foo *bar\nbaz*\n====\n")
        << QString::fromUtf8("<h1>Foo <em>bar\nbaz</em></h1>");
    QTest::newRow("CommonMark: Setext headings 3")
        << QString::fromUtf8("  Foo *bar\nbaz*\t\n====\n")
        << QString::fromUtf8("<h1>Foo <em>bar\nbaz</em></h1>");
    QTest::newRow("CommonMark: Setext headings 4")
        << QString::fromUtf8("Foo\n-------------------------\n\nFoo\n=\n")
        << QString::fromUtf8("<h2>Foo</h2>\n<h1>Foo</h1>");
    QTest::newRow("CommonMark: Setext headings 5")
        << QString::fromUtf8("   Foo\n---\n\n  Foo\n-----\n\n  Foo\n  ===\n")
        << QString::fromUtf8("<h2>Foo</h2>\n<h2>Foo</h2>\n<h1>Foo</h1>");
    QTest::newRow("CommonMark: Setext headings 6")
        << QString::fromUtf8("    Foo\n    ---\n\n    Foo\n---\n")
        << QString::fromUtf8("<pre><code>Foo\n---\n\nFoo\n</code></pre>\n<hr>");
    QTest::newRow("CommonMark: Setext headings 7")
        << QString::fromUtf8("Foo\n   ----      \n")
        << QString::fromUtf8("<h2>Foo</h2>");
    QTest::newRow("CommonMark: Setext headings 8")
        << QString::fromUtf8("Foo\n    ---\n")
        << QString::fromUtf8("<p>Foo\n---</p>");
    QTest::newRow("CommonMark: Setext headings 9")
        << QString::fromUtf8("Foo\n= =\n\nFoo\n--- -\n")
        << QString::fromUtf8("<p>Foo\n= =</p>\n<p>Foo</p>\n<hr>");
    QTest::newRow("CommonMark: Setext headings 10")
        << QString::fromUtf8("Foo  \n-----\n")
        << QString::fromUtf8("<h2>Foo</h2>");
    QTest::newRow("CommonMark: Setext headings 11")
        << QString::fromUtf8("Foo\\\n----\n")
        << QString::fromUtf8("<h2>Foo\\</h2>");
    QTest::newRow("CommonMark: Setext headings 12")
        << QString::fromUtf8("`Foo\n----\n`\n\n<a title=\"a lot\n---\nof dashes\"/>\n")
        << QString::fromUtf8("<h2>`Foo</h2>\n<p>`</p>\n<h2>&lt;a title=&quot;a lot</h2>\n<p>of dashes&quot;/&gt;</p>");
    QTest::newRow("CommonMark: Setext headings 13")
        << QString::fromUtf8("> Foo\n---\n")
        << QString::fromUtf8("<blockquote>\n<p>Foo</p>\n</blockquote>\n<hr>");
    QTest::newRow("CommonMark: Setext headings 14")
        << QString::fromUtf8("> foo\nbar\n===\n")
        << QString::fromUtf8("<blockquote>\n<p>foo\nbar\n===</p>\n</blockquote>");
    QTest::newRow("CommonMark: Setext headings 15")
        << QString::fromUtf8("- Foo\n---\n")
        << QString::fromUtf8("<ul>\n<li>Foo</li>\n</ul>\n<hr>");
    QTest::newRow("CommonMark: Setext headings 16")
        << QString::fromUtf8("Foo\nBar\n---\n")
        << QString::fromUtf8("<h2>Foo\nBar</h2>");
    QTest::newRow("CommonMark: Setext headings 17")
        << QString::fromUtf8("---\nFoo\n---\nBar\n---\nBaz\n")
        << QString::fromUtf8("<hr>\n<h2>Foo</h2>\n<h2>Bar</h2>\n<p>Baz</p>");
    QTest::newRow("CommonMark: Setext headings 18")
        << QString::fromUtf8("\n====\n")
        << QString::fromUtf8("<p>====</p>");
    QTest::newRow("CommonMark: Setext headings 19")
        << QString::fromUtf8("---\n---\n")
        << QString::fromUtf8("<hr>\n<hr>");
    QTest::newRow("CommonMark: Setext headings 20")
        << QString::fromUtf8("- foo\n-----\n")
        << QString::fromUtf8("<ul>\n<li>foo</li>\n</ul>\n<hr>");
    QTest::newRow("CommonMark: Setext headings 21")
        << QString::fromUtf8("    foo\n---\n")
        << QString::fromUtf8("<pre><code>foo\n</code></pre>\n<hr>");
    QTest::newRow("CommonMark: Setext headings 22")
        << QString::fromUtf8("> foo\n-----\n")
        << QString::fromUtf8("<blockquote>\n<p>foo</p>\n</blockquote>\n<hr>");
    QTest::newRow("CommonMark: Setext headings 23")
        << QString::fromUtf8("\\> foo\n------\n")
        << QString::fromUtf8("<h2>&gt; foo</h2>");
    QTest::newRow("CommonMark: Setext headings 24")
        << QString::fromUtf8("Foo\n\nbar\n---\nbaz\n")
        << QString::fromUtf8("<p>Foo</p>\n<h2>bar</h2>\n<p>baz</p>");
    QTest::newRow("CommonMark: Setext headings 25")
        << QString::fromUtf8("Foo\nbar\n\n---\n\nbaz\n")
        << QString::fromUtf8("<p>Foo\nbar</p>\n<hr>\n<p>baz</p>");
    QTest::newRow("CommonMark: Setext headings 26")
        << QString::fromUtf8("Foo\nbar\n* * *\nbaz\n")
        << QString::fromUtf8("<p>Foo\nbar</p>\n<hr>\n<p>baz</p>");
    QTest::newRow("CommonMark: Setext headings 27")
        << QString::fromUtf8("Foo\nbar\n\\---\nbaz\n")
        << QString::fromUtf8("<p>Foo\nbar\n---\nbaz</p>");
    QTest::newRow("CommonMark: Indented code blocks 1")
        << QString::fromUtf8("    a simple\n      indented code block\n")
        << QString::fromUtf8("<pre><code>a simple\n  indented code block\n</code></pre>");
    QTest::newRow("CommonMark: Indented code blocks 2")
        << QString::fromUtf8("  - foo\n\n    bar\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>foo</p>\n<p>bar</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Indented code blocks 3")
        << QString::fromUtf8("1.  foo\n\n    - bar\n")
        << QString::fromUtf8("<ol>\n<li>\n<p>foo</p>\n<ul>\n<li>bar</li>\n</ul>\n</li>\n</ol>");
    QTest::newRow("CommonMark: Indented code blocks 4")
        << QString::fromUtf8("    <a/>\n    *hi*\n\n    - one\n")
        << QString::fromUtf8("<pre><code>&lt;a/&gt;\n*hi*\n\n- one\n</code></pre>");
    QTest::newRow("CommonMark: Indented code blocks 5")
        << QString::fromUtf8("    chunk1\n\n    chunk2\n  \n \n \n    chunk3\n")
        << QString::fromUtf8("<pre><code>chunk1\n\nchunk2\n\n\n\nchunk3\n</code></pre>");
    QTest::newRow("CommonMark: Indented code blocks 6")
        << QString::fromUtf8("    chunk1\n      \n      chunk2\n")
        << QString::fromUtf8("<pre><code>chunk1\n  \n  chunk2\n</code></pre>");
    QTest::newRow("CommonMark: Indented code blocks 7")
        << QString::fromUtf8("Foo\n    bar\n\n")
        << QString::fromUtf8("<p>Foo\nbar</p>");
    QTest::newRow("CommonMark: Indented code blocks 8")
        << QString::fromUtf8("    foo\nbar\n")
        << QString::fromUtf8("<pre><code>foo\n</code></pre>\n<p>bar</p>");
    QTest::newRow("CommonMark: Indented code blocks 9")
        << QString::fromUtf8("# Heading\n    foo\nHeading\n------\n    foo\n----\n")
        << QString::fromUtf8("<h1>Heading</h1>\n<pre><code>foo\n</code></pre>\n<h2>Heading</h2>\n<pre><code>foo\n</code></pre>\n<hr>");
    QTest::newRow("CommonMark: Heading 1")
        << QString::fromUtf8("        foo\n    bar\n")
        << QString::fromUtf8("<pre><code>    foo\nbar\n</code></pre>");
    QTest::newRow("CommonMark: Heading 2")
        << QString::fromUtf8("\n    \n    foo\n    \n\n")
        << QString::fromUtf8("<pre><code>foo\n</code></pre>");
    QTest::newRow("CommonMark: Heading 3")
        << QString::fromUtf8("    foo  \n")
        << QString::fromUtf8("<pre><code>foo  \n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 1")
        << QString::fromUtf8("```\n<\n >\n```\n")
        << QString::fromUtf8("<pre><code>&lt;\n &gt;\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 2")
        << QString::fromUtf8("~~~\n<\n >\n~~~\n")
        << QString::fromUtf8("<pre><code>&lt;\n &gt;\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 3")
        << QString::fromUtf8("``\nfoo\n``\n")
        << QString::fromUtf8("<p><code>foo</code></p>");
    QTest::newRow("CommonMark: Fenced code blocks 4")
        << QString::fromUtf8("```\naaa\n~~~\n```\n")
        << QString::fromUtf8("<pre><code>aaa\n~~~\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 5")
        << QString::fromUtf8("~~~\naaa\n```\n~~~\n")
        << QString::fromUtf8("<pre><code>aaa\n```\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 6")
        << QString::fromUtf8("````\naaa\n```\n``````\n")
        << QString::fromUtf8("<pre><code>aaa\n```\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 7")
        << QString::fromUtf8("~~~~\naaa\n~~~\n~~~~\n")
        << QString::fromUtf8("<pre><code>aaa\n~~~\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 8")
        << QString::fromUtf8("```\n")
        << QString::fromUtf8("<pre><code></code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 9")
        << QString::fromUtf8("`````\n\n```\naaa\n")
        << QString::fromUtf8("<pre><code>\n```\naaa\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 10")
        << QString::fromUtf8("> ```\n> aaa\n\nbbb\n")
        << QString::fromUtf8("<blockquote>\n<pre><code>aaa\n</code></pre>\n</blockquote>\n<p>bbb</p>");
    QTest::newRow("CommonMark: Fenced code blocks 11")
        << QString::fromUtf8("```\n\n  \n```\n")
        << QString::fromUtf8("<pre><code>\n  \n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 12")
        << QString::fromUtf8("```\n```\n")
        << QString::fromUtf8("<pre><code></code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 13")
        << QString::fromUtf8(" ```\n aaa\naaa\n```\n")
        << QString::fromUtf8("<pre><code>aaa\naaa\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 14")
        << QString::fromUtf8("  ```\naaa\n  aaa\naaa\n  ```\n")
        << QString::fromUtf8("<pre><code>aaa\naaa\naaa\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 15")
        << QString::fromUtf8("   ```\n   aaa\n    aaa\n  aaa\n   ```\n")
        << QString::fromUtf8("<pre><code>aaa\n aaa\naaa\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 16")
        << QString::fromUtf8("    ```\n    aaa\n    ```\n")
        << QString::fromUtf8("<pre><code>```\naaa\n```\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 17")
        << QString::fromUtf8("```\naaa\n  ```\n")
        << QString::fromUtf8("<pre><code>aaa\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 18")
        << QString::fromUtf8("   ```\naaa\n  ```\n")
        << QString::fromUtf8("<pre><code>aaa\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 19")
        << QString::fromUtf8("```\naaa\n    ```\n")
        << QString::fromUtf8("<pre><code>aaa\n    ```\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 20")
        << QString::fromUtf8("``` ```\naaa\n")
        << QString::fromUtf8("<p><code> </code>\naaa</p>");
    QTest::newRow("CommonMark: Fenced code blocks 21")
        << QString::fromUtf8("~~~~~~\naaa\n~~~ ~~\n")
        << QString::fromUtf8("<pre><code>aaa\n~~~ ~~\n</code></pre>");
    QTest::newRow("CommonMark: Fenced code blocks 22")
        << QString::fromUtf8("foo\n```\nbar\n```\nbaz\n")
        << QString::fromUtf8("<p>foo</p>\n<pre><code>bar\n</code></pre>\n<p>baz</p>");
    QTest::newRow("CommonMark: Fenced code blocks 23")
        << QString::fromUtf8("foo\n---\n~~~\nbar\n~~~\n# baz\n")
        << QString::fromUtf8("<h2>foo</h2>\n<pre><code>bar\n</code></pre>\n<h1>baz</h1>");
    QTest::newRow("CommonMark: baz 2")
        << QString::fromUtf8("```ruby\ndef foo(x)\n  return 3\nend\n```\n")
        << QString::fromUtf8("<pre><code class=\"language-ruby\">def foo(x)\n  return 3\nend\n</code></pre>");
    QTest::newRow("CommonMark: baz 3")
        << QString::fromUtf8("~~~~    ruby startline=3 $%@#$\ndef foo(x)\n  return 3\nend\n~~~~~~~\n")
        << QString::fromUtf8("<pre><code class=\"language-ruby\">def foo(x)\n  return 3\nend\n</code></pre>");
    QTest::newRow("CommonMark: baz 4")
        << QString::fromUtf8("````;\n````\n")
        << QString::fromUtf8("<pre><code class=\"language-;\"></code></pre>");
    QTest::newRow("CommonMark: baz 5")
        << QString::fromUtf8("``` aa ```\nfoo\n")
        << QString::fromUtf8("<p><code>aa</code>\nfoo</p>");
    QTest::newRow("CommonMark: baz 6")
        << QString::fromUtf8("~~~ aa ``` ~~~\nfoo\n~~~\n")
        << QString::fromUtf8("<pre><code class=\"language-aa\">foo\n</code></pre>");
    QTest::newRow("CommonMark: baz 7")
        << QString::fromUtf8("```\n``` aaa\n```\n")
        << QString::fromUtf8("<pre><code>``` aaa\n</code></pre>");
    QTest::newRow("CommonMark: HTML blocks 1")
        << QString::fromUtf8("<table><tr><td>\n<pre>\n**Hello**,\n\n_world_.\n</pre>\n</td></tr></table>\n")
        << QString::fromUtf8("<table><tr><td>\n<pre>\n**Hello**,\n<p><em>world</em>.\n</pre></p>\n</td></tr></table>");
    QTest::newRow("CommonMark: HTML blocks 2")
        << QString::fromUtf8("<table>\n  <tr>\n    <td>\n           hi\n    </td>\n  </tr>\n</table>\n\nokay.\n")
        << QString::fromUtf8("<table>\n  <tr>\n    <td>\n           hi\n    </td>\n  </tr>\n</table>\n<p>okay.</p>");
    QTest::newRow("CommonMark: HTML blocks 3")
        << QString::fromUtf8(" <div>\n  *hello*\n         <foo><a>\n")
        << QString::fromUtf8(" <div>\n  *hello*\n         <foo><a>");
    QTest::newRow("CommonMark: HTML blocks 4")
        << QString::fromUtf8("</div>\n*foo*\n")
        << QString::fromUtf8("</div>\n*foo*");
    QTest::newRow("CommonMark: HTML blocks 5")
        << QString::fromUtf8("<DIV CLASS=\"foo\">\n\n*Markdown*\n\n</DIV>\n")
        << QString::fromUtf8("<DIV CLASS=\"foo\">\n<p><em>Markdown</em></p>\n</DIV>");
    QTest::newRow("CommonMark: HTML blocks 6")
        << QString::fromUtf8("<div id=\"foo\"\n  class=\"bar\">\n</div>\n")
        << QString::fromUtf8("<div id=\"foo\"\n  class=\"bar\">\n</div>");
    QTest::newRow("CommonMark: HTML blocks 7")
        << QString::fromUtf8("<div id=\"foo\" class=\"bar\n  baz\">\n</div>\n")
        << QString::fromUtf8("<div id=\"foo\" class=\"bar\n  baz\">\n</div>");
    QTest::newRow("CommonMark: HTML blocks 8")
        << QString::fromUtf8("<div>\n*foo*\n\n*bar*\n")
        << QString::fromUtf8("<div>\n*foo*\n<p><em>bar</em></p>");
    QTest::newRow("CommonMark: HTML blocks 9")
        << QString::fromUtf8("<div id=\"foo\"\n*hi*\n")
        << QString::fromUtf8("<div id=\"foo\"\n*hi*");
    QTest::newRow("CommonMark: HTML blocks 10")
        << QString::fromUtf8("<div class\nfoo\n")
        << QString::fromUtf8("<div class\nfoo");
    QTest::newRow("CommonMark: HTML blocks 11")
        << QString::fromUtf8("<div *\?\?\?-&&&-<---\n*foo*\n")
        << QString::fromUtf8("<div *\?\?\?-&&&-<---\n*foo*");
    QTest::newRow("CommonMark: HTML blocks 12")
        << QString::fromUtf8("<div><a href=\"bar\">*foo*</a></div>\n")
        << QString::fromUtf8("<div><a href=\"bar\">*foo*</a></div>");
    QTest::newRow("CommonMark: HTML blocks 13")
        << QString::fromUtf8("<table><tr><td>\nfoo\n</td></tr></table>\n")
        << QString::fromUtf8("<table><tr><td>\nfoo\n</td></tr></table>");
    QTest::newRow("CommonMark: HTML blocks 14")
        << QString::fromUtf8("<div></div>\n``` c\nint x = 33;\n```\n")
        << QString::fromUtf8("<div></div>\n``` c\nint x = 33;\n```");
    QTest::newRow("CommonMark: HTML blocks 15")
        << QString::fromUtf8("<div\n> not quoted text\n")
        << QString::fromUtf8("<div\n> not quoted text");
    QTest::newRow("CommonMark: HTML blocks 16")
        << QString::fromUtf8("<a href=\"foo\">\n*bar*\n</a>\n")
        << QString::fromUtf8("<a href=\"foo\">\n*bar*\n</a>");
    QTest::newRow("CommonMark: HTML blocks 17")
        << QString::fromUtf8("<Warning>\n*bar*\n</Warning>\n")
        << QString::fromUtf8("<Warning>\n*bar*\n</Warning>");
    QTest::newRow("CommonMark: HTML blocks 18")
        << QString::fromUtf8("<i class=\"foo\">\n*bar*\n</i>\n")
        << QString::fromUtf8("<i class=\"foo\">\n*bar*\n</i>");
    QTest::newRow("CommonMark: HTML blocks 19")
        << QString::fromUtf8("</ins>\n*bar*\n")
        << QString::fromUtf8("</ins>\n*bar*");
    QTest::newRow("CommonMark: HTML blocks 20")
        << QString::fromUtf8("<del>\n*foo*\n</del>\n")
        << QString::fromUtf8("<del>\n*foo*\n</del>");
    QTest::newRow("CommonMark: HTML blocks 21")
        << QString::fromUtf8("<del>\n\n*foo*\n\n</del>\n")
        << QString::fromUtf8("<del>\n<p><em>foo</em></p>\n</del>");
    QTest::newRow("CommonMark: HTML blocks 22")
        << QString::fromUtf8("<del>*foo*</del>\n")
        << QString::fromUtf8("<p><del><em>foo</em></del></p>");
    QTest::newRow("CommonMark: HTML blocks 23")
        << QString::fromUtf8("<del\nclass=\"foo\">\n*foo*\n</del>\n")
        << QString::fromUtf8("<p><del\nclass=\"foo\">\n<em>foo</em>\n</del></p>");
    QTest::newRow("CommonMark: HTML blocks 24")
        << QString::fromUtf8("<pre language=\"haskell\"><code>\nimport Text.HTML.TagSoup\n\nmain :: IO ()\nmain = print $ parseTags tags\n</code></pre>\nokay\n")
        << QString::fromUtf8("<pre language=\"haskell\"><code>\nimport Text.HTML.TagSoup\n\nmain :: IO ()\nmain = print $ parseTags tags\n</code></pre>\n<p>okay</p>");
    QTest::newRow("CommonMark: HTML blocks 25")
        << QString::fromUtf8("<script type=\"text/javascript\">\n// JavaScript example\n\ndocument.getElementById(\"demo\").innerHTML = \"Hello JavaScript!\";\n</script>\nokay\n")
        << QString::fromUtf8("<script type=\"text/javascript\">\n// JavaScript example\n\ndocument.getElementById(\"demo\").innerHTML = \"Hello JavaScript!\";\n</script>\n<p>okay</p>");
    QTest::newRow("CommonMark: HTML blocks 26")
        << QString::fromUtf8("<textarea>\n\n*foo*\n\n_bar_\n\n</textarea>\n")
        << QString::fromUtf8("<textarea>\n\n*foo*\n\n_bar_\n\n</textarea>");
    QTest::newRow("CommonMark: HTML blocks 27")
        << QString::fromUtf8("<style\n  type=\"text/css\">\nh1 {color:red;}\n\np {color:blue;}\n</style>\nokay\n")
        << QString::fromUtf8("<style\n  type=\"text/css\">\nh1 {color:red;}\n\np {color:blue;}\n</style>\n<p>okay</p>");
    QTest::newRow("CommonMark: HTML blocks 28")
        << QString::fromUtf8("<style\n  type=\"text/css\">\n\nfoo\n")
        << QString::fromUtf8("<style\n  type=\"text/css\">\n\nfoo");
    QTest::newRow("CommonMark: HTML blocks 29")
        << QString::fromUtf8("> <div>\n> foo\n\nbar\n")
        << QString::fromUtf8("<blockquote>\n<div>\nfoo\n</blockquote>\n<p>bar</p>");
    QTest::newRow("CommonMark: HTML blocks 30")
        << QString::fromUtf8("- <div>\n- foo\n")
        << QString::fromUtf8("<ul>\n<li>\n<div>\n</li>\n<li>foo</li>\n</ul>");
    QTest::newRow("CommonMark: HTML blocks 31")
        << QString::fromUtf8("<style>p{color:red;}</style>\n*foo*\n")
        << QString::fromUtf8("<style>p{color:red;}</style>\n<p><em>foo</em></p>");
    QTest::newRow("CommonMark: HTML blocks 32")
        << QString::fromUtf8("<!-- foo -->*bar*\n*baz*\n")
        << QString::fromUtf8("<!-- foo -->*bar*\n<p><em>baz</em></p>");
    QTest::newRow("CommonMark: HTML blocks 33")
        << QString::fromUtf8("<script>\nfoo\n</script>1. *bar*\n")
        << QString::fromUtf8("<script>\nfoo\n</script>1. *bar*");
    QTest::newRow("CommonMark: HTML blocks 34")
        << QString::fromUtf8("<!-- Foo\n\nbar\n   baz -->\nokay\n")
        << QString::fromUtf8("<!-- Foo\n\nbar\n   baz -->\n<p>okay</p>");
    QTest::newRow("CommonMark: HTML blocks 35")
        << QString::fromUtf8("<\?php\n\n  echo '>';\n\n\?>\nokay\n")
        << QString::fromUtf8("<\?php\n\n  echo '>';\n\n\?>\n<p>okay</p>");
    QTest::newRow("CommonMark: HTML blocks 36")
        << QString::fromUtf8("<!DOCTYPE html>\n")
        << QString::fromUtf8("<!DOCTYPE html>");
    QTest::newRow("CommonMark: HTML blocks 37")
        << QString::fromUtf8("<![CDATA[\nfunction matchwo(a,b)\n{\n  if (a < b && a < 0) then {\n    return 1;\n\n  } else {\n\n    return 0;\n  }\n}\n]]>\nokay\n")
        << QString::fromUtf8("<![CDATA[\nfunction matchwo(a,b)\n{\n  if (a < b && a < 0) then {\n    return 1;\n\n  } else {\n\n    return 0;\n  }\n}\n]]>\n<p>okay</p>");
    QTest::newRow("CommonMark: HTML blocks 38")
        << QString::fromUtf8("  <!-- foo -->\n\n    <!-- foo -->\n")
        << QString::fromUtf8("  <!-- foo -->\n<pre><code>&lt;!-- foo --&gt;\n</code></pre>");
    QTest::newRow("CommonMark: HTML blocks 39")
        << QString::fromUtf8("  <div>\n\n    <div>\n")
        << QString::fromUtf8("  <div>\n<pre><code>&lt;div&gt;\n</code></pre>");
    QTest::newRow("CommonMark: HTML blocks 40")
        << QString::fromUtf8("Foo\n<div>\nbar\n</div>\n")
        << QString::fromUtf8("<p>Foo</p>\n<div>\nbar\n</div>");
    QTest::newRow("CommonMark: HTML blocks 41")
        << QString::fromUtf8("<div>\nbar\n</div>\n*foo*\n")
        << QString::fromUtf8("<div>\nbar\n</div>\n*foo*");
    QTest::newRow("CommonMark: HTML blocks 42")
        << QString::fromUtf8("Foo\n<a href=\"bar\">\nbaz\n")
        << QString::fromUtf8("<p>Foo\n<a href=\"bar\">\nbaz</p>");
    QTest::newRow("CommonMark: HTML blocks 43")
        << QString::fromUtf8("<div>\n\n*Emphasized* text.\n\n</div>\n")
        << QString::fromUtf8("<div>\n<p><em>Emphasized</em> text.</p>\n</div>");
    QTest::newRow("CommonMark: HTML blocks 44")
        << QString::fromUtf8("<div>\n*Emphasized* text.\n</div>\n")
        << QString::fromUtf8("<div>\n*Emphasized* text.\n</div>");
    QTest::newRow("CommonMark: HTML blocks 45")
        << QString::fromUtf8("<table>\n\n<tr>\n\n<td>\nHi\n</td>\n\n</tr>\n\n</table>\n")
        << QString::fromUtf8("<table>\n<tr>\n<td>\nHi\n</td>\n</tr>\n</table>");
    QTest::newRow("CommonMark: HTML blocks 46")
        << QString::fromUtf8("<table>\n\n  <tr>\n\n    <td>\n      Hi\n    </td>\n\n  </tr>\n\n</table>\n")
        << QString::fromUtf8("<table>\n  <tr>\n<pre><code>&lt;td&gt;\n  Hi\n&lt;/td&gt;\n</code></pre>\n  </tr>\n</table>");
    QTest::newRow("CommonMark: Link reference definitions 1")
        << QString::fromUtf8("[foo]: /url \"title\"\n\n[foo]\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title\">foo</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 2")
        << QString::fromUtf8("   [foo]: \n      /url  \n           'the title'  \n\n[foo]\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"the title\">foo</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 3")
        << QString::fromUtf8("[Foo*bar\\]]:my_(url) 'title (with parens)'\n\n[Foo*bar\\]]\n")
        << QString::fromUtf8("<p><a href=\"my_(url)\" title=\"title (with parens)\">Foo*bar]</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 4")
        << QString::fromUtf8("[Foo bar]:\n<my url>\n'title'\n\n[Foo bar]\n")
        << QString::fromUtf8("<p><a href=\"my%20url\" title=\"title\">Foo bar</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 5")
        << QString::fromUtf8("[foo]: /url '\ntitle\nline1\nline2\n'\n\n[foo]\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"\ntitle\nline1\nline2\n\">foo</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 6")
        << QString::fromUtf8("[foo]: /url 'title\n\nwith blank line'\n\n[foo]\n")
        << QString::fromUtf8("<p>[foo]: /url 'title</p>\n<p>with blank line'</p>\n<p>[foo]</p>");
    QTest::newRow("CommonMark: Link reference definitions 7")
        << QString::fromUtf8("[foo]:\n/url\n\n[foo]\n")
        << QString::fromUtf8("<p><a href=\"/url\">foo</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 8")
        << QString::fromUtf8("[foo]:\n\n[foo]\n")
        << QString::fromUtf8("<p>[foo]:</p>\n<p>[foo]</p>");
    QTest::newRow("CommonMark: Link reference definitions 9")
        << QString::fromUtf8("[foo]: <>\n\n[foo]\n")
        << QString::fromUtf8("<p><a href=\"\">foo</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 10")
        << QString::fromUtf8("[foo]: <bar>(baz)\n\n[foo]\n")
        << QString::fromUtf8("<p>[foo]: <bar>(baz)</p>\n<p>[foo]</p>");
    QTest::newRow("CommonMark: Link reference definitions 11")
        << QString::fromUtf8("[foo]: /url\\bar\\*baz \"foo\\\"bar\\baz\"\n\n[foo]\n")
        << QString::fromUtf8("<p><a href=\"/url%5Cbar*baz\" title=\"foo&quot;bar\\baz\">foo</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 12")
        << QString::fromUtf8("[foo]\n\n[foo]: url\n")
        << QString::fromUtf8("<p><a href=\"url\">foo</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 13")
        << QString::fromUtf8("[foo]\n\n[foo]: first\n[foo]: second\n")
        << QString::fromUtf8("<p><a href=\"first\">foo</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 14")
        << QString::fromUtf8("[FOO]: /url\n\n[Foo]\n")
        << QString::fromUtf8("<p><a href=\"/url\">Foo</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 15")
        << QString::fromUtf8("[ΑΓΩ]: /φου\n\n[αγω]\n")
        << QString::fromUtf8("<p><a href=\"/%CF%86%CE%BF%CF%85\">αγω</a></p>");
    QTest::newRow("CommonMark: Link reference definitions 16")
        << QString::fromUtf8("[foo]: /url\n")
        << QString::fromUtf8("");
    QTest::newRow("CommonMark: Link reference definitions 17")
        << QString::fromUtf8("[\nfoo\n]: /url\nbar\n")
        << QString::fromUtf8("<p>bar</p>");
    QTest::newRow("CommonMark: Link reference definitions 18")
        << QString::fromUtf8("[foo]: /url \"title\" ok\n")
        << QString::fromUtf8("<p>[foo]: /url &quot;title&quot; ok</p>");
    QTest::newRow("CommonMark: Link reference definitions 19")
        << QString::fromUtf8("[foo]: /url\n\"title\" ok\n")
        << QString::fromUtf8("<p>&quot;title&quot; ok</p>");
    QTest::newRow("CommonMark: Link reference definitions 20")
        << QString::fromUtf8("    [foo]: /url \"title\"\n\n[foo]\n")
        << QString::fromUtf8("<pre><code>[foo]: /url &quot;title&quot;\n</code></pre>\n<p>[foo]</p>");
    QTest::newRow("CommonMark: Link reference definitions 21")
        << QString::fromUtf8("```\n[foo]: /url\n```\n\n[foo]\n")
        << QString::fromUtf8("<pre><code>[foo]: /url\n</code></pre>\n<p>[foo]</p>");
    QTest::newRow("CommonMark: Link reference definitions 22")
        << QString::fromUtf8("Foo\n[bar]: /baz\n\n[bar]\n")
        << QString::fromUtf8("<p>Foo\n[bar]: /baz</p>\n<p>[bar]</p>");
    QTest::newRow("CommonMark: Link reference definitions 23")
        << QString::fromUtf8("# [Foo]\n[foo]: /url\n> bar\n")
        << QString::fromUtf8("<h1><a href=\"/url\">Foo</a></h1>\n<blockquote>\n<p>bar</p>\n</blockquote>");
    QTest::newRow("CommonMark: [Foo] 1")
        << QString::fromUtf8("[foo]: /url\nbar\n===\n[foo]\n")
        << QString::fromUtf8("<h1>bar</h1>\n<p><a href=\"/url\">foo</a></p>");
    QTest::newRow("CommonMark: [Foo] 2")
        << QString::fromUtf8("[foo]: /url\n===\n[foo]\n")
        << QString::fromUtf8("<p>===\n<a href=\"/url\">foo</a></p>");
    QTest::newRow("CommonMark: [Foo] 3")
        << QString::fromUtf8("[foo]: /foo-url \"foo\"\n[bar]: /bar-url\n  \"bar\"\n[baz]: /baz-url\n\n[foo],\n[bar],\n[baz]\n")
        << QString::fromUtf8("<p><a href=\"/foo-url\" title=\"foo\">foo</a>,\n<a href=\"/bar-url\" title=\"bar\">bar</a>,\n<a href=\"/baz-url\">baz</a></p>");
    QTest::newRow("CommonMark: [Foo] 4")
        << QString::fromUtf8("[foo]\n\n> [foo]: /url\n")
        << QString::fromUtf8("<p><a href=\"/url\">foo</a></p>\n<blockquote>\n</blockquote>");
    QTest::newRow("CommonMark: Paragraphs 1")
        << QString::fromUtf8("aaa\n\nbbb\n")
        << QString::fromUtf8("<p>aaa</p>\n<p>bbb</p>");
    QTest::newRow("CommonMark: Paragraphs 2")
        << QString::fromUtf8("aaa\nbbb\n\nccc\nddd\n")
        << QString::fromUtf8("<p>aaa\nbbb</p>\n<p>ccc\nddd</p>");
    QTest::newRow("CommonMark: Paragraphs 3")
        << QString::fromUtf8("aaa\n\n\nbbb\n")
        << QString::fromUtf8("<p>aaa</p>\n<p>bbb</p>");
    QTest::newRow("CommonMark: Paragraphs 4")
        << QString::fromUtf8("  aaa\n bbb\n")
        << QString::fromUtf8("<p>aaa\nbbb</p>");
    QTest::newRow("CommonMark: Paragraphs 5")
        << QString::fromUtf8("aaa\n             bbb\n                                       ccc\n")
        << QString::fromUtf8("<p>aaa\nbbb\nccc</p>");
    QTest::newRow("CommonMark: Paragraphs 6")
        << QString::fromUtf8("   aaa\nbbb\n")
        << QString::fromUtf8("<p>aaa\nbbb</p>");
    QTest::newRow("CommonMark: Paragraphs 7")
        << QString::fromUtf8("    aaa\nbbb\n")
        << QString::fromUtf8("<pre><code>aaa\n</code></pre>\n<p>bbb</p>");
    QTest::newRow("CommonMark: Paragraphs 8")
        << QString::fromUtf8("aaa     \nbbb     \n")
        << QString::fromUtf8("<p>aaa<br>\nbbb</p>");
    QTest::newRow("CommonMark: Blank lines 1")
        << QString::fromUtf8("  \n\naaa\n  \n\n# aaa\n\n  \n")
        << QString::fromUtf8("<p>aaa</p>\n<h1>aaa</h1>");
    QTest::newRow("CommonMark: Block quotes 1")
        << QString::fromUtf8("> # Foo\n> bar\n> baz\n")
        << QString::fromUtf8("<blockquote>\n<h1>Foo</h1>\n<p>bar\nbaz</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 2")
        << QString::fromUtf8("># Foo\n>bar\n> baz\n")
        << QString::fromUtf8("<blockquote>\n<h1>Foo</h1>\n<p>bar\nbaz</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 3")
        << QString::fromUtf8("   > # Foo\n   > bar\n > baz\n")
        << QString::fromUtf8("<blockquote>\n<h1>Foo</h1>\n<p>bar\nbaz</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 4")
        << QString::fromUtf8("    > # Foo\n    > bar\n    > baz\n")
        << QString::fromUtf8("<pre><code>&gt; # Foo\n&gt; bar\n&gt; baz\n</code></pre>");
    QTest::newRow("CommonMark: Block quotes 5")
        << QString::fromUtf8("> # Foo\n> bar\nbaz\n")
        << QString::fromUtf8("<blockquote>\n<h1>Foo</h1>\n<p>bar\nbaz</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 6")
        << QString::fromUtf8("> bar\nbaz\n> foo\n")
        << QString::fromUtf8("<blockquote>\n<p>bar\nbaz\nfoo</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 7")
        << QString::fromUtf8("> foo\n---\n")
        << QString::fromUtf8("<blockquote>\n<p>foo</p>\n</blockquote>\n<hr>");
    QTest::newRow("CommonMark: Block quotes 8")
        << QString::fromUtf8("> - foo\n- bar\n")
        << QString::fromUtf8("<blockquote>\n<ul>\n<li>foo</li>\n</ul>\n</blockquote>\n<ul>\n<li>bar</li>\n</ul>");
    QTest::newRow("CommonMark: Block quotes 9")
        << QString::fromUtf8(">     foo\n    bar\n")
        << QString::fromUtf8("<blockquote>\n<pre><code>foo\n</code></pre>\n</blockquote>\n<pre><code>bar\n</code></pre>");
    QTest::newRow("CommonMark: Block quotes 10")
        << QString::fromUtf8("> ```\nfoo\n```\n")
        << QString::fromUtf8("<blockquote>\n<pre><code></code></pre>\n</blockquote>\n<p>foo</p>\n<pre><code></code></pre>");
    QTest::newRow("CommonMark: Block quotes 11")
        << QString::fromUtf8("> foo\n    - bar\n")
        << QString::fromUtf8("<blockquote>\n<p>foo\n- bar</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 12")
        << QString::fromUtf8(">\n")
        << QString::fromUtf8("<blockquote>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 13")
        << QString::fromUtf8(">\n>  \n> \n")
        << QString::fromUtf8("<blockquote>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 14")
        << QString::fromUtf8(">\n> foo\n>  \n")
        << QString::fromUtf8("<blockquote>\n<p>foo</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 15")
        << QString::fromUtf8("> foo\n\n> bar\n")
        << QString::fromUtf8("<blockquote>\n<p>foo</p>\n</blockquote>\n<blockquote>\n<p>bar</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 16")
        << QString::fromUtf8("> foo\n> bar\n")
        << QString::fromUtf8("<blockquote>\n<p>foo\nbar</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 17")
        << QString::fromUtf8("> foo\n>\n> bar\n")
        << QString::fromUtf8("<blockquote>\n<p>foo</p>\n<p>bar</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 18")
        << QString::fromUtf8("foo\n> bar\n")
        << QString::fromUtf8("<p>foo</p>\n<blockquote>\n<p>bar</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 19")
        << QString::fromUtf8("> aaa\n***\n> bbb\n")
        << QString::fromUtf8("<blockquote>\n<p>aaa</p>\n</blockquote>\n<hr>\n<blockquote>\n<p>bbb</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 20")
        << QString::fromUtf8("> bar\nbaz\n")
        << QString::fromUtf8("<blockquote>\n<p>bar\nbaz</p>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 21")
        << QString::fromUtf8("> bar\n\nbaz\n")
        << QString::fromUtf8("<blockquote>\n<p>bar</p>\n</blockquote>\n<p>baz</p>");
    QTest::newRow("CommonMark: Block quotes 22")
        << QString::fromUtf8("> bar\n>\nbaz\n")
        << QString::fromUtf8("<blockquote>\n<p>bar</p>\n</blockquote>\n<p>baz</p>");
    QTest::newRow("CommonMark: Block quotes 23")
        << QString::fromUtf8("> > > foo\nbar\n")
        << QString::fromUtf8("<blockquote>\n<blockquote>\n<blockquote>\n<p>foo\nbar</p>\n</blockquote>\n</blockquote>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 24")
        << QString::fromUtf8(">>> foo\n> bar\n>>baz\n")
        << QString::fromUtf8("<blockquote>\n<blockquote>\n<blockquote>\n<p>foo\nbar\nbaz</p>\n</blockquote>\n</blockquote>\n</blockquote>");
    QTest::newRow("CommonMark: Block quotes 25")
        << QString::fromUtf8(">     code\n\n>    not code\n")
        << QString::fromUtf8("<blockquote>\n<pre><code>code\n</code></pre>\n</blockquote>\n<blockquote>\n<p>not code</p>\n</blockquote>");
    QTest::newRow("CommonMark: List items 1")
        << QString::fromUtf8("A paragraph\nwith two lines.\n\n    indented code\n\n> A block quote.\n")
        << QString::fromUtf8("<p>A paragraph\nwith two lines.</p>\n<pre><code>indented code\n</code></pre>\n<blockquote>\n<p>A block quote.</p>\n</blockquote>");
    QTest::newRow("CommonMark: List items 2")
        << QString::fromUtf8("1.  A paragraph\n    with two lines.\n\n        indented code\n\n    > A block quote.\n")
        << QString::fromUtf8("<ol>\n<li>\n<p>A paragraph\nwith two lines.</p>\n<pre><code>indented code\n</code></pre>\n<blockquote>\n<p>A block quote.</p>\n</blockquote>\n</li>\n</ol>");
    QTest::newRow("CommonMark: List items 3")
        << QString::fromUtf8("- one\n\n two\n")
        << QString::fromUtf8("<ul>\n<li>one</li>\n</ul>\n<p>two</p>");
    QTest::newRow("CommonMark: List items 4")
        << QString::fromUtf8("- one\n\n  two\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>one</p>\n<p>two</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: List items 5")
        << QString::fromUtf8(" -    one\n\n     two\n")
        << QString::fromUtf8("<ul>\n<li>one</li>\n</ul>\n<pre><code> two\n</code></pre>");
    QTest::newRow("CommonMark: List items 6")
        << QString::fromUtf8(" -    one\n\n      two\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>one</p>\n<p>two</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: List items 7")
        << QString::fromUtf8("   > > 1.  one\n>>\n>>     two\n")
        << QString::fromUtf8("<blockquote>\n<blockquote>\n<ol>\n<li>\n<p>one</p>\n<p>two</p>\n</li>\n</ol>\n</blockquote>\n</blockquote>");
    QTest::newRow("CommonMark: List items 8")
        << QString::fromUtf8(">>- one\n>>\n  >  > two\n")
        << QString::fromUtf8("<blockquote>\n<blockquote>\n<ul>\n<li>one</li>\n</ul>\n<p>two</p>\n</blockquote>\n</blockquote>");
    QTest::newRow("CommonMark: List items 9")
        << QString::fromUtf8("-one\n\n2.two\n")
        << QString::fromUtf8("<p>-one</p>\n<p>2.two</p>");
    QTest::newRow("CommonMark: List items 10")
        << QString::fromUtf8("- foo\n\n\n  bar\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>foo</p>\n<p>bar</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: List items 11")
        << QString::fromUtf8("1.  foo\n\n    ```\n    bar\n    ```\n\n    baz\n\n    > bam\n")
        << QString::fromUtf8("<ol>\n<li>\n<p>foo</p>\n<pre><code>bar\n</code></pre>\n<p>baz</p>\n<blockquote>\n<p>bam</p>\n</blockquote>\n</li>\n</ol>");
    QTest::newRow("CommonMark: List items 12")
        << QString::fromUtf8("- Foo\n\n      bar\n\n\n      baz\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>Foo</p>\n<pre><code>bar\n\n\nbaz\n</code></pre>\n</li>\n</ul>");
    QTest::newRow("CommonMark: List items 13")
        << QString::fromUtf8("123456789. ok\n")
        << QString::fromUtf8("<ol start=\"123456789\">\n<li>ok</li>\n</ol>");
    QTest::newRow("CommonMark: List items 14")
        << QString::fromUtf8("1234567890. not ok\n")
        << QString::fromUtf8("<p>1234567890. not ok</p>");
    QTest::newRow("CommonMark: List items 15")
        << QString::fromUtf8("0. ok\n")
        << QString::fromUtf8("<ol start=\"0\">\n<li>ok</li>\n</ol>");
    QTest::newRow("CommonMark: List items 16")
        << QString::fromUtf8("003. ok\n")
        << QString::fromUtf8("<ol start=\"3\">\n<li>ok</li>\n</ol>");
    QTest::newRow("CommonMark: List items 17")
        << QString::fromUtf8("-1. not ok\n")
        << QString::fromUtf8("<p>-1. not ok</p>");
    QTest::newRow("CommonMark: List items 18")
        << QString::fromUtf8("- foo\n\n      bar\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>foo</p>\n<pre><code>bar\n</code></pre>\n</li>\n</ul>");
    QTest::newRow("CommonMark: List items 19")
        << QString::fromUtf8("  10.  foo\n\n           bar\n")
        << QString::fromUtf8("<ol start=\"10\">\n<li>\n<p>foo</p>\n<pre><code>bar\n</code></pre>\n</li>\n</ol>");
    QTest::newRow("CommonMark: List items 20")
        << QString::fromUtf8("    indented code\n\nparagraph\n\n    more code\n")
        << QString::fromUtf8("<pre><code>indented code\n</code></pre>\n<p>paragraph</p>\n<pre><code>more code\n</code></pre>");
    QTest::newRow("CommonMark: List items 21")
        << QString::fromUtf8("1.     indented code\n\n   paragraph\n\n       more code\n")
        << QString::fromUtf8("<ol>\n<li>\n<pre><code>indented code\n</code></pre>\n<p>paragraph</p>\n<pre><code>more code\n</code></pre>\n</li>\n</ol>");
    QTest::newRow("CommonMark: List items 22")
        << QString::fromUtf8("1.      indented code\n\n   paragraph\n\n       more code\n")
        << QString::fromUtf8("<ol>\n<li>\n<pre><code> indented code\n</code></pre>\n<p>paragraph</p>\n<pre><code>more code\n</code></pre>\n</li>\n</ol>");
    QTest::newRow("CommonMark: List items 23")
        << QString::fromUtf8("   foo\n\nbar\n")
        << QString::fromUtf8("<p>foo</p>\n<p>bar</p>");
    QTest::newRow("CommonMark: List items 24")
        << QString::fromUtf8("-    foo\n\n  bar\n")
        << QString::fromUtf8("<ul>\n<li>foo</li>\n</ul>\n<p>bar</p>");
    QTest::newRow("CommonMark: List items 25")
        << QString::fromUtf8("-  foo\n\n   bar\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>foo</p>\n<p>bar</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: List items 26")
        << QString::fromUtf8("-\n  foo\n-\n  ```\n  bar\n  ```\n-\n      baz\n")
        << QString::fromUtf8("<ul>\n<li>foo</li>\n<li>\n<pre><code>bar\n</code></pre>\n</li>\n<li>\n<pre><code>baz\n</code></pre>\n</li>\n</ul>");
    QTest::newRow("CommonMark: List items 27")
        << QString::fromUtf8("-   \n  foo\n")
        << QString::fromUtf8("<ul>\n<li>foo</li>\n</ul>");
    QTest::newRow("CommonMark: List items 28")
        << QString::fromUtf8("-\n\n  foo\n")
        << QString::fromUtf8("<ul>\n<li></li>\n</ul>\n<p>foo</p>");
    QTest::newRow("CommonMark: List items 29")
        << QString::fromUtf8("- foo\n-\n- bar\n")
        << QString::fromUtf8("<ul>\n<li>foo</li>\n<li></li>\n<li>bar</li>\n</ul>");
    QTest::newRow("CommonMark: List items 30")
        << QString::fromUtf8("- foo\n-   \n- bar\n")
        << QString::fromUtf8("<ul>\n<li>foo</li>\n<li></li>\n<li>bar</li>\n</ul>");
    QTest::newRow("CommonMark: List items 31")
        << QString::fromUtf8("1. foo\n2.\n3. bar\n")
        << QString::fromUtf8("<ol>\n<li>foo</li>\n<li></li>\n<li>bar</li>\n</ol>");
    QTest::newRow("CommonMark: List items 32")
        << QString::fromUtf8("*\n")
        << QString::fromUtf8("<ul>\n<li></li>\n</ul>");
    QTest::newRow("CommonMark: List items 33")
        << QString::fromUtf8("foo\n*\n\nfoo\n1.\n")
        << QString::fromUtf8("<p>foo\n*</p>\n<p>foo\n1.</p>");
    QTest::newRow("CommonMark: List items 34")
        << QString::fromUtf8(" 1.  A paragraph\n     with two lines.\n\n         indented code\n\n     > A block quote.\n")
        << QString::fromUtf8("<ol>\n<li>\n<p>A paragraph\nwith two lines.</p>\n<pre><code>indented code\n</code></pre>\n<blockquote>\n<p>A block quote.</p>\n</blockquote>\n</li>\n</ol>");
    QTest::newRow("CommonMark: List items 35")
        << QString::fromUtf8("  1.  A paragraph\n      with two lines.\n\n          indented code\n\n      > A block quote.\n")
        << QString::fromUtf8("<ol>\n<li>\n<p>A paragraph\nwith two lines.</p>\n<pre><code>indented code\n</code></pre>\n<blockquote>\n<p>A block quote.</p>\n</blockquote>\n</li>\n</ol>");
    QTest::newRow("CommonMark: List items 36")
        << QString::fromUtf8("   1.  A paragraph\n       with two lines.\n\n           indented code\n\n       > A block quote.\n")
        << QString::fromUtf8("<ol>\n<li>\n<p>A paragraph\nwith two lines.</p>\n<pre><code>indented code\n</code></pre>\n<blockquote>\n<p>A block quote.</p>\n</blockquote>\n</li>\n</ol>");
    QTest::newRow("CommonMark: List items 37")
        << QString::fromUtf8("    1.  A paragraph\n        with two lines.\n\n            indented code\n\n        > A block quote.\n")
        << QString::fromUtf8("<pre><code>1.  A paragraph\n    with two lines.\n\n        indented code\n\n    &gt; A block quote.\n</code></pre>");
    QTest::newRow("CommonMark: List items 38")
        << QString::fromUtf8("  1.  A paragraph\nwith two lines.\n\n          indented code\n\n      > A block quote.\n")
        << QString::fromUtf8("<ol>\n<li>\n<p>A paragraph\nwith two lines.</p>\n<pre><code>indented code\n</code></pre>\n<blockquote>\n<p>A block quote.</p>\n</blockquote>\n</li>\n</ol>");
    QTest::newRow("CommonMark: List items 39")
        << QString::fromUtf8("  1.  A paragraph\n    with two lines.\n")
        << QString::fromUtf8("<ol>\n<li>A paragraph\nwith two lines.</li>\n</ol>");
    QTest::newRow("CommonMark: List items 40")
        << QString::fromUtf8("> 1. > Blockquote\ncontinued here.\n")
        << QString::fromUtf8("<blockquote>\n<ol>\n<li>\n<blockquote>\n<p>Blockquote\ncontinued here.</p>\n</blockquote>\n</li>\n</ol>\n</blockquote>");
    QTest::newRow("CommonMark: List items 41")
        << QString::fromUtf8("> 1. > Blockquote\n> continued here.\n")
        << QString::fromUtf8("<blockquote>\n<ol>\n<li>\n<blockquote>\n<p>Blockquote\ncontinued here.</p>\n</blockquote>\n</li>\n</ol>\n</blockquote>");
    QTest::newRow("CommonMark: List items 42")
        << QString::fromUtf8("- foo\n  - bar\n    - baz\n      - boo\n")
        << QString::fromUtf8("<ul>\n<li>foo\n<ul>\n<li>bar\n<ul>\n<li>baz\n<ul>\n<li>boo</li>\n</ul>\n</li>\n</ul>\n</li>\n</ul>\n</li>\n</ul>");
    QTest::newRow("CommonMark: List items 43")
        << QString::fromUtf8("- foo\n - bar\n  - baz\n   - boo\n")
        << QString::fromUtf8("<ul>\n<li>foo</li>\n<li>bar</li>\n<li>baz</li>\n<li>boo</li>\n</ul>");
    QTest::newRow("CommonMark: List items 44")
        << QString::fromUtf8("10) foo\n    - bar\n")
        << QString::fromUtf8("<ol start=\"10\">\n<li>foo\n<ul>\n<li>bar</li>\n</ul>\n</li>\n</ol>");
    QTest::newRow("CommonMark: List items 45")
        << QString::fromUtf8("10) foo\n   - bar\n")
        << QString::fromUtf8("<ol start=\"10\">\n<li>foo</li>\n</ol>\n<ul>\n<li>bar</li>\n</ul>");
    QTest::newRow("CommonMark: List items 46")
        << QString::fromUtf8("- - foo\n")
        << QString::fromUtf8("<ul>\n<li>\n<ul>\n<li>foo</li>\n</ul>\n</li>\n</ul>");
    QTest::newRow("CommonMark: List items 47")
        << QString::fromUtf8("1. - 2. foo\n")
        << QString::fromUtf8("<ol>\n<li>\n<ul>\n<li>\n<ol start=\"2\">\n<li>foo</li>\n</ol>\n</li>\n</ul>\n</li>\n</ol>");
    QTest::newRow("CommonMark: List items 48")
        << QString::fromUtf8("- # Foo\n- Bar\n  ---\n  baz\n")
        << QString::fromUtf8("<ul>\n<li>\n<h1>Foo</h1>\n</li>\n<li>\n<h2>Bar</h2>\nbaz</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 1")
        << QString::fromUtf8("- foo\n- bar\n+ baz\n")
        << QString::fromUtf8("<ul>\n<li>foo</li>\n<li>bar</li>\n</ul>\n<ul>\n<li>baz</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 2")
        << QString::fromUtf8("1. foo\n2. bar\n3) baz\n")
        << QString::fromUtf8("<ol>\n<li>foo</li>\n<li>bar</li>\n</ol>\n<ol start=\"3\">\n<li>baz</li>\n</ol>");
    QTest::newRow("CommonMark: Lists 3")
        << QString::fromUtf8("Foo\n- bar\n- baz\n")
        << QString::fromUtf8("<p>Foo</p>\n<ul>\n<li>bar</li>\n<li>baz</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 4")
        << QString::fromUtf8("The number of windows in my house is\n14.  The number of doors is 6.\n")
        << QString::fromUtf8("<p>The number of windows in my house is\n14.  The number of doors is 6.</p>");
    QTest::newRow("CommonMark: Lists 5")
        << QString::fromUtf8("The number of windows in my house is\n1.  The number of doors is 6.\n")
        << QString::fromUtf8("<p>The number of windows in my house is</p>\n<ol>\n<li>The number of doors is 6.</li>\n</ol>");
    QTest::newRow("CommonMark: Lists 6")
        << QString::fromUtf8("- foo\n\n- bar\n\n\n- baz\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>foo</p>\n</li>\n<li>\n<p>bar</p>\n</li>\n<li>\n<p>baz</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 7")
        << QString::fromUtf8("- foo\n  - bar\n    - baz\n\n\n      bim\n")
        << QString::fromUtf8("<ul>\n<li>foo\n<ul>\n<li>bar\n<ul>\n<li>\n<p>baz</p>\n<p>bim</p>\n</li>\n</ul>\n</li>\n</ul>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 8")
        << QString::fromUtf8("- foo\n- bar\n\n<!-- -->\n\n- baz\n- bim\n")
        << QString::fromUtf8("<ul>\n<li>foo</li>\n<li>bar</li>\n</ul>\n<!-- -->\n<ul>\n<li>baz</li>\n<li>bim</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 9")
        << QString::fromUtf8("-   foo\n\n    notcode\n\n-   foo\n\n<!-- -->\n\n    code\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>foo</p>\n<p>notcode</p>\n</li>\n<li>\n<p>foo</p>\n</li>\n</ul>\n<!-- -->\n<pre><code>code\n</code></pre>");
    QTest::newRow("CommonMark: Lists 10")
        << QString::fromUtf8("- a\n - b\n  - c\n   - d\n  - e\n - f\n- g\n")
        << QString::fromUtf8("<ul>\n<li>a</li>\n<li>b</li>\n<li>c</li>\n<li>d</li>\n<li>e</li>\n<li>f</li>\n<li>g</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 11")
        << QString::fromUtf8("1. a\n\n  2. b\n\n   3. c\n")
        << QString::fromUtf8("<ol>\n<li>\n<p>a</p>\n</li>\n<li>\n<p>b</p>\n</li>\n<li>\n<p>c</p>\n</li>\n</ol>");
    QTest::newRow("CommonMark: Lists 12")
        << QString::fromUtf8("- a\n - b\n  - c\n   - d\n    - e\n")
        << QString::fromUtf8("<ul>\n<li>a</li>\n<li>b</li>\n<li>c</li>\n<li>d\n- e</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 13")
        << QString::fromUtf8("1. a\n\n  2. b\n\n    3. c\n")
        << QString::fromUtf8("<ol>\n<li>\n<p>a</p>\n</li>\n<li>\n<p>b</p>\n</li>\n</ol>\n<pre><code>3. c\n</code></pre>");
    QTest::newRow("CommonMark: Lists 14")
        << QString::fromUtf8("- a\n- b\n\n- c\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>a</p>\n</li>\n<li>\n<p>b</p>\n</li>\n<li>\n<p>c</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 15")
        << QString::fromUtf8("* a\n*\n\n* c\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>a</p>\n</li>\n<li></li>\n<li>\n<p>c</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 16")
        << QString::fromUtf8("- a\n- b\n\n  c\n- d\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>a</p>\n</li>\n<li>\n<p>b</p>\n<p>c</p>\n</li>\n<li>\n<p>d</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 17")
        << QString::fromUtf8("- a\n- b\n\n  [ref]: /url\n- d\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>a</p>\n</li>\n<li>\n<p>b</p>\n</li>\n<li>\n<p>d</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 18")
        << QString::fromUtf8("- a\n- ```\n  b\n\n\n  ```\n- c\n")
        << QString::fromUtf8("<ul>\n<li>a</li>\n<li>\n<pre><code>b\n\n\n</code></pre>\n</li>\n<li>c</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 19")
        << QString::fromUtf8("- a\n  - b\n\n    c\n- d\n")
        << QString::fromUtf8("<ul>\n<li>a\n<ul>\n<li>\n<p>b</p>\n<p>c</p>\n</li>\n</ul>\n</li>\n<li>d</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 20")
        << QString::fromUtf8("* a\n  > b\n  >\n* c\n")
        << QString::fromUtf8("<ul>\n<li>a\n<blockquote>\n<p>b</p>\n</blockquote>\n</li>\n<li>c</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 21")
        << QString::fromUtf8("- a\n  > b\n  ```\n  c\n  ```\n- d\n")
        << QString::fromUtf8("<ul>\n<li>a\n<blockquote>\n<p>b</p>\n</blockquote>\n<pre><code>c\n</code></pre>\n</li>\n<li>d</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 22")
        << QString::fromUtf8("- a\n")
        << QString::fromUtf8("<ul>\n<li>a</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 23")
        << QString::fromUtf8("- a\n  - b\n")
        << QString::fromUtf8("<ul>\n<li>a\n<ul>\n<li>b</li>\n</ul>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 24")
        << QString::fromUtf8("1. ```\n   foo\n   ```\n\n   bar\n")
        << QString::fromUtf8("<ol>\n<li>\n<pre><code>foo\n</code></pre>\n<p>bar</p>\n</li>\n</ol>");
    QTest::newRow("CommonMark: Lists 25")
        << QString::fromUtf8("* foo\n  * bar\n\n  baz\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>foo</p>\n<ul>\n<li>bar</li>\n</ul>\n<p>baz</p>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Lists 26")
        << QString::fromUtf8("- a\n  - b\n  - c\n\n- d\n  - e\n  - f\n")
        << QString::fromUtf8("<ul>\n<li>\n<p>a</p>\n<ul>\n<li>b</li>\n<li>c</li>\n</ul>\n</li>\n<li>\n<p>d</p>\n<ul>\n<li>e</li>\n<li>f</li>\n</ul>\n</li>\n</ul>");
    QTest::newRow("CommonMark: Inlines 1")
        << QString::fromUtf8("`hi`lo`\n")
        << QString::fromUtf8("<p><code>hi</code>lo`</p>");
    QTest::newRow("CommonMark: Code spans 1")
        << QString::fromUtf8("`foo`\n")
        << QString::fromUtf8("<p><code>foo</code></p>");
    QTest::newRow("CommonMark: Code spans 2")
        << QString::fromUtf8("`` foo ` bar ``\n")
        << QString::fromUtf8("<p><code>foo ` bar</code></p>");
    QTest::newRow("CommonMark: Code spans 3")
        << QString::fromUtf8("` `` `\n")
        << QString::fromUtf8("<p><code>``</code></p>");
    QTest::newRow("CommonMark: Code spans 4")
        << QString::fromUtf8("`  ``  `\n")
        << QString::fromUtf8("<p><code> `` </code></p>");
    QTest::newRow("CommonMark: Code spans 5")
        << QString::fromUtf8("` a`\n")
        << QString::fromUtf8("<p><code> a</code></p>");
    QTest::newRow("CommonMark: Code spans 6")
        << QString::fromUtf8("` b `\n")
        << QString::fromUtf8("<p><code> b </code></p>");
    QTest::newRow("CommonMark: Code spans 7")
        << QString::fromUtf8("` `\n`  `\n")
        << QString::fromUtf8("<p><code> </code>\n<code>  </code></p>");
    QTest::newRow("CommonMark: Code spans 8")
        << QString::fromUtf8("``\nfoo\nbar  \nbaz\n``\n")
        << QString::fromUtf8("<p><code>foo bar   baz</code></p>");
    QTest::newRow("CommonMark: Code spans 9")
        << QString::fromUtf8("``\nfoo \n``\n")
        << QString::fromUtf8("<p><code>foo </code></p>");
    QTest::newRow("CommonMark: Code spans 10")
        << QString::fromUtf8("`foo   bar \nbaz`\n")
        << QString::fromUtf8("<p><code>foo   bar  baz</code></p>");
    QTest::newRow("CommonMark: Code spans 11")
        << QString::fromUtf8("`foo\\`bar`\n")
        << QString::fromUtf8("<p><code>foo\\</code>bar`</p>");
    QTest::newRow("CommonMark: Code spans 12")
        << QString::fromUtf8("``foo`bar``\n")
        << QString::fromUtf8("<p><code>foo`bar</code></p>");
    QTest::newRow("CommonMark: Code spans 13")
        << QString::fromUtf8("` foo `` bar `\n")
        << QString::fromUtf8("<p><code>foo `` bar</code></p>");
    QTest::newRow("CommonMark: Code spans 14")
        << QString::fromUtf8("*foo`*`\n")
        << QString::fromUtf8("<p>*foo<code>*</code></p>");
    QTest::newRow("CommonMark: Code spans 15")
        << QString::fromUtf8("[not a `link](/foo`)\n")
        << QString::fromUtf8("<p>[not a <code>link](/foo</code>)</p>");
    QTest::newRow("CommonMark: Code spans 16")
        << QString::fromUtf8("`<a href=\"`\">`\n")
        << QString::fromUtf8("<p><code>&lt;a href=&quot;</code>&quot;&gt;`</p>");
    QTest::newRow("CommonMark: Code spans 17")
        << QString::fromUtf8("<a href=\"`\">`\n")
        << QString::fromUtf8("<p><a href=\"`\">`</p>");
    QTest::newRow("CommonMark: Code spans 18")
        << QString::fromUtf8("`<https://foo.bar.`baz>`\n")
        << QString::fromUtf8("<p><code>&lt;https://foo.bar.</code>baz&gt;`</p>");
    QTest::newRow("CommonMark: Code spans 19")
        << QString::fromUtf8("<https://foo.bar.`baz>`\n")
        << QString::fromUtf8("<p><a href=\"https://foo.bar.%60baz\">https://foo.bar.`baz</a>`</p>");
    QTest::newRow("CommonMark: Code spans 20")
        << QString::fromUtf8("```foo``\n")
        << QString::fromUtf8("<p>```foo``</p>");
    QTest::newRow("CommonMark: Code spans 21")
        << QString::fromUtf8("`foo\n")
        << QString::fromUtf8("<p>`foo</p>");
    QTest::newRow("CommonMark: Code spans 22")
        << QString::fromUtf8("`foo``bar``\n")
        << QString::fromUtf8("<p>`foo<code>bar</code></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 1")
        << QString::fromUtf8("*foo bar*\n")
        << QString::fromUtf8("<p><em>foo bar</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 2")
        << QString::fromUtf8("a * foo bar*\n")
        << QString::fromUtf8("<p>a * foo bar*</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 3")
        << QString::fromUtf8("a*\"foo\"*\n")
        << QString::fromUtf8("<p>a*&quot;foo&quot;*</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 4")
        << QString::fromUtf8("* a *\n")
        << QString::fromUtf8("<p>* a *</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 5")
        << QString::fromUtf8("*$*alpha.\n\n*£*bravo.\n\n*€*charlie.\n\n*𞋿*delta.\n")
        << QString::fromUtf8("<p>*$*alpha.</p>\n<p>*£*bravo.</p>\n<p>*€*charlie.</p>\n<p>*𞋿*delta.</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 6")
        << QString::fromUtf8("foo*bar*\n")
        << QString::fromUtf8("<p>foo<em>bar</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 7")
        << QString::fromUtf8("5*6*78\n")
        << QString::fromUtf8("<p>5<em>6</em>78</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 8")
        << QString::fromUtf8("_foo bar_\n")
        << QString::fromUtf8("<p><em>foo bar</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 9")
        << QString::fromUtf8("_ foo bar_\n")
        << QString::fromUtf8("<p>_ foo bar_</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 10")
        << QString::fromUtf8("a_\"foo\"_\n")
        << QString::fromUtf8("<p>a_&quot;foo&quot;_</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 11")
        << QString::fromUtf8("foo_bar_\n")
        << QString::fromUtf8("<p>foo_bar_</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 12")
        << QString::fromUtf8("5_6_78\n")
        << QString::fromUtf8("<p>5_6_78</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 13")
        << QString::fromUtf8("пристаням_стремятся_\n")
        << QString::fromUtf8("<p>пристаням_стремятся_</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 14")
        << QString::fromUtf8("aa_\"bb\"_cc\n")
        << QString::fromUtf8("<p>aa_&quot;bb&quot;_cc</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 15")
        << QString::fromUtf8("foo-_(bar)_\n")
        << QString::fromUtf8("<p>foo-<em>(bar)</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 16")
        << QString::fromUtf8("_foo*\n")
        << QString::fromUtf8("<p>_foo*</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 17")
        << QString::fromUtf8("*foo bar *\n")
        << QString::fromUtf8("<p>*foo bar *</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 18")
        << QString::fromUtf8("*foo bar\n*\n")
        << QString::fromUtf8("<p>*foo bar\n*</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 19")
        << QString::fromUtf8("*(*foo)\n")
        << QString::fromUtf8("<p>*(*foo)</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 20")
        << QString::fromUtf8("*(*foo*)*\n")
        << QString::fromUtf8("<p><em>(<em>foo</em>)</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 21")
        << QString::fromUtf8("*foo*bar\n")
        << QString::fromUtf8("<p><em>foo</em>bar</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 22")
        << QString::fromUtf8("_foo bar _\n")
        << QString::fromUtf8("<p>_foo bar _</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 23")
        << QString::fromUtf8("_(_foo)\n")
        << QString::fromUtf8("<p>_(_foo)</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 24")
        << QString::fromUtf8("_(_foo_)_\n")
        << QString::fromUtf8("<p><em>(<em>foo</em>)</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 25")
        << QString::fromUtf8("_foo_bar\n")
        << QString::fromUtf8("<p>_foo_bar</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 26")
        << QString::fromUtf8("_пристаням_стремятся\n")
        << QString::fromUtf8("<p>_пристаням_стремятся</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 27")
        << QString::fromUtf8("_foo_bar_baz_\n")
        << QString::fromUtf8("<p><em>foo_bar_baz</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 28")
        << QString::fromUtf8("_(bar)_.\n")
        << QString::fromUtf8("<p><em>(bar)</em>.</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 29")
        << QString::fromUtf8("**foo bar**\n")
        << QString::fromUtf8("<p><strong>foo bar</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 30")
        << QString::fromUtf8("** foo bar**\n")
        << QString::fromUtf8("<p>** foo bar**</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 31")
        << QString::fromUtf8("a**\"foo\"**\n")
        << QString::fromUtf8("<p>a**&quot;foo&quot;**</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 32")
        << QString::fromUtf8("foo**bar**\n")
        << QString::fromUtf8("<p>foo<strong>bar</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 33")
        << QString::fromUtf8("__foo bar__\n")
        << QString::fromUtf8("<p><strong>foo bar</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 34")
        << QString::fromUtf8("__ foo bar__\n")
        << QString::fromUtf8("<p>__ foo bar__</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 35")
        << QString::fromUtf8("__\nfoo bar__\n")
        << QString::fromUtf8("<p>__\nfoo bar__</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 36")
        << QString::fromUtf8("a__\"foo\"__\n")
        << QString::fromUtf8("<p>a__&quot;foo&quot;__</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 37")
        << QString::fromUtf8("foo__bar__\n")
        << QString::fromUtf8("<p>foo__bar__</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 38")
        << QString::fromUtf8("5__6__78\n")
        << QString::fromUtf8("<p>5__6__78</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 39")
        << QString::fromUtf8("пристаням__стремятся__\n")
        << QString::fromUtf8("<p>пристаням__стремятся__</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 40")
        << QString::fromUtf8("__foo, __bar__, baz__\n")
        << QString::fromUtf8("<p><strong>foo, <strong>bar</strong>, baz</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 41")
        << QString::fromUtf8("foo-__(bar)__\n")
        << QString::fromUtf8("<p>foo-<strong>(bar)</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 42")
        << QString::fromUtf8("**foo bar **\n")
        << QString::fromUtf8("<p>**foo bar **</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 43")
        << QString::fromUtf8("**(**foo)\n")
        << QString::fromUtf8("<p>**(**foo)</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 44")
        << QString::fromUtf8("*(**foo**)*\n")
        << QString::fromUtf8("<p><em>(<strong>foo</strong>)</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 45")
        << QString::fromUtf8("**Gomphocarpus (*Gomphocarpus physocarpus*, syn.\n*Asclepias physocarpa*)**\n")
        << QString::fromUtf8("<p><strong>Gomphocarpus (<em>Gomphocarpus physocarpus</em>, syn.\n<em>Asclepias physocarpa</em>)</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 46")
        << QString::fromUtf8("**foo \"*bar*\" foo**\n")
        << QString::fromUtf8("<p><strong>foo &quot;<em>bar</em>&quot; foo</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 47")
        << QString::fromUtf8("**foo**bar\n")
        << QString::fromUtf8("<p><strong>foo</strong>bar</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 48")
        << QString::fromUtf8("__foo bar __\n")
        << QString::fromUtf8("<p>__foo bar __</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 49")
        << QString::fromUtf8("__(__foo)\n")
        << QString::fromUtf8("<p>__(__foo)</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 50")
        << QString::fromUtf8("_(__foo__)_\n")
        << QString::fromUtf8("<p><em>(<strong>foo</strong>)</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 51")
        << QString::fromUtf8("__foo__bar\n")
        << QString::fromUtf8("<p>__foo__bar</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 52")
        << QString::fromUtf8("__пристаням__стремятся\n")
        << QString::fromUtf8("<p>__пристаням__стремятся</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 53")
        << QString::fromUtf8("__foo__bar__baz__\n")
        << QString::fromUtf8("<p><strong>foo__bar__baz</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 54")
        << QString::fromUtf8("__(bar)__.\n")
        << QString::fromUtf8("<p><strong>(bar)</strong>.</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 55")
        << QString::fromUtf8("*foo [bar](/url)*\n")
        << QString::fromUtf8("<p><em>foo <a href=\"/url\">bar</a></em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 56")
        << QString::fromUtf8("*foo\nbar*\n")
        << QString::fromUtf8("<p><em>foo\nbar</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 57")
        << QString::fromUtf8("_foo __bar__ baz_\n")
        << QString::fromUtf8("<p><em>foo <strong>bar</strong> baz</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 58")
        << QString::fromUtf8("_foo _bar_ baz_\n")
        << QString::fromUtf8("<p><em>foo <em>bar</em> baz</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 59")
        << QString::fromUtf8("__foo_ bar_\n")
        << QString::fromUtf8("<p><em><em>foo</em> bar</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 60")
        << QString::fromUtf8("*foo *bar**\n")
        << QString::fromUtf8("<p><em>foo <em>bar</em></em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 61")
        << QString::fromUtf8("*foo **bar** baz*\n")
        << QString::fromUtf8("<p><em>foo <strong>bar</strong> baz</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 62")
        << QString::fromUtf8("*foo**bar**baz*\n")
        << QString::fromUtf8("<p><em>foo<strong>bar</strong>baz</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 63")
        << QString::fromUtf8("*foo**bar*\n")
        << QString::fromUtf8("<p><em>foo**bar</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 64")
        << QString::fromUtf8("***foo** bar*\n")
        << QString::fromUtf8("<p><em><strong>foo</strong> bar</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 65")
        << QString::fromUtf8("*foo **bar***\n")
        << QString::fromUtf8("<p><em>foo <strong>bar</strong></em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 66")
        << QString::fromUtf8("*foo**bar***\n")
        << QString::fromUtf8("<p><em>foo<strong>bar</strong></em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 67")
        << QString::fromUtf8("foo***bar***baz\n")
        << QString::fromUtf8("<p>foo<em><strong>bar</strong></em>baz</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 68")
        << QString::fromUtf8("foo******bar*********baz\n")
        << QString::fromUtf8("<p>foo<strong><strong><strong>bar</strong></strong></strong>***baz</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 69")
        << QString::fromUtf8("*foo **bar *baz* bim** bop*\n")
        << QString::fromUtf8("<p><em>foo <strong>bar <em>baz</em> bim</strong> bop</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 70")
        << QString::fromUtf8("*foo [*bar*](/url)*\n")
        << QString::fromUtf8("<p><em>foo <a href=\"/url\"><em>bar</em></a></em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 71")
        << QString::fromUtf8("** is not an empty emphasis\n")
        << QString::fromUtf8("<p>** is not an empty emphasis</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 72")
        << QString::fromUtf8("**** is not an empty strong emphasis\n")
        << QString::fromUtf8("<p>**** is not an empty strong emphasis</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 73")
        << QString::fromUtf8("**foo [bar](/url)**\n")
        << QString::fromUtf8("<p><strong>foo <a href=\"/url\">bar</a></strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 74")
        << QString::fromUtf8("**foo\nbar**\n")
        << QString::fromUtf8("<p><strong>foo\nbar</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 75")
        << QString::fromUtf8("__foo _bar_ baz__\n")
        << QString::fromUtf8("<p><strong>foo <em>bar</em> baz</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 76")
        << QString::fromUtf8("__foo __bar__ baz__\n")
        << QString::fromUtf8("<p><strong>foo <strong>bar</strong> baz</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 77")
        << QString::fromUtf8("____foo__ bar__\n")
        << QString::fromUtf8("<p><strong><strong>foo</strong> bar</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 78")
        << QString::fromUtf8("**foo **bar****\n")
        << QString::fromUtf8("<p><strong>foo <strong>bar</strong></strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 79")
        << QString::fromUtf8("**foo *bar* baz**\n")
        << QString::fromUtf8("<p><strong>foo <em>bar</em> baz</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 80")
        << QString::fromUtf8("**foo*bar*baz**\n")
        << QString::fromUtf8("<p><strong>foo<em>bar</em>baz</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 81")
        << QString::fromUtf8("***foo* bar**\n")
        << QString::fromUtf8("<p><strong><em>foo</em> bar</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 82")
        << QString::fromUtf8("**foo *bar***\n")
        << QString::fromUtf8("<p><strong>foo <em>bar</em></strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 83")
        << QString::fromUtf8("**foo *bar **baz**\nbim* bop**\n")
        << QString::fromUtf8("<p><strong>foo <em>bar <strong>baz</strong>\nbim</em> bop</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 84")
        << QString::fromUtf8("**foo [*bar*](/url)**\n")
        << QString::fromUtf8("<p><strong>foo <a href=\"/url\"><em>bar</em></a></strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 85")
        << QString::fromUtf8("__ is not an empty emphasis\n")
        << QString::fromUtf8("<p>__ is not an empty emphasis</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 86")
        << QString::fromUtf8("____ is not an empty strong emphasis\n")
        << QString::fromUtf8("<p>____ is not an empty strong emphasis</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 87")
        << QString::fromUtf8("foo ***\n")
        << QString::fromUtf8("<p>foo ***</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 88")
        << QString::fromUtf8("foo *\\**\n")
        << QString::fromUtf8("<p>foo <em>*</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 89")
        << QString::fromUtf8("foo *_*\n")
        << QString::fromUtf8("<p>foo <em>_</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 90")
        << QString::fromUtf8("foo *****\n")
        << QString::fromUtf8("<p>foo *****</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 91")
        << QString::fromUtf8("foo **\\***\n")
        << QString::fromUtf8("<p>foo <strong>*</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 92")
        << QString::fromUtf8("foo **_**\n")
        << QString::fromUtf8("<p>foo <strong>_</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 93")
        << QString::fromUtf8("**foo*\n")
        << QString::fromUtf8("<p>*<em>foo</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 94")
        << QString::fromUtf8("*foo**\n")
        << QString::fromUtf8("<p><em>foo</em>*</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 95")
        << QString::fromUtf8("***foo**\n")
        << QString::fromUtf8("<p>*<strong>foo</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 96")
        << QString::fromUtf8("****foo*\n")
        << QString::fromUtf8("<p>***<em>foo</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 97")
        << QString::fromUtf8("**foo***\n")
        << QString::fromUtf8("<p><strong>foo</strong>*</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 98")
        << QString::fromUtf8("*foo****\n")
        << QString::fromUtf8("<p><em>foo</em>***</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 99")
        << QString::fromUtf8("foo ___\n")
        << QString::fromUtf8("<p>foo ___</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 100")
        << QString::fromUtf8("foo _\\__\n")
        << QString::fromUtf8("<p>foo <em>_</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 101")
        << QString::fromUtf8("foo _*_\n")
        << QString::fromUtf8("<p>foo <em>*</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 102")
        << QString::fromUtf8("foo _____\n")
        << QString::fromUtf8("<p>foo _____</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 103")
        << QString::fromUtf8("foo __\\___\n")
        << QString::fromUtf8("<p>foo <strong>_</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 104")
        << QString::fromUtf8("foo __*__\n")
        << QString::fromUtf8("<p>foo <strong>*</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 105")
        << QString::fromUtf8("__foo_\n")
        << QString::fromUtf8("<p>_<em>foo</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 106")
        << QString::fromUtf8("_foo__\n")
        << QString::fromUtf8("<p><em>foo</em>_</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 107")
        << QString::fromUtf8("___foo__\n")
        << QString::fromUtf8("<p>_<strong>foo</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 108")
        << QString::fromUtf8("____foo_\n")
        << QString::fromUtf8("<p>___<em>foo</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 109")
        << QString::fromUtf8("__foo___\n")
        << QString::fromUtf8("<p><strong>foo</strong>_</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 110")
        << QString::fromUtf8("_foo____\n")
        << QString::fromUtf8("<p><em>foo</em>___</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 111")
        << QString::fromUtf8("**foo**\n")
        << QString::fromUtf8("<p><strong>foo</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 112")
        << QString::fromUtf8("*_foo_*\n")
        << QString::fromUtf8("<p><em><em>foo</em></em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 113")
        << QString::fromUtf8("__foo__\n")
        << QString::fromUtf8("<p><strong>foo</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 114")
        << QString::fromUtf8("_*foo*_\n")
        << QString::fromUtf8("<p><em><em>foo</em></em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 115")
        << QString::fromUtf8("****foo****\n")
        << QString::fromUtf8("<p><strong><strong>foo</strong></strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 116")
        << QString::fromUtf8("____foo____\n")
        << QString::fromUtf8("<p><strong><strong>foo</strong></strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 117")
        << QString::fromUtf8("******foo******\n")
        << QString::fromUtf8("<p><strong><strong><strong>foo</strong></strong></strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 118")
        << QString::fromUtf8("***foo***\n")
        << QString::fromUtf8("<p><em><strong>foo</strong></em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 119")
        << QString::fromUtf8("_____foo_____\n")
        << QString::fromUtf8("<p><em><strong><strong>foo</strong></strong></em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 120")
        << QString::fromUtf8("*foo _bar* baz_\n")
        << QString::fromUtf8("<p><em>foo _bar</em> baz_</p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 121")
        << QString::fromUtf8("*foo __bar *baz bim__ bam*\n")
        << QString::fromUtf8("<p><em>foo <strong>bar *baz bim</strong> bam</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 122")
        << QString::fromUtf8("**foo **bar baz**\n")
        << QString::fromUtf8("<p>**foo <strong>bar baz</strong></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 123")
        << QString::fromUtf8("*foo *bar baz*\n")
        << QString::fromUtf8("<p>*foo <em>bar baz</em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 124")
        << QString::fromUtf8("*[bar*](/url)\n")
        << QString::fromUtf8("<p>*<a href=\"/url\">bar*</a></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 125")
        << QString::fromUtf8("_foo [bar_](/url)\n")
        << QString::fromUtf8("<p>_foo <a href=\"/url\">bar_</a></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 126")
        << QString::fromUtf8("*<img src=\"foo\" title=\"*\"/>\n")
        << QString::fromUtf8("<p>*<img src=\"foo\" title=\"*\"/></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 127")
        << QString::fromUtf8("**<a href=\"**\">\n")
        << QString::fromUtf8("<p>**<a href=\"**\"></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 128")
        << QString::fromUtf8("__<a href=\"__\">\n")
        << QString::fromUtf8("<p>__<a href=\"__\"></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 129")
        << QString::fromUtf8("*a `*`*\n")
        << QString::fromUtf8("<p><em>a <code>*</code></em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 130")
        << QString::fromUtf8("_a `_`_\n")
        << QString::fromUtf8("<p><em>a <code>_</code></em></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 131")
        << QString::fromUtf8("**a<https://foo.bar/\?q=**>\n")
        << QString::fromUtf8("<p>**a<a href=\"https://foo.bar/\?q=**\">https://foo.bar/\?q=**</a></p>");
    QTest::newRow("CommonMark: Emphasis and strong emphasis 132")
        << QString::fromUtf8("__a<https://foo.bar/\?q=__>\n")
        << QString::fromUtf8("<p>__a<a href=\"https://foo.bar/\?q=__\">https://foo.bar/\?q=__</a></p>");
    QTest::newRow("CommonMark: Links 1")
        << QString::fromUtf8("[link](/uri \"title\")\n")
        << QString::fromUtf8("<p><a href=\"/uri\" title=\"title\">link</a></p>");
    QTest::newRow("CommonMark: Links 2")
        << QString::fromUtf8("[link](/uri)\n")
        << QString::fromUtf8("<p><a href=\"/uri\">link</a></p>");
    QTest::newRow("CommonMark: Links 3")
        << QString::fromUtf8("[](./target.md)\n")
        << QString::fromUtf8("<p><a href=\"./target.md\"></a></p>");
    QTest::newRow("CommonMark: Links 4")
        << QString::fromUtf8("[link]()\n")
        << QString::fromUtf8("<p><a href=\"\">link</a></p>");
    QTest::newRow("CommonMark: Links 5")
        << QString::fromUtf8("[link](<>)\n")
        << QString::fromUtf8("<p><a href=\"\">link</a></p>");
    QTest::newRow("CommonMark: Links 6")
        << QString::fromUtf8("[]()\n")
        << QString::fromUtf8("<p><a href=\"\"></a></p>");
    QTest::newRow("CommonMark: Links 7")
        << QString::fromUtf8("[link](/my uri)\n")
        << QString::fromUtf8("<p>[link](/my uri)</p>");
    QTest::newRow("CommonMark: Links 8")
        << QString::fromUtf8("[link](</my uri>)\n")
        << QString::fromUtf8("<p><a href=\"/my%20uri\">link</a></p>");
    QTest::newRow("CommonMark: Links 9")
        << QString::fromUtf8("[link](foo\nbar)\n")
        << QString::fromUtf8("<p>[link](foo\nbar)</p>");
    QTest::newRow("CommonMark: Links 10")
        << QString::fromUtf8("[link](<foo\nbar>)\n")
        << QString::fromUtf8("<p>[link](<foo\nbar>)</p>");
    QTest::newRow("CommonMark: Links 11")
        << QString::fromUtf8("[a](<b)c>)\n")
        << QString::fromUtf8("<p><a href=\"b)c\">a</a></p>");
    QTest::newRow("CommonMark: Links 12")
        << QString::fromUtf8("[link](<foo\\>)\n")
        << QString::fromUtf8("<p>[link](&lt;foo&gt;)</p>");
    QTest::newRow("CommonMark: Links 13")
        << QString::fromUtf8("[a](<b)c\n[a](<b)c>\n[a](<b>c)\n")
        << QString::fromUtf8("<p>[a](&lt;b)c\n[a](&lt;b)c&gt;\n[a](<b>c)</p>");
    QTest::newRow("CommonMark: Links 14")
        << QString::fromUtf8("[link](\\(foo\\))\n")
        << QString::fromUtf8("<p><a href=\"(foo)\">link</a></p>");
    QTest::newRow("CommonMark: Links 15")
        << QString::fromUtf8("[link](foo(and(bar)))\n")
        << QString::fromUtf8("<p><a href=\"foo(and(bar))\">link</a></p>");
    QTest::newRow("CommonMark: Links 16")
        << QString::fromUtf8("[link](foo(and(bar))\n")
        << QString::fromUtf8("<p>[link](foo(and(bar))</p>");
    QTest::newRow("CommonMark: Links 17")
        << QString::fromUtf8("[link](foo\\(and\\(bar\\))\n")
        << QString::fromUtf8("<p><a href=\"foo(and(bar)\">link</a></p>");
    QTest::newRow("CommonMark: Links 18")
        << QString::fromUtf8("[link](<foo(and(bar)>)\n")
        << QString::fromUtf8("<p><a href=\"foo(and(bar)\">link</a></p>");
    QTest::newRow("CommonMark: Links 19")
        << QString::fromUtf8("[link](foo\\)\\:)\n")
        << QString::fromUtf8("<p><a href=\"foo):\">link</a></p>");
    QTest::newRow("CommonMark: Links 20")
        << QString::fromUtf8("[link](#fragment)\n\n[link](https://example.com#fragment)\n\n[link](https://example.com\?foo=3#frag)\n")
        << QString::fromUtf8("<p><a href=\"#fragment\">link</a></p>\n<p><a href=\"https://example.com#fragment\">link</a></p>\n<p><a href=\"https://example.com\?foo=3#frag\">link</a></p>");
    QTest::newRow("CommonMark: Links 21")
        << QString::fromUtf8("[link](foo\\bar)\n")
        << QString::fromUtf8("<p><a href=\"foo%5Cbar\">link</a></p>");
    QTest::newRow("CommonMark: Links 22")
        << QString::fromUtf8("[link](foo%20b&auml;)\n")
        << QString::fromUtf8("<p><a href=\"foo%20b%C3%A4\">link</a></p>");
    QTest::newRow("CommonMark: Links 23")
        << QString::fromUtf8("[link](\"title\")\n")
        << QString::fromUtf8("<p><a href=\"%22title%22\">link</a></p>");
    QTest::newRow("CommonMark: Links 24")
        << QString::fromUtf8("[link](/url \"title\")\n[link](/url 'title')\n[link](/url (title))\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title\">link</a>\n<a href=\"/url\" title=\"title\">link</a>\n<a href=\"/url\" title=\"title\">link</a></p>");
    QTest::newRow("CommonMark: Links 25")
        << QString::fromUtf8("[link](/url \"title \\\"&quot;\")\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title &quot;&quot;\">link</a></p>");
    QTest::newRow("CommonMark: Links 26")
        << QString::fromUtf8("[link](/url \"title\")\n")
        << QString::fromUtf8("<p><a href=\"/url%C2%A0%22title%22\">link</a></p>");
    QTest::newRow("CommonMark: Links 27")
        << QString::fromUtf8("[link](/url \"title \"and\" title\")\n")
        << QString::fromUtf8("<p>[link](/url &quot;title &quot;and&quot; title&quot;)</p>");
    QTest::newRow("CommonMark: Links 28")
        << QString::fromUtf8("[link](/url 'title \"and\" title')\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title &quot;and&quot; title\">link</a></p>");
    QTest::newRow("CommonMark: Links 29")
        << QString::fromUtf8("[link](   /uri\n  \"title\"  )\n")
        << QString::fromUtf8("<p><a href=\"/uri\" title=\"title\">link</a></p>");
    QTest::newRow("CommonMark: Links 30")
        << QString::fromUtf8("[link] (/uri)\n")
        << QString::fromUtf8("<p>[link] (/uri)</p>");
    QTest::newRow("CommonMark: Links 31")
        << QString::fromUtf8("[link [foo [bar]]](/uri)\n")
        << QString::fromUtf8("<p><a href=\"/uri\">link [foo [bar]]</a></p>");
    QTest::newRow("CommonMark: Links 32")
        << QString::fromUtf8("[link] bar](/uri)\n")
        << QString::fromUtf8("<p>[link] bar](/uri)</p>");
    QTest::newRow("CommonMark: Links 33")
        << QString::fromUtf8("[link [bar](/uri)\n")
        << QString::fromUtf8("<p>[link <a href=\"/uri\">bar</a></p>");
    QTest::newRow("CommonMark: Links 34")
        << QString::fromUtf8("[link \\[bar](/uri)\n")
        << QString::fromUtf8("<p><a href=\"/uri\">link [bar</a></p>");
    QTest::newRow("CommonMark: Links 35")
        << QString::fromUtf8("[link *foo **bar** `#`*](/uri)\n")
        << QString::fromUtf8("<p><a href=\"/uri\">link <em>foo <strong>bar</strong> <code>#</code></em></a></p>");
    QTest::newRow("CommonMark: Links 36")
        << QString::fromUtf8("[![moon](moon.jpg)](/uri)\n")
        << QString::fromUtf8("<p><a href=\"/uri\"><img src=\"moon.jpg\" alt=\"moon\"></a></p>");
    QTest::newRow("CommonMark: Links 37")
        << QString::fromUtf8("[foo [bar](/uri)](/uri)\n")
        << QString::fromUtf8("<p>[foo <a href=\"/uri\">bar</a>](/uri)</p>");
    QTest::newRow("CommonMark: Links 38")
        << QString::fromUtf8("[foo *[bar [baz](/uri)](/uri)*](/uri)\n")
        << QString::fromUtf8("<p>[foo <em>[bar <a href=\"/uri\">baz</a>](/uri)</em>](/uri)</p>");
    QTest::newRow("CommonMark: Links 39")
        << QString::fromUtf8("![[[foo](uri1)](uri2)](uri3)\n")
        << QString::fromUtf8("<p><img src=\"uri3\" alt=\"[foo](uri2)\"></p>");
    QTest::newRow("CommonMark: Links 40")
        << QString::fromUtf8("*[foo*](/uri)\n")
        << QString::fromUtf8("<p>*<a href=\"/uri\">foo*</a></p>");
    QTest::newRow("CommonMark: Links 41")
        << QString::fromUtf8("[foo *bar](baz*)\n")
        << QString::fromUtf8("<p><a href=\"baz*\">foo *bar</a></p>");
    QTest::newRow("CommonMark: Links 42")
        << QString::fromUtf8("*foo [bar* baz]\n")
        << QString::fromUtf8("<p><em>foo [bar</em> baz]</p>");
    QTest::newRow("CommonMark: Links 43")
        << QString::fromUtf8("[foo <bar attr=\"](baz)\">\n")
        << QString::fromUtf8("<p>[foo <bar attr=\"](baz)\"></p>");
    QTest::newRow("CommonMark: Links 44")
        << QString::fromUtf8("[foo`](/uri)`\n")
        << QString::fromUtf8("<p>[foo<code>](/uri)</code></p>");
    QTest::newRow("CommonMark: Links 45")
        << QString::fromUtf8("[foo<https://example.com/\?search=](uri)>\n")
        << QString::fromUtf8("<p>[foo<a href=\"https://example.com/\?search=%5D(uri)\">https://example.com/\?search=](uri)</a></p>");
    QTest::newRow("CommonMark: Links 46")
        << QString::fromUtf8("[foo][bar]\n\n[bar]: /url \"title\"\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title\">foo</a></p>");
    QTest::newRow("CommonMark: Links 47")
        << QString::fromUtf8("[link [foo [bar]]][ref]\n\n[ref]: /uri\n")
        << QString::fromUtf8("<p><a href=\"/uri\">link [foo [bar]]</a></p>");
    QTest::newRow("CommonMark: Links 48")
        << QString::fromUtf8("[link \\[bar][ref]\n\n[ref]: /uri\n")
        << QString::fromUtf8("<p><a href=\"/uri\">link [bar</a></p>");
    QTest::newRow("CommonMark: Links 49")
        << QString::fromUtf8("[link *foo **bar** `#`*][ref]\n\n[ref]: /uri\n")
        << QString::fromUtf8("<p><a href=\"/uri\">link <em>foo <strong>bar</strong> <code>#</code></em></a></p>");
    QTest::newRow("CommonMark: Links 50")
        << QString::fromUtf8("[![moon](moon.jpg)][ref]\n\n[ref]: /uri\n")
        << QString::fromUtf8("<p><a href=\"/uri\"><img src=\"moon.jpg\" alt=\"moon\"></a></p>");
    QTest::newRow("CommonMark: Links 51")
        << QString::fromUtf8("[foo [bar](/uri)][ref]\n\n[ref]: /uri\n")
        << QString::fromUtf8("<p>[foo <a href=\"/uri\">bar</a>]<a href=\"/uri\">ref</a></p>");
    QTest::newRow("CommonMark: Links 52")
        << QString::fromUtf8("[foo *bar [baz][ref]*][ref]\n\n[ref]: /uri\n")
        << QString::fromUtf8("<p>[foo <em>bar <a href=\"/uri\">baz</a></em>]<a href=\"/uri\">ref</a></p>");
    QTest::newRow("CommonMark: Links 53")
        << QString::fromUtf8("*[foo*][ref]\n\n[ref]: /uri\n")
        << QString::fromUtf8("<p>*<a href=\"/uri\">foo*</a></p>");
    QTest::newRow("CommonMark: Links 54")
        << QString::fromUtf8("[foo *bar][ref]*\n\n[ref]: /uri\n")
        << QString::fromUtf8("<p><a href=\"/uri\">foo *bar</a>*</p>");
    QTest::newRow("CommonMark: Links 55")
        << QString::fromUtf8("[foo <bar attr=\"][ref]\">\n\n[ref]: /uri\n")
        << QString::fromUtf8("<p>[foo <bar attr=\"][ref]\"></p>");
    QTest::newRow("CommonMark: Links 56")
        << QString::fromUtf8("[foo`][ref]`\n\n[ref]: /uri\n")
        << QString::fromUtf8("<p>[foo<code>][ref]</code></p>");
    QTest::newRow("CommonMark: Links 57")
        << QString::fromUtf8("[foo<https://example.com/\?search=][ref]>\n\n[ref]: /uri\n")
        << QString::fromUtf8("<p>[foo<a href=\"https://example.com/\?search=%5D%5Bref%5D\">https://example.com/\?search=][ref]</a></p>");
    QTest::newRow("CommonMark: Links 58")
        << QString::fromUtf8("[foo][BaR]\n\n[bar]: /url \"title\"\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title\">foo</a></p>");
    QTest::newRow("CommonMark: Links 59")
        << QString::fromUtf8("[ẞ]\n\n[SS]: /url\n")
        << QString::fromUtf8("<p><a href=\"/url\">ẞ</a></p>");
    QTest::newRow("CommonMark: Links 60")
        << QString::fromUtf8("[Foo\n  bar]: /url\n\n[Baz][Foo bar]\n")
        << QString::fromUtf8("<p><a href=\"/url\">Baz</a></p>");
    QTest::newRow("CommonMark: Links 61")
        << QString::fromUtf8("[foo] [bar]\n\n[bar]: /url \"title\"\n")
        << QString::fromUtf8("<p>[foo] <a href=\"/url\" title=\"title\">bar</a></p>");
    QTest::newRow("CommonMark: Links 62")
        << QString::fromUtf8("[foo]\n[bar]\n\n[bar]: /url \"title\"\n")
        << QString::fromUtf8("<p>[foo]\n<a href=\"/url\" title=\"title\">bar</a></p>");
    QTest::newRow("CommonMark: Links 63")
        << QString::fromUtf8("[foo]: /url1\n\n[foo]: /url2\n\n[bar][foo]\n")
        << QString::fromUtf8("<p><a href=\"/url1\">bar</a></p>");
    QTest::newRow("CommonMark: Links 64")
        << QString::fromUtf8("[bar][foo\\!]\n\n[foo!]: /url\n")
        << QString::fromUtf8("<p>[bar][foo!]</p>");
    QTest::newRow("CommonMark: Links 65")
        << QString::fromUtf8("[foo][ref[]\n\n[ref[]: /uri\n")
        << QString::fromUtf8("<p>[foo][ref[]</p>\n<p>[ref[]: /uri</p>");
    QTest::newRow("CommonMark: Links 66")
        << QString::fromUtf8("[foo][ref[bar]]\n\n[ref[bar]]: /uri\n")
        << QString::fromUtf8("<p>[foo][ref[bar]]</p>\n<p>[ref[bar]]: /uri</p>");
    QTest::newRow("CommonMark: Links 67")
        << QString::fromUtf8("[[[foo]]]\n\n[[[foo]]]: /url\n")
        << QString::fromUtf8("<p>[[[foo]]]</p>\n<p>[[[foo]]]: /url</p>");
    QTest::newRow("CommonMark: Links 68")
        << QString::fromUtf8("[foo][ref\\[]\n\n[ref\\[]: /uri\n")
        << QString::fromUtf8("<p><a href=\"/uri\">foo</a></p>");
    QTest::newRow("CommonMark: Links 69")
        << QString::fromUtf8("[bar\\\\]: /uri\n\n[bar\\\\]\n")
        << QString::fromUtf8("<p><a href=\"/uri\">bar\\</a></p>");
    QTest::newRow("CommonMark: Links 70")
        << QString::fromUtf8("[]\n\n[]: /uri\n")
        << QString::fromUtf8("<p>[]</p>\n<p>[]: /uri</p>");
    QTest::newRow("CommonMark: Links 71")
        << QString::fromUtf8("[\n ]\n\n[\n ]: /uri\n")
        << QString::fromUtf8("<p>[\n]</p>\n<p>[\n]: /uri</p>");
    QTest::newRow("CommonMark: Links 72")
        << QString::fromUtf8("[foo][]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title\">foo</a></p>");
    QTest::newRow("CommonMark: Links 73")
        << QString::fromUtf8("[*foo* bar][]\n\n[*foo* bar]: /url \"title\"\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title\"><em>foo</em> bar</a></p>");
    QTest::newRow("CommonMark: Links 74")
        << QString::fromUtf8("[Foo][]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title\">Foo</a></p>");
    QTest::newRow("CommonMark: Links 75")
        << QString::fromUtf8("[foo] \n[]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title\">foo</a>\n[]</p>");
    QTest::newRow("CommonMark: Links 76")
        << QString::fromUtf8("[foo]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title\">foo</a></p>");
    QTest::newRow("CommonMark: Links 77")
        << QString::fromUtf8("[*foo* bar]\n\n[*foo* bar]: /url \"title\"\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title\"><em>foo</em> bar</a></p>");
    QTest::newRow("CommonMark: Links 78")
        << QString::fromUtf8("[[*foo* bar]]\n\n[*foo* bar]: /url \"title\"\n")
        << QString::fromUtf8("<p>[<a href=\"/url\" title=\"title\"><em>foo</em> bar</a>]</p>");
    QTest::newRow("CommonMark: Links 79")
        << QString::fromUtf8("[[bar [foo]\n\n[foo]: /url\n")
        << QString::fromUtf8("<p>[[bar <a href=\"/url\">foo</a></p>");
    QTest::newRow("CommonMark: Links 80")
        << QString::fromUtf8("[Foo]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p><a href=\"/url\" title=\"title\">Foo</a></p>");
    QTest::newRow("CommonMark: Links 81")
        << QString::fromUtf8("[foo] bar\n\n[foo]: /url\n")
        << QString::fromUtf8("<p><a href=\"/url\">foo</a> bar</p>");
    QTest::newRow("CommonMark: Links 82")
        << QString::fromUtf8("\\[foo]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p>[foo]</p>");
    QTest::newRow("CommonMark: Links 83")
        << QString::fromUtf8("[foo*]: /url\n\n*[foo*]\n")
        << QString::fromUtf8("<p>*<a href=\"/url\">foo*</a></p>");
    QTest::newRow("CommonMark: Links 84")
        << QString::fromUtf8("[foo][bar]\n\n[foo]: /url1\n[bar]: /url2\n")
        << QString::fromUtf8("<p><a href=\"/url2\">foo</a></p>");
    QTest::newRow("CommonMark: Links 85")
        << QString::fromUtf8("[foo][]\n\n[foo]: /url1\n")
        << QString::fromUtf8("<p><a href=\"/url1\">foo</a></p>");
    QTest::newRow("CommonMark: Links 86")
        << QString::fromUtf8("[foo]()\n\n[foo]: /url1\n")
        << QString::fromUtf8("<p><a href=\"\">foo</a></p>");
    QTest::newRow("CommonMark: Links 87")
        << QString::fromUtf8("[foo](not a link)\n\n[foo]: /url1\n")
        << QString::fromUtf8("<p><a href=\"/url1\">foo</a>(not a link)</p>");
    QTest::newRow("CommonMark: Links 88")
        << QString::fromUtf8("[foo][bar][baz]\n\n[baz]: /url\n")
        << QString::fromUtf8("<p>[foo]<a href=\"/url\">bar</a></p>");
    QTest::newRow("CommonMark: Links 89")
        << QString::fromUtf8("[foo][bar][baz]\n\n[baz]: /url1\n[bar]: /url2\n")
        << QString::fromUtf8("<p><a href=\"/url2\">foo</a><a href=\"/url1\">baz</a></p>");
    QTest::newRow("CommonMark: Links 90")
        << QString::fromUtf8("[foo][bar][baz]\n\n[baz]: /url1\n[foo]: /url2\n")
        << QString::fromUtf8("<p>[foo]<a href=\"/url1\">bar</a></p>");
    QTest::newRow("CommonMark: Images 1")
        << QString::fromUtf8("![foo](/url \"title\")\n")
        << QString::fromUtf8("<p><img src=\"/url\" alt=\"foo\" title=\"title\"></p>");
    QTest::newRow("CommonMark: Images 2")
        << QString::fromUtf8("![foo *bar*]\n\n[foo *bar*]: train.jpg \"train & tracks\"\n")
        << QString::fromUtf8("<p><img src=\"train.jpg\" alt=\"foo bar\" title=\"train &amp; tracks\"></p>");
    QTest::newRow("CommonMark: Images 3")
        << QString::fromUtf8("![foo ![bar](/url)](/url2)\n")
        << QString::fromUtf8("<p><img src=\"/url2\" alt=\"foo bar\"></p>");
    QTest::newRow("CommonMark: Images 4")
        << QString::fromUtf8("![foo [bar](/url)](/url2)\n")
        << QString::fromUtf8("<p><img src=\"/url2\" alt=\"foo bar\"></p>");
    QTest::newRow("CommonMark: Images 5")
        << QString::fromUtf8("![foo *bar*][]\n\n[foo *bar*]: train.jpg \"train & tracks\"\n")
        << QString::fromUtf8("<p><img src=\"train.jpg\" alt=\"foo bar\" title=\"train &amp; tracks\"></p>");
    QTest::newRow("CommonMark: Images 6")
        << QString::fromUtf8("![foo *bar*][foobar]\n\n[FOOBAR]: train.jpg \"train & tracks\"\n")
        << QString::fromUtf8("<p><img src=\"train.jpg\" alt=\"foo bar\" title=\"train &amp; tracks\"></p>");
    QTest::newRow("CommonMark: Images 7")
        << QString::fromUtf8("![foo](train.jpg)\n")
        << QString::fromUtf8("<p><img src=\"train.jpg\" alt=\"foo\"></p>");
    QTest::newRow("CommonMark: Images 8")
        << QString::fromUtf8("My ![foo bar](/path/to/train.jpg  \"title\"   )\n")
        << QString::fromUtf8("<p>My <img src=\"/path/to/train.jpg\" alt=\"foo bar\" title=\"title\"></p>");
    QTest::newRow("CommonMark: Images 9")
        << QString::fromUtf8("![foo](<url>)\n")
        << QString::fromUtf8("<p><img src=\"url\" alt=\"foo\"></p>");
    QTest::newRow("CommonMark: Images 10")
        << QString::fromUtf8("![](/url)\n")
        << QString::fromUtf8("<p><img src=\"/url\" alt=\"\"></p>");
    QTest::newRow("CommonMark: Images 11")
        << QString::fromUtf8("![foo][bar]\n\n[bar]: /url\n")
        << QString::fromUtf8("<p><img src=\"/url\" alt=\"foo\"></p>");
    QTest::newRow("CommonMark: Images 12")
        << QString::fromUtf8("![foo][bar]\n\n[BAR]: /url\n")
        << QString::fromUtf8("<p><img src=\"/url\" alt=\"foo\"></p>");
    QTest::newRow("CommonMark: Images 13")
        << QString::fromUtf8("![foo][]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p><img src=\"/url\" alt=\"foo\" title=\"title\"></p>");
    QTest::newRow("CommonMark: Images 14")
        << QString::fromUtf8("![*foo* bar][]\n\n[*foo* bar]: /url \"title\"\n")
        << QString::fromUtf8("<p><img src=\"/url\" alt=\"foo bar\" title=\"title\"></p>");
    QTest::newRow("CommonMark: Images 15")
        << QString::fromUtf8("![Foo][]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p><img src=\"/url\" alt=\"Foo\" title=\"title\"></p>");
    QTest::newRow("CommonMark: Images 16")
        << QString::fromUtf8("![foo] \n[]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p><img src=\"/url\" alt=\"foo\" title=\"title\">\n[]</p>");
    QTest::newRow("CommonMark: Images 17")
        << QString::fromUtf8("![foo]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p><img src=\"/url\" alt=\"foo\" title=\"title\"></p>");
    QTest::newRow("CommonMark: Images 18")
        << QString::fromUtf8("![*foo* bar]\n\n[*foo* bar]: /url \"title\"\n")
        << QString::fromUtf8("<p><img src=\"/url\" alt=\"foo bar\" title=\"title\"></p>");
    QTest::newRow("CommonMark: Images 19")
        << QString::fromUtf8("![[foo]]\n\n[[foo]]: /url \"title\"\n")
        << QString::fromUtf8("<p>![[foo]]</p>\n<p>[[foo]]: /url &quot;title&quot;</p>");
    QTest::newRow("CommonMark: Images 20")
        << QString::fromUtf8("![Foo]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p><img src=\"/url\" alt=\"Foo\" title=\"title\"></p>");
    QTest::newRow("CommonMark: Images 21")
        << QString::fromUtf8("!\\[foo]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p>![foo]</p>");
    QTest::newRow("CommonMark: Images 22")
        << QString::fromUtf8("\\![foo]\n\n[foo]: /url \"title\"\n")
        << QString::fromUtf8("<p>!<a href=\"/url\" title=\"title\">foo</a></p>");
    QTest::newRow("CommonMark: Autolinks 1")
        << QString::fromUtf8("<http://foo.bar.baz>\n")
        << QString::fromUtf8("<p><a href=\"http://foo.bar.baz\">http://foo.bar.baz</a></p>");
    QTest::newRow("CommonMark: Autolinks 2")
        << QString::fromUtf8("<https://foo.bar.baz/test\?q=hello&id=22&boolean>\n")
        << QString::fromUtf8("<p><a href=\"https://foo.bar.baz/test\?q=hello&amp;id=22&amp;boolean\">https://foo.bar.baz/test\?q=hello&amp;id=22&amp;boolean</a></p>");
    QTest::newRow("CommonMark: Autolinks 3")
        << QString::fromUtf8("<irc://foo.bar:2233/baz>\n")
        << QString::fromUtf8("<p><a href=\"irc://foo.bar:2233/baz\">irc://foo.bar:2233/baz</a></p>");
    QTest::newRow("CommonMark: Autolinks 4")
        << QString::fromUtf8("<MAILTO:FOO@BAR.BAZ>\n")
        << QString::fromUtf8("<p><a href=\"MAILTO:FOO@BAR.BAZ\">MAILTO:FOO@BAR.BAZ</a></p>");
    QTest::newRow("CommonMark: Autolinks 5")
        << QString::fromUtf8("<a+b+c:d>\n")
        << QString::fromUtf8("<p><a href=\"a+b+c:d\">a+b+c:d</a></p>");
    QTest::newRow("CommonMark: Autolinks 6")
        << QString::fromUtf8("<made-up-scheme://foo,bar>\n")
        << QString::fromUtf8("<p><a href=\"made-up-scheme://foo,bar\">made-up-scheme://foo,bar</a></p>");
    QTest::newRow("CommonMark: Autolinks 7")
        << QString::fromUtf8("<https://../>\n")
        << QString::fromUtf8("<p><a href=\"https://../\">https://../</a></p>");
    QTest::newRow("CommonMark: Autolinks 8")
        << QString::fromUtf8("<localhost:5001/foo>\n")
        << QString::fromUtf8("<p><a href=\"localhost:5001/foo\">localhost:5001/foo</a></p>");
    QTest::newRow("CommonMark: Autolinks 10")
        << QString::fromUtf8("<https://example.com/\\[\\>\n")
        << QString::fromUtf8("<p><a href=\"https://example.com/%5C%5B%5C\">https://example.com/\\[\\</a></p>");
    QTest::newRow("CommonMark: Autolinks 11")
        << QString::fromUtf8("<foo@bar.example.com>\n")
        << QString::fromUtf8("<p><a href=\"mailto:foo@bar.example.com\">foo@bar.example.com</a></p>");
    QTest::newRow("CommonMark: Autolinks 12")
        << QString::fromUtf8("<foo+special@Bar.baz-bar0.com>\n")
        << QString::fromUtf8("<p><a href=\"mailto:foo+special@Bar.baz-bar0.com\">foo+special@Bar.baz-bar0.com</a></p>");
    QTest::newRow("CommonMark: Autolinks 14")
        << QString::fromUtf8("<>\n")
        << QString::fromUtf8("<p>&lt;&gt;</p>");
    QTest::newRow("CommonMark: Autolinks 16")
        << QString::fromUtf8("<m:abc>\n")
        << QString::fromUtf8("<p>&lt;m:abc&gt;</p>");
    QTest::newRow("CommonMark: Autolinks 17")
        << QString::fromUtf8("<foo.bar.baz>\n")
        << QString::fromUtf8("<p>&lt;foo.bar.baz&gt;</p>");
    QTest::newRow("CommonMark: Raw HTML 1")
        << QString::fromUtf8("<a><bab><c2c>\n")
        << QString::fromUtf8("<p><a><bab><c2c></p>");
    QTest::newRow("CommonMark: Raw HTML 2")
        << QString::fromUtf8("<a/><b2/>\n")
        << QString::fromUtf8("<p><a/><b2/></p>");
    QTest::newRow("CommonMark: Raw HTML 3")
        << QString::fromUtf8("<a  /><b2\ndata=\"foo\" >\n")
        << QString::fromUtf8("<p><a  /><b2\ndata=\"foo\" ></p>");
    QTest::newRow("CommonMark: Raw HTML 4")
        << QString::fromUtf8("<a foo=\"bar\" bam = 'baz <em>\"</em>'\n_boolean zoop:33=zoop:33 />\n")
        << QString::fromUtf8("<p><a foo=\"bar\" bam = 'baz <em>\"</em>'\n_boolean zoop:33=zoop:33 /></p>");
    QTest::newRow("CommonMark: Raw HTML 5")
        << QString::fromUtf8("Foo <responsive-image src=\"foo.jpg\" />\n")
        << QString::fromUtf8("<p>Foo <responsive-image src=\"foo.jpg\" /></p>");
    QTest::newRow("CommonMark: Raw HTML 6")
        << QString::fromUtf8("<33> <__>\n")
        << QString::fromUtf8("<p>&lt;33&gt; &lt;__&gt;</p>");
    QTest::newRow("CommonMark: Raw HTML 7")
        << QString::fromUtf8("<a h*#ref=\"hi\">\n")
        << QString::fromUtf8("<p>&lt;a h*#ref=&quot;hi&quot;&gt;</p>");
    QTest::newRow("CommonMark: Raw HTML 8")
        << QString::fromUtf8("<a href=\"hi'> <a href=hi'>\n")
        << QString::fromUtf8("<p>&lt;a href=&quot;hi'&gt; &lt;a href=hi'&gt;</p>");
    QTest::newRow("CommonMark: Raw HTML 9")
        << QString::fromUtf8("< a><\nfoo><bar/ >\n<foo bar=baz\nbim!bop />\n")
        << QString::fromUtf8("<p>&lt; a&gt;&lt;\nfoo&gt;&lt;bar/ &gt;\n&lt;foo bar=baz\nbim!bop /&gt;</p>");
    QTest::newRow("CommonMark: Raw HTML 10")
        << QString::fromUtf8("<a href='bar'title=title>\n")
        << QString::fromUtf8("<p>&lt;a href='bar'title=title&gt;</p>");
    QTest::newRow("CommonMark: Raw HTML 11")
        << QString::fromUtf8("</a></foo >\n")
        << QString::fromUtf8("<p></a></foo ></p>");
    QTest::newRow("CommonMark: Raw HTML 12")
        << QString::fromUtf8("</a href=\"foo\">\n")
        << QString::fromUtf8("<p>&lt;/a href=&quot;foo&quot;&gt;</p>");
    QTest::newRow("CommonMark: Raw HTML 13")
        << QString::fromUtf8("foo <!-- this is a --\ncomment - with hyphens -->\n")
        << QString::fromUtf8("<p>foo <!-- this is a --\ncomment - with hyphens --></p>");
    QTest::newRow("CommonMark: Raw HTML 14")
        << QString::fromUtf8("foo <!--> foo -->\n\nfoo <!---> foo -->\n")
        << QString::fromUtf8("<p>foo <!--> foo --&gt;</p>\n<p>foo <!---> foo --&gt;</p>");
    QTest::newRow("CommonMark: Raw HTML 15")
        << QString::fromUtf8("foo <\?php echo $a; \?>\n")
        << QString::fromUtf8("<p>foo <\?php echo $a; \?></p>");
    QTest::newRow("CommonMark: Raw HTML 16")
        << QString::fromUtf8("foo <!ELEMENT br EMPTY>\n")
        << QString::fromUtf8("<p>foo <!ELEMENT br EMPTY></p>");
    QTest::newRow("CommonMark: Raw HTML 17")
        << QString::fromUtf8("foo <![CDATA[>&<]]>\n")
        << QString::fromUtf8("<p>foo <![CDATA[>&<]]></p>");
    QTest::newRow("CommonMark: Raw HTML 18")
        << QString::fromUtf8("foo <a href=\"&ouml;\">\n")
        << QString::fromUtf8("<p>foo <a href=\"&ouml;\"></p>");
    QTest::newRow("CommonMark: Raw HTML 19")
        << QString::fromUtf8("foo <a href=\"\\*\">\n")
        << QString::fromUtf8("<p>foo <a href=\"\\*\"></p>");
    QTest::newRow("CommonMark: Raw HTML 20")
        << QString::fromUtf8("<a href=\"\\\"\">\n")
        << QString::fromUtf8("<p>&lt;a href=&quot;&quot;&quot;&gt;</p>");
    QTest::newRow("CommonMark: Raw HTML 21")
        << QString::fromUtf8("<a\n> quoted text\n")
        << QString::fromUtf8("<p>&lt;a</p>\n<blockquote>\n<p>quoted text</p>\n</blockquote>");
    QTest::newRow("CommonMark: Hard line breaks 1")
        << QString::fromUtf8("foo  \nbaz\n")
        << QString::fromUtf8("<p>foo<br>\nbaz</p>");
    QTest::newRow("CommonMark: Hard line breaks 2")
        << QString::fromUtf8("foo\\\nbaz\n")
        << QString::fromUtf8("<p>foo<br>\nbaz</p>");
    QTest::newRow("CommonMark: Hard line breaks 3")
        << QString::fromUtf8("foo       \nbaz\n")
        << QString::fromUtf8("<p>foo<br>\nbaz</p>");
    QTest::newRow("CommonMark: Hard line breaks 4")
        << QString::fromUtf8("foo  \n     bar\n")
        << QString::fromUtf8("<p>foo<br>\nbar</p>");
    QTest::newRow("CommonMark: Hard line breaks 5")
        << QString::fromUtf8("foo\\\n     bar\n")
        << QString::fromUtf8("<p>foo<br>\nbar</p>");
    QTest::newRow("CommonMark: Hard line breaks 6")
        << QString::fromUtf8("*foo  \nbar*\n")
        << QString::fromUtf8("<p><em>foo<br>\nbar</em></p>");
    QTest::newRow("CommonMark: Hard line breaks 7")
        << QString::fromUtf8("*foo\\\nbar*\n")
        << QString::fromUtf8("<p><em>foo<br>\nbar</em></p>");
    QTest::newRow("CommonMark: Hard line breaks 8")
        << QString::fromUtf8("`code  \nspan`\n")
        << QString::fromUtf8("<p><code>code   span</code></p>");
    QTest::newRow("CommonMark: Hard line breaks 9")
        << QString::fromUtf8("`code\\\nspan`\n")
        << QString::fromUtf8("<p><code>code\\ span</code></p>");
    QTest::newRow("CommonMark: Hard line breaks 10")
        << QString::fromUtf8("<a href=\"foo  \nbar\">\n")
        << QString::fromUtf8("<p><a href=\"foo  \nbar\"></p>");
    QTest::newRow("CommonMark: Hard line breaks 11")
        << QString::fromUtf8("<a href=\"foo\\\nbar\">\n")
        << QString::fromUtf8("<p><a href=\"foo\\\nbar\"></p>");
    QTest::newRow("CommonMark: Hard line breaks 12")
        << QString::fromUtf8("foo\\\n")
        << QString::fromUtf8("<p>foo\\</p>");
    QTest::newRow("CommonMark: Hard line breaks 13")
        << QString::fromUtf8("foo  \n")
        << QString::fromUtf8("<p>foo</p>");
    QTest::newRow("CommonMark: Hard line breaks 14")
        << QString::fromUtf8("### foo\\\n")
        << QString::fromUtf8("<h3>foo\\</h3>");
    QTest::newRow("CommonMark: foo\\ 1")
        << QString::fromUtf8("### foo  \n")
        << QString::fromUtf8("<h3>foo</h3>");
    QTest::newRow("CommonMark: Soft line breaks 1")
        << QString::fromUtf8("foo\nbaz\n")
        << QString::fromUtf8("<p>foo\nbaz</p>");
    QTest::newRow("CommonMark: Soft line breaks 2")
        << QString::fromUtf8("foo \n baz\n")
        << QString::fromUtf8("<p>foo\nbaz</p>");
    QTest::newRow("CommonMark: Textual content 1")
        << QString::fromUtf8("hello $.;'there\n")
        << QString::fromUtf8("<p>hello $.;'there</p>");
    QTest::newRow("CommonMark: Textual content 2")
        << QString::fromUtf8("Foo χρῆν\n")
        << QString::fromUtf8("<p>Foo χρῆν</p>");
    QTest::newRow("CommonMark: Textual content 3")
        << QString::fromUtf8("Multiple     spaces\n")
        << QString::fromUtf8("<p>Multiple     spaces</p>");
    QTest::newRow("GFM: Tables (extension) 1")
        << QString::fromUtf8("| foo | bar |\n| --- | --- |\n| baz | bim |\n")
        << QString::fromUtf8("<table>\n<thead>\n<tr>\n<th>foo</th>\n<th>bar</th>\n</tr>\n</thead>\n<tbody>\n<tr>\n<td>baz</td>\n<td>bim</td>\n</tr>\n</tbody>\n</table>");
    QTest::newRow("GFM: Tables (extension) 2")
        << QString::fromUtf8("| abc | defghi |\n:-: | -----------:\nbar | baz\n")
        << QString::fromUtf8("<table>\n<thead>\n<tr>\n<th align=\"center\">abc</th>\n<th align=\"right\">defghi</th>\n</tr>\n</thead>\n<tbody>\n<tr>\n<td align=\"center\">bar</td>\n<td align=\"right\">baz</td>\n</tr>\n</tbody>\n</table>");
    QTest::newRow("GFM: Tables (extension) 3")
        << QString::fromUtf8("| f\\|oo  |\n| ------ |\n| b `\\|` az |\n| b **\\|** im |\n")
        << QString::fromUtf8("<table>\n<thead>\n<tr>\n<th>f|oo</th>\n</tr>\n</thead>\n<tbody>\n<tr>\n<td>b <code>|</code> az</td>\n</tr>\n<tr>\n<td>b <strong>|</strong> im</td>\n</tr>\n</tbody>\n</table>");
    QTest::newRow("GFM: Tables (extension) 4")
        << QString::fromUtf8("| abc | def |\n| --- | --- |\n| bar | baz |\n> bar\n")
        << QString::fromUtf8("<table>\n<thead>\n<tr>\n<th>abc</th>\n<th>def</th>\n</tr>\n</thead>\n<tbody>\n<tr>\n<td>bar</td>\n<td>baz</td>\n</tr>\n</tbody>\n</table>\n<blockquote>\n<p>bar</p>\n</blockquote>");
    QTest::newRow("GFM: Tables (extension) 5")
        << QString::fromUtf8("| abc | def |\n| --- | --- |\n| bar | baz |\nbar\n\nbar\n")
        << QString::fromUtf8("<table>\n<thead>\n<tr>\n<th>abc</th>\n<th>def</th>\n</tr>\n</thead>\n<tbody>\n<tr>\n<td>bar</td>\n<td>baz</td>\n</tr>\n<tr>\n<td>bar</td>\n<td></td>\n</tr>\n</tbody>\n</table>\n<p>bar</p>");
    QTest::newRow("GFM: Tables (extension) 6")
        << QString::fromUtf8("| abc | def |\n| --- |\n| bar |\n")
        << QString::fromUtf8("<p>| abc | def |\n| --- |\n| bar |</p>");
    QTest::newRow("GFM: Tables (extension) 7")
        << QString::fromUtf8("| abc | def |\n| --- | --- |\n| bar |\n| bar | baz | boo |\n")
        << QString::fromUtf8("<table>\n<thead>\n<tr>\n<th>abc</th>\n<th>def</th>\n</tr>\n</thead>\n<tbody>\n<tr>\n<td>bar</td>\n<td></td>\n</tr>\n<tr>\n<td>bar</td>\n<td>baz</td>\n</tr>\n</tbody>\n</table>");
    QTest::newRow("GFM: Tables (extension) 8")
        << QString::fromUtf8("| abc | def |\n| --- | --- |\n")
        << QString::fromUtf8("<table>\n<thead>\n<tr>\n<th>abc</th>\n<th>def</th>\n</tr>\n</thead>\n</table>");
    QTest::newRow("GFM: Task list items (extension) 1")
        << QString::fromUtf8("- [ ] foo\n- [x] bar\n")
        << QString::fromUtf8("<ul>\n<li><input disabled=\"\" type=\"checkbox\"> foo</li>\n<li><input checked=\"\" disabled=\"\" type=\"checkbox\"> bar</li>\n</ul>");
    QTest::newRow("GFM: Task list items (extension) 2")
        << QString::fromUtf8("- [x] foo\n  - [ ] bar\n  - [x] baz\n- [ ] bim\n")
        << QString::fromUtf8("<ul>\n<li><input checked=\"\" disabled=\"\" type=\"checkbox\"> foo\n<ul>\n<li><input disabled=\"\" type=\"checkbox\"> bar</li>\n<li><input checked=\"\" disabled=\"\" type=\"checkbox\"> baz</li>\n</ul>\n</li>\n<li><input disabled=\"\" type=\"checkbox\"> bim</li>\n</ul>");
    QTest::newRow("GFM: Strikethrough (extension) 1")
        << QString::fromUtf8("~~Hi~~ Hello, world!\n")
        << QString::fromUtf8("<p><del>Hi</del> Hello, world!</p>");
    QTest::newRow("GFM: Strikethrough (extension) 2")
        << QString::fromUtf8("This ~~has a\n\nnew paragraph~~.\n")
        << QString::fromUtf8("<p>This ~~has a</p>\n<p>new paragraph~~.</p>");
    QTest::newRow("GFM: Autolinks (extension) 1")
        << QString::fromUtf8("www.commonmark.org\n")
        << QString::fromUtf8("<p><a href=\"http://www.commonmark.org\">www.commonmark.org</a></p>");
    QTest::newRow("GFM: Autolinks (extension) 2")
        << QString::fromUtf8("Visit www.commonmark.org/help for more information.\n")
        << QString::fromUtf8("<p>Visit <a href=\"http://www.commonmark.org/help\">www.commonmark.org/help</a> for more information.</p>");
    QTest::newRow("GFM: Autolinks (extension) 3")
        << QString::fromUtf8("Visit www.commonmark.org.\n\nVisit www.commonmark.org/a.b.\n")
        << QString::fromUtf8("<p>Visit <a href=\"http://www.commonmark.org\">www.commonmark.org</a>.</p>\n<p>Visit <a href=\"http://www.commonmark.org/a.b\">www.commonmark.org/a.b</a>.</p>");
    QTest::newRow("GFM: Autolinks (extension) 4")
        << QString::fromUtf8("www.google.com/search\?q=Markup+(business)\n\nwww.google.com/search\?q=Markup+(business)))\n\n(www.google.com/search\?q=Markup+(business))\n\n(www.google.com/search\?q=Markup+(business)\n")
        << QString::fromUtf8("<p><a href=\"http://www.google.com/search\?q=Markup+(business)\">www.google.com/search\?q=Markup+(business)</a></p>\n<p><a href=\"http://www.google.com/search\?q=Markup+(business)\">www.google.com/search\?q=Markup+(business)</a>))</p>\n<p>(<a href=\"http://www.google.com/search\?q=Markup+(business)\">www.google.com/search\?q=Markup+(business)</a>)</p>\n<p>(<a href=\"http://www.google.com/search\?q=Markup+(business)\">www.google.com/search\?q=Markup+(business)</a></p>");
    QTest::newRow("GFM: Autolinks (extension) 5")
        << QString::fromUtf8("www.google.com/search\?q=(business))+ok\n")
        << QString::fromUtf8("<p><a href=\"http://www.google.com/search\?q=(business))+ok\">www.google.com/search\?q=(business))+ok</a></p>");
    QTest::newRow("GFM: Autolinks (extension) 6")
        << QString::fromUtf8("www.google.com/search\?q=commonmark&hl=en\n\nwww.google.com/search\?q=commonmark&hl;\n")
        << QString::fromUtf8("<p><a href=\"http://www.google.com/search\?q=commonmark&amp;hl=en\">www.google.com/search\?q=commonmark&amp;hl=en</a></p>\n<p><a href=\"http://www.google.com/search\?q=commonmark\">www.google.com/search\?q=commonmark</a>&amp;hl;</p>");
    QTest::newRow("GFM: Autolinks (extension) 7")
        << QString::fromUtf8("www.commonmark.org/he<lp\n")
        << QString::fromUtf8("<p><a href=\"http://www.commonmark.org/he\">www.commonmark.org/he</a>&lt;lp</p>");
    QTest::newRow("GFM: Autolinks (extension) 8")
        << QString::fromUtf8("http://commonmark.org\n\n(Visit https://encrypted.google.com/search\?q=Markup+(business))\n\nAnonymous FTP is available at ftp://foo.bar.baz.\n")
        << QString::fromUtf8("<p><a href=\"http://commonmark.org\">http://commonmark.org</a></p>\n<p>(Visit <a href=\"https://encrypted.google.com/search\?q=Markup+(business)\">https://encrypted.google.com/search\?q=Markup+(business)</a>)</p>\n<p>Anonymous FTP is available at <a href=\"ftp://foo.bar.baz\">ftp://foo.bar.baz</a>.</p>");
    QTest::newRow("GFM: Autolinks (extension) 9")
        << QString::fromUtf8("foo@bar.baz\n")
        << QString::fromUtf8("<p><a href=\"mailto:foo@bar.baz\">foo@bar.baz</a></p>");
    QTest::newRow("GFM: Autolinks (extension) 10")
        << QString::fromUtf8("hello@mail+xyz.example isn't valid, but hello+xyz@mail.example is.\n")
        << QString::fromUtf8("<p>hello@mail+xyz.example isn't valid, but <a href=\"mailto:hello+xyz@mail.example\">hello+xyz@mail.example</a> is.</p>");
    QTest::newRow("GFM: Autolinks (extension) 11")
        << QString::fromUtf8("a.b-c_d@a.b\n\na.b-c_d@a.b.\n\na.b-c_d@a.b-\n\na.b-c_d@a.b_\n")
        << QString::fromUtf8("<p><a href=\"mailto:a.b-c_d@a.b\">a.b-c_d@a.b</a></p>\n<p><a href=\"mailto:a.b-c_d@a.b\">a.b-c_d@a.b</a>.</p>\n<p>a.b-c_d@a.b-</p>\n<p>a.b-c_d@a.b_</p>");
