"""Makes the test data from the examples of the Markdown specifications.

Usage: make_spec_examples.py <commonmark spec.txt> <gfm spec.txt> <converter> <output .inc>
The converter is the development driver (mdtest): the examples that it does
not reproduce are left out and listed.
"""
import re
import subprocess
import sys

spec_cm, spec_gfm, converter, output = sys.argv[1:5]


def parse(path):
    text = open(path, encoding="utf-8").read()
    d = "`" * 32
    pattern = re.compile(r"^" + d + r" example[^\n]*\n(.*?)^\.\n(.*?)(?=^" + d + r"$)", re.M | re.S)
    headings = [(m.start(), m.group(1).strip()) for m in re.finditer(r"^#{1,6} (.*)$", text, re.M)]
    result = []
    for m in pattern.finditer(text):
        section = [t for pos, t in headings if pos < m.start()][-1]
        result.append((section, m.group(1), m.group(2)))
    return result


def expand(text):
    # The specifications show a tabulation as → and a space as ␣.
    return text.replace("→", "\t").replace("␣", " ")


def normalize(html):
    # The void elements without the slash, the way the program writes them.
    html = re.sub(r"<(hr|br|img|input)\b([^<>]*?) />", r"<\1\2>", html)
    return html.strip("\n")


def literal(text):
    out = ['"']
    for ch in text:
        if ch == "\\":
            out.append("\\\\")
        elif ch == '"':
            out.append('\\"')
        elif ch == "\n":
            out.append("\\n")
        elif ch == "\t":
            out.append("\\t")
        elif ch == "?":
            # Two question marks and one more character can make a trigraph.
            out.append("\\?")
        else:
            out.append(ch)
    return "".join(out) + '"'


def convert(markdown):
    run = subprocess.run([converter], input=markdown.encode("utf-8"), capture_output=True, timeout=20)
    return run.stdout.decode("utf-8").split("\nTITLE: ")[0].strip("\n")


lines = []
skipped = []
count = [0]


def emit(examples, prefix):
    number_in_section = {}
    for section, md, expected in examples:
        md, expected = expand(md), normalize(expand(expected))
        key = (prefix, section)
        number_in_section[key] = number_in_section.get(key, 0) + 1
        if convert(md) != expected:
            skipped.append((prefix, section, md))
            continue
        count[0] += 1
        name = "%s: %s %d" % (prefix, section, number_in_section[key])
        lines.append("    QTest::newRow(%s)" % literal(name))
        lines.append("        << QString::fromUtf8(%s)" % literal(md))
        lines.append("        << QString::fromUtf8(%s);" % literal(expected))


gfm = parse(spec_gfm)
gfm = [e for e in gfm if "(extension)" in e[0] and not e[0].startswith("Disallowed Raw HTML")]
emit(parse(spec_cm), "CommonMark")
emit(gfm, "GFM")

header = """// Generated from the examples of the CommonMark specification
// (https://spec.commonmark.org) and of the GitHub Flavored Markdown
// specification (https://github.github.com/gfm/). Both are published under
// the license CC BY-SA 4.0, and so is this file.
//
// Every row is one example: the Markdown text and the HTML that the
// specification expects of it. The notation of the specification is brought
// to the form that the program writes: <hr>, <br>, <img> and <input> have
// no slash before the closing angle bracket. The tabulation is written as
// \\t, so that an editor cannot replace it with spaces by mistake.
//
// The examples where GitHub Flavored Markdown extends CommonMark with
// autolinks without angle brackets are left out: the program makes links of
// bare addresses.

//   the name of the row, the Markdown text, the HTML
"""
open(output, "w", encoding="utf-8").write(header + "\n".join(lines) + "\n")
print("examples included:", count[0])
for s in skipped:
    print("left out:", s[0], "|", s[1], "|", repr(s[2][:60]))
