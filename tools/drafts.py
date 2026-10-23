"""Finds draft bodies that are disabled in the source, so ready.py and
open_work.py do not offer them as unwritten work.

A disabled draft is inside an `#if 0` block and says so in one of three ways:
a `// Retail 0x...` marker (not `// @retail`, which the build would compile),
a `// Disabled implementation for 0x...` comment, or a function definition
named `function_<va>`. This module walks src/ and returns those addresses.

    python tools/drafts.py [--root src]

The scan reads preprocessor conditionals only: a draft is disabled when its
marker line sits inside an `#if 0` branch, including a nested one. `#elif`
after an `#if 0` is a new branch and is enabled unless it is itself `#elif 0`.
"""
import argparse
import os
import re
import sys

from xbe import ROOT

MARKER = re.compile(r'^\s*//\s*(?:@retail|retail)\s+(0x[0-9a-fA-F]+)\b', re.IGNORECASE)
DISABLED_COMMENT = re.compile(r'//\s*disabled implementation for\s+(0x[0-9a-fA-F]+)', re.IGNORECASE)
# A definition, not a call or a prototype: it has a return type before the
# name, and the signature's closing parenthesis is followed by `{` (possibly
# on the next line of the same disabled block).
DEFINITION = re.compile(
    r'^[ \t]*(?!return\b)(?:[A-Za-z_]\w*[\s*&]+)+function_([0-9a-fA-F]+)\s*\([^;{}]*\)\s*(?:\r?\n[ \t]*)?\{',
    re.MULTILINE)
_CONDITIONAL = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b(.*)$')
_COMMENT = re.compile(r'/\*.*?\*/|//.*$')


def _expression(text):
    """A conditional's expression without its trailing comment."""
    return _COMMENT.sub('', text).strip()


def disabled_lines(path):
    """(line number, line) for each line inside a disabled conditional."""
    stack = []  # one bool per open conditional: is its current branch disabled?
    with open(path, encoding='utf-8', errors='replace') as fh:
        for number, line in enumerate(fh, 1):
            m = _CONDITIONAL.match(line)
            if m:
                word, rest = m.group(1), _expression(m.group(2))
                if word in ('if', 'ifdef', 'ifndef'):
                    parent = any(stack)
                    zero = word == 'if' and rest == '0'
                    stack.append(parent or zero)
                elif word == 'elif':
                    if stack:
                        parent = any(stack[:-1])
                        stack[-1] = parent or rest == '0'
                elif word == 'else':
                    if stack:
                        parent = any(stack[:-1])
                        stack[-1] = parent or not stack[-1]
                elif stack:
                    stack.pop()
                continue
            if stack and stack[-1]:
                yield number, line


def disabled_addresses(root=os.path.join(ROOT, 'src')):
    """{retail va: source path} for drafts disabled inside an `#if 0` block."""
    found = {}
    for directory, _, names in os.walk(root):
        for name in sorted(names):
            if not name.endswith('.cpp'):
                continue
            path = os.path.join(directory, name)
            lines = list(disabled_lines(path))
            for _, line in lines:
                m = MARKER.match(line) or DISABLED_COMMENT.search(line)
                if m:
                    found[int(m.group(1), 16)] = path
            text = ''.join(line for _, line in lines)
            for m in DEFINITION.finditer(text):
                found[int(m.group(1), 16)] = path
    return found


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('--root', default=os.path.join(ROOT, 'src'),
                    help='source tree to scan (default: src)')
    args = ap.parse_args(argv)
    found = disabled_addresses(args.root)
    for va in sorted(found):
        print(f'0x{va:x} {found[va]}')
    print(f'{len(found)} disabled drafts', file=sys.stderr)
    return 0


if __name__ == '__main__':
    sys.exit(main())
