import re
import os
import sys


debug_enabled = False
def dbg(*args, **kwargs):
    if debug_enabled:
        print("//", *args, **kwargs, file=sys.stderr)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# classes to specify amalgamation

class cmtfile:
    """commented file"""
    def __init__(self, filename):
        self.filename = filename
    def __str__(self):
        return self.filename


class cmttext:
    """commented text"""
    def __init__(self, text):
        self.text = text
    def __str__(self):
        return self.text


class ignfile:
    """ignore file"""
    def __init__(self, filename):
        self.filename = filename
    def __str__(self):
        return self.filename


class hdrfile:
    """header file, with custom include guard"""
    def __init__(self, filename, incpattern, include_guard=None):
        self.filename = filename
        self.incpattern = incpattern
        self.include_guard = include_guard
    def __str__(self):
        return self.filename


class injfile:
    """header file, to be injected at the first include point"""
    def __init__(self, filename, incpattern):
        self.filename = filename
        self.incpattern = incpattern
    def __str__(self):
        return self.filename


class injcode:
    """direct code to inject"""
    def __init__(self, code):
        self.code = code
    def __str__(self):
        return self.code


class onlyif:
    """include an object only if a condition is met"""
    def __init__(self, condition, obj):
        self.condition = condition
        self.obj = obj
    def __str__(self):
        return str(self.obj)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

def catfiles(filenames, rootdir,
             include_regexes,
             definition_macro,
             repo,
             result_incguard):
    file_re = re.compile('[-./]')
    sepb = "//" + ("**" * 40)
    sepf = "//" + ("--" * 40)
    to_inject = {}
    custom_include_guards = {}
    def banner(s):
        return f"\n\n\n{sepb}\n{sepf}\n// {s}\n{sepf}\n{sepb}\n\n"
    def footer(s):
        return f"\n\n// (end {s})\n"
    def incguard(filename):
        return custom_include_guards.get(filename,
                                         f"{file_re.sub('_', filename).upper()}_")
    def is_guarded(icurr, lines, guard):
        rx = re.compile(r'^\s*#\s*ifndef\s+{}.*'.format(guard))
        iprev = icurr - 1
        while iprev >= 0:
            prev = lines[iprev]
            iprev -= 1
            if prev.startswith("//") or prev.startswith("/*"):
                continue
            if rx.match(prev):
                return True
            else:
                break
        return False
    def replace_include(rx, match, line_idx, lines, guard):
        line = lines[line_idx].rstrip()
        incl = match.group(1)
        if to_inject.get(incl) is None:
            repl = f"//{line}   // amalgamate: remove include\n#error \"amalgamate: {incl} must have been amalgamated before this point\""
            if guard is None:
                guard = incguard(incl)
            if is_guarded(line_idx, lines, guard):
                return repl + "\n"
            else:
                return f"""#ifndef {guard}
{repl}
#endif /* {guard} */
"""
        else:
            entry = to_inject[incl]
            del to_inject[incl]
            return append_file(entry.filename)
    def append_file(filename, guard=None):
        s = ""
        with open(filename, encoding="utf8") as f:
            lines = list(f.readlines())
            for i, line in enumerate(lines):
                for rx in include_regexes:
                    match = rx.match(line)
                    if match:
                        line = replace_include(rx, match, i, lines, guard)
                s += line
        return s
    def append_cpp(filename):
        if definition_macro is None:
            return append_file(filename)
        else:
            return f"""#ifdef {definition_macro}
{append_file(filename)}
#endif /* {definition_macro} */
"""
    def is_src(filename):
        return filename.endswith(".cpp") or filename.endswith(".c")
    def cmtline(line, more=""):
        if len(line.strip()) > 0:
            return f"// {line}{more}"
        else:
            return "//\n"
    out = ""
    for entry in filenames:
        if isinstance(entry, onlyif):
            if entry.condition:
                dbg(entry, "yes")
                entry = entry.obj
            else:
                dbg(entry, "no")
                continue
        if isinstance(entry, ignfile):
            pass
        elif isinstance(entry, cmttext):
            for line in entry.text.split("\n"):
                out += cmtline(line, "\n")
        elif isinstance(entry, cmtfile):
            filename = f"{rootdir}/{entry.filename}"
            out += banner(entry.filename)
            with open(filename, encoding="utf8") as file:
                for line in file.readlines():
                    out += cmtline(line)
        elif isinstance(entry, injcode):
            out += f"\n{entry.code}\n"
        elif isinstance(entry, injfile):
            entry.filename = f"{rootdir}/{entry.filename}"
            to_inject[entry.incpattern] = entry
        else:
            filename = f"{rootdir}/{entry}"
            out += banner(entry)
            if isinstance(entry, hdrfile):
                if entry.include_guard is not None:
                    custom_include_guards[entry.incpattern] = entry.include_guard
                out += append_file(filename, entry.include_guard)
            else:
                assert isinstance(entry, str)
                if is_src(filename):
                    out += append_cpp(filename)
                else:
                    out += append_file(filename)
            out += footer(entry)
    if result_incguard is not None:
        out = f"""#pragma once
#ifndef {result_incguard}
#define {result_incguard}

{out}
#endif /* {result_incguard} */
"""
    return out


# -----------------------------------------------------------------------------

def include_only_first(file_contents: str):
    """read the file contents, and remove repeated includes of the same file"""
    rx = [
        re.compile(r'^\s*#\s*include\s+"(.*?)".*'),
        re.compile(r'^\s*#\s*include\s+<(.*?)>.*'),
    ]
    already_included = {}
    out = ""
    for line in file_contents.split("\n"):
        for expr in rx:
            match = expr.match(line)
            if match:
                incl = match.group(1)
                if already_included.get(incl) is None:
                    already_included[incl] = line
                    if incl.endswith(".h"):
                        cpp_version = f"c{incl[:-2]}"
                        already_included[cpp_version] = line
                    elif incl.startswith("c") and not (incl.endswith(".h") or incl.endswith(".hpp")):
                        c_version = f"{incl[1:]}.h"
                        already_included[c_version] = line
                else:
                    line = f"//{line}  // amalgamate: included above"
                break
        out += line
        out += "\n"
    return out


# -----------------------------------------------------------------------------

def mkparser(**bool_args):
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("output", default=None, nargs='?', help="output file. defaults to stdout")
    for k, (default, help) in bool_args.items():
        yes = '--' + k
        no = '--no-' + k
        if default:
            txt_default = f"the default is {yes}"
        else:
            txt_default = f"the default is {no}"
        if sys.version_info >= (3,9):
            # https://stackoverflow.com/questions/9234258/in-python-argparse-is-it-possible-to-have-paired-no-something-something-arg
            parser.add_argument(yes, default=default, action=argparse.BooleanOptionalAction,
                                help=f"{help}. {txt_default}")
        else:
            # https://stackoverflow.com/questions/15008758/parsing-boolean-values-with-argparse
            feature = parser.add_mutually_exclusive_group(required=False)
            feature.add_argument(yes, dest=k, action='store_true', help=f"{help}. {txt_default}.")
            feature.add_argument(no, dest=k, action='store_false', help=f"{help}. {txt_default}.")
            parser.set_defaults(**{k: default})
    return parser


# -----------------------------------------------------------------------------

def file_put_contents(filename: str, contents: str):
    if filename is None:
        print(contents)
    else:
        dirname = os.path.dirname(filename)
        if dirname:
            os.makedirs(dirname, exist_ok=True)
        with open(filename, "w", encoding="utf8") as output:
            output.write(contents)
