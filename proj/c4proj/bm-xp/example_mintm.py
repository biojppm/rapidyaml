import os
import sys
import argparse
import copy
import requests
import flask
import json
import re
import yaml
import shutil
import mmh3
from itertools import islice

from munch import Munch, munchify
from flask import render_template, redirect, url_for, send_from_directory
from markupsafe import escape


def log(*args, **kwargs):
    print(*args, **kwargs, flush=True)


def myhash_combine(curr, value):
    return curr ^ (value + 0x9e3779b9 + (curr<<6) + (curr>>2))


def nth(iterable, n, default=None):
    "Returns the nth item or a default value"
    return next(islice(iterable, n, None), default)


def optionals(obj, *attrs):
    ret = []
    for attr in attrs:
        if not hasattr(obj, attr):
            log("attr not present:", attr)
            continue
        ret.append(getattr(obj, attr))
    return ret


def myhash(*args):
    h = 137597
    for a in args:
        if isinstance(a, str):
            if a == "":
                continue
            b = bytes(a, "utf8")
        else:
            b = bytes(a)
        hb = mmh3.hash(b, signed=False)
        h = myhash_combine(h, hb)
    s = hex(h)
    return s[2:min(10, len(s))]


def copy_file_to_dir(file, dir):
    dir = os.path.abspath(dir)
    src = os.path.abspath(file)
    dst = f"{dir}/{os.path.basename(src)}"
    if not os.path.exists(dir):
        os.makedirs(dir)
    if os.path.exists(dst):
        os.remove(dst)
    log("copy:", src, "-->", dst)
    shutil.copy(src, dst)
    return dst


def chk(f):
    log(f"looking for file:", f)
    assert os.path.exists(f), f
    return f


def load_yml_file(filename):
    if not os.path.exists(filename):
        raise Exception(f"not found: {filename}")
    with open(filename) as f:
        return load_yml(f.read())


def dump_yml(data, filename):
    with open(filename, "w") as f:
        yaml.safe_dump(data, f)


def load_yml(yml):
    return munchify(yaml.safe_load(yml))


def dump_json(data, filename):
    with open(filename, "w") as f:
        f.write(json.dumps(data, indent=2, sort_keys=True))


def load_json(filename):
    with open(filename, "r") as f:
        try:
            return munchify(json.load(f))
        except Exception as exc:
            raise Exception(f"could not load file: {filename}: {exc}")


def main():
    def _common_args(parser):
        parser.add_argument("-m", "--manifest", type=str, default="", help="enable debug mode")
        parser.add_argument("--debug", action="store_true", help="enable debug mode")
    #
    parser = argparse.ArgumentParser(description="Browse benchmark results", prog="bm")
    _common_args(parser)
    subparsers = parser.add_subparsers()
    #
    sp = subparsers.add_parser("create", help="create benchmark collection")
    _common_args(sp)
    sp.add_argument("--debug", action="store_true", help="enable debug mode")
    sp.add_argument("filename", type=str, help="the YAML file with the benchmark specs")
    sp.add_argument("target", type=str, help="the directory to store the results")
    #
    sp = subparsers.add_parser("meta", help="get the required meta-information: cpu info, commit data")
    _common_args(sp)
    sp.add_argument("--debug", action="store_true", help="enable debug mode")
    sp.add_argument("results", type=str, help="the directory with the results")
    sp.add_argument("cmakecache", type=str, help="the path to the CMakeCache.txt file used to build the benchmark binaries")
    sp.add_argument("build_type", type=str, help="the build type, eg Release Debug MinSizeRel RelWithDebInfo")
    #
    sp = subparsers.add_parser("add", help="add benchmark results")
    _common_args(sp)
    sp.add_argument("--debug", action="store_true", help="enable debug mode")
    sp.add_argument("results", type=str, help="the directory with the results")
    sp.add_argument("target", type=str, help="the directory to store the results")
    #
    sp = subparsers.add_parser("serve", help="serve benchmark results")
    _common_args(sp)
    sp.add_argument("--debug", action="store_true", help="enable debug mode")
    sp.add_argument("bmdir", type=os.path.abspath, default=os.getcwd(), help="the directory with the results. default=.")
    sp.add_argument("-H", "--host", type=str, default="localhost", help="host. default=%(default)s")
    sp.add_argument("-p", "--port", type=int, default=8000, help="port. default=%(default)s")
    #
    sp = subparsers.add_parser("export", help="export static html")
    sp.set_defaults(func=freeze)
    sp.add_argument("bmdir", type=os.path.abspath, default=os.getcwd(), help="the directory with the results. default=.")
    _common_args(sp)
    #
    sp = subparsers.add_parser("deps", help="install server dependencies")
    sp.set_defaults(func=lambda _: download_deps())
    sp.add_argument("--debug", action="store_true", help="enable debug mode")
    _common_args(sp)
    #
    args = parser.parse_args(sys.argv[1:] if len(sys.argv) > 1 else ["serve"])
    if args.debug:
        log(args)
    args.func(args)


def get_manifest(args):
    bmdir = os.path.abspath(args.bmdir)
    if not args.manifest:
        manifest_yml = os.path.join(bmdir, "manifest.yml")
    else:
        if not os.path.isabs(args.manifest):
            manifest_yml = os.path.join(os.getcwd(), args.manifest)
    manifest_json = os.path.join(os.path.dirname(manifest.yml), "manifest.json")
    manifest = load_yml_file(manifest_yml)
    dump_json(manifest, manifest_json)
    return manifest


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

app = flask.Flask(__name__,
                  template_folder='template')


def _setup_app(args):
    def _s(prop, val):
        assert not hasattr(app, prop), prop
        setattr(app, prop, val)
    _s('args', args)
    _s('manifest', get_manifest(args))
    if args.debug:
        app.config["DEBUG"] = True


def freeze(args):
    "https://pythonhosted.org/Frozen-Flask/"
    from flask_frozen import Freezer
    _setup_app(args)
    freezer = Freezer(app)
    freezer.freeze(debug=args.debug)


def serve(args):
    _setup_app(args)
    app.run(host=args.host, port=args.port, debug=args.debug)


@app.route("/")
def home():
    log("requested home")
    return render_template("index.html")


@app.route("/<path>")
def other_(path):
    path = escape(path)
    d = app.args.bmdir
    log("requested other path:", path, "---", os.path.join(d, path))
    return send_from_directory(d, path)


@app.route("/static/<path>")
def static_(path):
    path = escape(path)
    d = os.path.join(app.args.bmdir, "static")
    log("requested static path:", path, "---", os.path.join(d, path))
    return send_from_directory(d, path, cache_timeout=1)  # timeout in seconds


@app.route("/bm/<commit>/<run>/<resultjson>")
def bm_(commit, run, resultjson):
    commit = escape(commit)
    run = escape(run)
    resultjson = escape(resultjson)
    d = os.path.join(app.args.bmdir, "runs", commit, run)
    log("requested result:", os.path.join(d, resultjson))
    return send_from_directory(d, resultjson, cache_timeout=1)  # timeout in seconds


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

def download_deps():
    deps = [
        "https://code.jquery.com/jquery-3.3.1.js",
        "https://code.jquery.com/jquery-3.3.1.js",
        "https://code.jquery.com/ui/1.12.1/jquery-ui.js",
        "https://cdn.datatables.net/1.10.20/js/jquery.dataTables.js",
        "https://cdn.datatables.net/1.10.20/js/jquery.dataTables.min.js",
        "https://cdn.datatables.net/1.10.20/css/jquery.dataTables.css",
        "https://cdn.datatables.net/1.10.20/css/jquery.dataTables.min.css",
        "https://www.chartjs.org/dist/2.9.1/Chart.min.js",
        #("https://cdnjs.cloudflare.com/ajax/libs/highlight.js/10.3.2/styles/github.css", "highlight.github.css"),
        ("https://cdnjs.cloudflare.com/ajax/libs/highlight.js/10.3.2/styles/github.min.css", "highlight.github.min.css"),
        #"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/10.3.2/highlight.js",
        "https://cdnjs.cloudflare.com/ajax/libs/highlight.js/10.3.2/highlight.min.js",
    ]
    for src in deps:
        if type(src) == str:
            base = os.path.basename(src)
        else:
            src, base = src
        dst = f"{os.getcwd()}/static/{base}"
        download_url(src, dst)


def download_url(url, dst):
    log("download url:", url, "--->", dst)
    req = requests.get(url, stream=True)
    if req.status_code == 200:
        sz = 0
        with open(dst, 'wb') as f:
            for chunk in req:
                f.write(chunk)
                sz += len(chunk)
        log(f"........ finished: {sz}B")
    else:
        log(f"         error:", req.status_code, url)


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class BenchmarkCollection:

    @staticmethod
    def create_new(args):
        dir = args.target
        filename = os.path.join(dir, "bm.yml")
        manifest = os.path.join(dir, "manifest.yml")
        if not os.path.exists(dir):
            os.makedirs(dir)
        shutil.copyfile(args.filename, filename)
        dump_yml(load_yml("""{runs: {}, bm: {}}"""), manifest)
        return __class__(dir)

    def __init__(self, dir):
        if not os.path.exists(dir):
            raise Exception(f"not found: {dir}")
        self.dir = os.path.abspath(dir)
        self.runs_dir = os.path.join(self.dir, "runs")
        self.manifest = os.path.join(self.dir, "manifest.yml")
        self.filename = os.path.join(self.dir, "bm.yml")
        self.specs = munchify(load_yml_file(self.filename))
        self.manif = munchify(load_yml_file(self.manifest))

    def add(self, results_dir):
        results_dir = os.path.abspath(results_dir)
        dst_dir, meta = self._read_run(results_dir)
        self._add_run(results_dir, dst_dir, meta)
        dump_yml(self.manif, self.manifest)

    def _read_run(self, results_dir):
        log("adding run...")
        id = f"{len(self.manif.runs.keys()):05d}"
        log(f"adding run: id={id}")
        meta = ResultMeta.load(results_dir)
        dst_dir = os.path.join(self.runs_dir, meta.name)
        return dst_dir, meta

    def _add_run(self, results_dir, dst_dir, meta):
        cats = self._add_meta_categories(meta)
        for filename in ("meta.yml",
                         "CMakeCCompiler.cmake",
                         "CMakeCXXCompiler.cmake",
                         "CMakeSystem.cmake",
                         "compile_commands.json"):
            filename = os.path.join(results_dir, filename)
            if os.path.exists(filename):
                copy_file_to_dir(filename, dst_dir)
            else:
                if not filename.endswith("compile_commands.json"):
                    raise Exception(f"wtf???? {filename}")
        for name, specs in self.specs.bm.items():
            if not hasattr(specs, 'variants'):
                filename = chk(f"{results_dir}/{name}.json")
                dst = copy_file_to_dir(filename, dst_dir)
                self._add_bm_run(name, specs, meta)
            else:
                for t in specs.variants:
                    tname = f"{name}-{t}"
                    filename = chk(f"{results_dir}/{tname}.json")
                    dst = copy_file_to_dir(filename, dst_dir)
                    self._add_bm_run(tname, specs, meta)

    def _add_bm_run(self, name, specs, meta):
        if name not in self.manif.bm.keys():
            self.manif.bm[name] = Munch(specs=specs, entries=[])
        entry = self.manif.bm[name]
        entry.specs = specs
        if meta.name not in entry.entries:
            entry.entries.append(meta.name)

    def _add_meta_categories(self, meta):
        run = Munch()
        for catname in ('commit', 'cpu', 'system', 'build'):
            meta_item = getattr(meta, catname)
            self._add_item_to_category(meta.name, catname, meta_item)
            run[catname] = meta_item.storage_id
        # build specs are too verbose; remove them
        self.manif.build[meta.build.storage_id].specs = Munch()
        self.manif.runs[meta.name] = run

    def _add_item_to_category(self, run, category_name, item):
        if not hasattr(self.manif, category_name):
            setattr(self.manif, category_name, Munch())
        category = getattr(self.manif, category_name)
        if item.storage_id not in category.keys():
            category[item.storage_id] = Munch(specs=item, entries=[])
        entry = category[item.storage_id]
        entry.specs = item
        if run not in entry.entries:
            entry.entries.append(run)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

class ResultMeta(Munch):

    def __init__(self, results_dir, cmakecache, build_type):
        super().__init__(self)
        self.date = __class__.get_date()
        self.commit = __class__.get_commit(results_dir)
        self.cpu = __class__.get_cpu_info()
        self.system = __class__.get_sys_info()
        self.build = __class__.get_build_info(cmakecache, build_type)
        self.name = self._get_name()

    @staticmethod
    def load(results_dir):
        results_dir = os.path.join(os.path.abspath(results_dir), "meta.yml")
        data = load_yml_file(results_dir)
        return munchify(data)

    def save(self, results_dir):
        out = os.path.join(results_dir, "meta.yml")
        log("saving meta:", out)
        dump_yml(self, out)
        self.build.save(results_dir)

    @staticmethod
    def get_date():
        import datetime
        now = datetime.datetime.now()
        return now.strftime("%Y%m%d-%H%M%S")

    def _get_name(self):
        commit = self.commit.storage_name
        cpu = self.cpu.storage_name
        sys = self.system.storage_name
        build = self.build.storage_name
        name = f"{commit}/{cpu}-{sys}-{build}"
        return name

    @staticmethod
    def get_commit(results_dir):
        import git
        repo = git.Repo(results_dir, search_parent_directories=True)
        commit = repo.head.commit
        commit = {p: str(getattr(commit, p))
                  for p in ('message', 'summary', 'name_rev',
                            'author',
                            'authored_datetime',
                            'committer',
                            'committed_datetime',)}
        commit = Munch(commit)
        commit.message = commit.message.strip()
        commit.sha1 = commit.name_rev[:7]
        spl = commit.authored_datetime.split(" ")
        date = re.sub(r'-', '', spl[0])
        time = re.sub(r'(\d+):(\d+):(\d+).*', r'\1\2\3', spl[1])
        commit.storage_id = commit.sha1
        commit.storage_name = f"git{date}_{time}-{commit.sha1}"
        return commit

    @staticmethod
    def get_cpu_info():
        import cpuinfo
        nfo = cpuinfo.get_cpu_info()
        nfo = Munch(nfo)
        for a in ('cpu_version', 'cpu_version_string', 'python_version'):
            if hasattr(nfo, a):
                delattr(nfo, a)
        for a in ('arch_string_raw', 'brand_raw', 'hardware_raw', 'vendor_id_raw'):
            if not hasattr(nfo, a):
                setattr(nfo, a, '')
        nfo.storage_id = myhash(
            nfo.arch_string_raw, nfo.brand_raw, nfo.hardware_raw, nfo.vendor_id_raw,
            nfo.arch, nfo.bits, nfo.count, nfo.family, nfo.model, nfo.stepping,
            ",".join(nfo.flags), nfo.hz_advertised_friendly,
            nfo.l2_cache_associativity,
            nfo.l2_cache_line_size,
            nfo.l2_cache_size,
            nfo.l3_cache_size,
            *optionals('l1_data_cache_size', 'l1_instruction_cache_size')
        )
        nfo.storage_name = f"{nfo.arch.lower()}_{nfo.storage_id}"
        return nfo

    @staticmethod
    def get_sys_info():
        import platform
        uname = platform.uname()
        nfo = Munch(
            sys_platform=sys.platform,
            sys=platform.system(),
            uname=Munch(
                machine=uname.machine,
                node=uname.node,
                release=uname.release,
                system=uname.system,
                version=uname.version,
            )
        )
        nfo.storage_id = myhash(
            nfo.sys_platform,
            nfo.uname.machine,
        )
        nfo.storage_name = f"{nfo.sys_platform}_{nfo.storage_id}"
        return nfo

    @staticmethod
    def get_build_info(cmakecache_txt, buildtype):
        nfo = CMakeCache(cmakecache_txt)
        def _btflags(name):
            return (getattr(nfo, name), getattr(nfo, f"{name}_{buildtype.upper()}"))
        nfo.storage_id = myhash(
            buildtype,
            nfo.CMAKE_CXX_COMPILER_ID,
            nfo.CMAKE_CXX_COMPILER_VERSION,
            nfo.CMAKE_CXX_COMPILER_VERSION_INTERNAL,
            nfo.CMAKE_CXX_COMPILER_ABI,
            nfo.CMAKE_CXX_SIZEOF_DATA_PTR,
            nfo.CMAKE_C_COMPILER_ID,
            nfo.CMAKE_C_COMPILER_VERSION,
            nfo.CMAKE_C_COMPILER_VERSION_INTERNAL,
            nfo.CMAKE_C_COMPILER_ABI,
            nfo.CMAKE_C_SIZEOF_DATA_PTR,
            *_btflags("CMAKE_CXX_FLAGS"),
            *_btflags("CMAKE_C_FLAGS"),
            *_btflags("CMAKE_STATIC_LINKER_FLAGS"),
            *_btflags("CMAKE_SHARED_LINKER_FLAGS"),
        )
        #
        ccname = nfo.CMAKE_CXX_COMPILER_ID.lower()
        if ccname == "gnu":
            ccname = "gcc"
        ccname += nfo.CMAKE_CXX_COMPILER_VERSION.lower()
        #
        if nfo.CMAKE_C_SIZEOF_DATA_PTR == "4":
            bits = "32bit"
        elif nfo.CMAKE_C_SIZEOF_DATA_PTR == "8":
            bits = "64bit"
        else:
            raise Exception("unknown architecture")
        #
        nfo.storage_name = f"{bits}_{buildtype}_{ccname}_{nfo.storage_id}"
        return nfo


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

class CMakeCache(Munch):

    def __init__(self, cmakecache_txt):
        import glob
        for line in iter_cmake_lines(cmakecache_txt):
            spl = line.split("=")
            if len(spl) < 2:
                continue
            k, ty = spl[0].split(":")
            v = "=".join(spl[1:]).strip()
            setattr(self, k, v)
        bdir = os.path.dirname(os.path.abspath(cmakecache_txt))
        self._c_compiler_file = sorted(glob.glob(f"{bdir}/CMakeFiles/*/CMakeCCompiler.cmake"))[-1]  # get the last
        self._cxx_compiler_file = sorted(glob.glob(f"{bdir}/CMakeFiles/*/CMakeCXXCompiler.cmake"))[-1]  # get the last
        self._system_file = sorted(glob.glob(f"{bdir}/CMakeFiles/*/CMakeSystem.cmake"))[-1]  # get the last
        self._load_cmake_file(self._c_compiler_file)
        self._load_cmake_file(self._cxx_compiler_file)
        ccomfile = f"{bdir}/compile_commands.json"
        self._compile_commands_file = ccomfile if os.path.exists(ccomfile) else None

    def _load_cmake_file(self, filename):
        for line in iter_cmake_lines(filename):
            if not line.startswith("set("):
                continue
            k = re.sub(r"set\((.*)\ +(.*)\)", r"\1", line)
            v = re.sub(r"set\((.*)\ +(.*)\)", r"\2", line)
            v = v.strip('"').strip("'").strip()
            setattr(self, k, v)

    def save(self, results_dir):
        copy_file_to_dir(self._c_compiler_file, results_dir)
        copy_file_to_dir(self._cxx_compiler_file, results_dir)
        copy_file_to_dir(self._system_file, results_dir)
        if self._compile_commands_file is not None:
            copy_file_to_dir(self._compile_commands_file, results_dir)


def iter_cmake_lines(filename):
    with open(filename) as f:
        for line in f.readlines():
            line = line.strip()
            if line.startswith("#") or line.startswith("//") or len(line) == 0:
                continue
            yield line


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class BenchmarkRun(Munch):
    "results of an individual run"

    def __init__(self, json_file, meta_class):
        props = load_json(json_file)
        setattr(self, "filename", json_file)
        assert hasattr(props, "context")
        assert hasattr(props, "benchmarks")
        super().__init__(**props)
        for e in self.benchmarks:
            setattr(e, 'meta', meta_class(e.name))
        setattr(self, 'property_names', (
            'meta',
            'shorttitle',
            'name',
            'run_name',
            'run_type',
            'repetitions',
            'repetition_index',
            'repetition_index',
            'threads',
            'iterations',
            'real_time',
            'cpu_time',
            'time_unit',
            'bytes_per_second',
            'items_per_second',
            'counters',
        ))

    @property
    def first(self):
        return self.benchmarks[0]

    @property
    def entries(self):
        for entry in self.benchmarks:
            yield entry

    @property
    def meta(self):
        for entry in self.benchmarks:
            yield entry.meta

    @property
    def filtered_names(self):
        for entry in self.benchmarks:
            yield entry.meta.shorttitle

    @property
    def names(self):
        for entry in self.benchmarks:
            yield entry.name

    @property
    def run_names(self):
        for entry in self.benchmarks:
            yield entry.run_name

    @property
    def run_types(self):
        for entry in self.benchmarks:
            yield entry.run_type

    @property
    def repetitions(self):
        for entry in self.benchmarks:
            yield entry.repetitions

    @property
    def repetition_indices(self):
        for entry in self.benchmarks:
            yield entry.repetition_index

    @property
    def threads(self):
        for entry in self.benchmarks:
            yield entry.threads

    @property
    def iterations(self):
        for entry in self.benchmarks:
            yield entry.iterations

    @property
    def real_time(self):
        for entry in self.benchmarks:
            yield entry.real_time

    @property
    def cpu_time(self):
        for entry in self.benchmarks:
            yield entry.cpu_time

    @property
    def time_unit(self):
        for entry in self.benchmarks:
            yield entry.time_unit

    @property
    def bytes_per_second(self):
        for entry in self.benchmarks:
            yield entry.bytes_per_second

    @property
    def items_per_second(self):
        for entry in self.benchmarks:
            yield entry.items_per_second


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class BenchmarkPanel:

    def __init__(self, runs, bm_meta_cls=None):
        self.runs = [BenchmarkRun(a, bm_meta_cls) for a in runs]

    def plot_bars(self, title):
        plot_benchmarks_as_lines(title, *self.runs)


    def plot_all_lines(self, title):
        plot_benchmarks_as_lines(title, *self.runs,
                                 transform=lambda r: r.meta.num_pixels,
                                 line_title_transform=lambda r: r.meta.shortname)


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------


def plot_benchmarks_as_bars(title, *bm, transform=None):
    from bokeh.models import ColumnDataSource, FactorRange
    from bokeh.plotting import figure, show
    from bokeh.transform import factor_cmap
    pass




def plot_benchmarks_as_lines(title, *bm, transform=None,
            line_title_transform=None,
            logx=True, logy=True):
    import bokeh
    from bokeh.plotting import figure, output_file, show
    from bokeh.palettes import Dark2_5 as palette
    from bokeh.layouts import row, column
    from bokeh.models import (Legend, LegendItem, CheckboxGroup, CustomJS, Div,
                              RadioGroup, Toggle,
                              ColumnDataSource, DataTable, TableColumn)
    from bokeh.models.markers import marker_types
    import itertools
    #
    ids = entry_ids(*bm, transform=transform)
    colors = itertools.cycle(palette)
    markers = itertools.cycle(marker_types)
    p = figure(title=title,
               x_axis_type="log" if logx else "linear",
               y_axis_type="log" if logy else "linear",
               #background_fill_color="#fafafa",
               plot_width=1000,
               x_axis_label="Number of pixels",
               y_axis_label="Throughput (MB/s)",
               )
    p.toolbar.autohide = True
    #p.toolbar.active_inspect = [hover_tool, crosshair_tool]
    p.toolbar.active_drag = "auto"
    p.toolbar.active_scroll = "auto"
    #
    def dft(v): return v if v else (lambda n: n)
    tr = dft(transform)
    lttr = dft(line_title_transform)
    #
    for results in bm:
        x = [ids[name] for name in results.names]
        y = [bps/1e6 for bps in results.bytes_per_second]
        c = next(colors)
        marker = next(markers)
        next(markers)  # advance two
        line_name = lttr(results.first)
        #legends.append(LegendItem(name=c, label=line_name))
        p.scatter(x, y, marker=marker, size=8, color=c, legend_label=line_name)
        p.line(x, y,
               color=c, alpha=0.9,
               #muted_color=c, muted_alpha=0.05,
               legend_label=line_name)
    p.legend.click_policy = "hide"
    p.legend.label_text_font_size = "10px"
    #
    def input_title(title):
        return Div(text=f"<h3>{title}</h3>")
    inputs = []
    first = bm[0].first.meta
    for k, g in first.checkbox_groups().items():
        cb = CheckboxGroup(labels=[str(v) for v in g],
                           active=[i for i in range(len(g))],
                           inline=True)
        inputs.append(input_title(k))
        inputs.append(cb)
    #
    # https://github.com/bokeh/bokeh/blob/branch-2.3/examples/app/export_csv/main.py
    x_axis_values = [f"{m.num_pixels}px" for m in bm[0].meta]
    table_sources = []
    for i, px in enumerate(x_axis_values):
        c = ColumnDataSource(data={
            'name': [nth(results.filtered_names, i) for results in bm],
            'bytes_per_second': [nth(results.bytes_per_second, i) for results in bm],
            'items_per_second': [nth(results.items_per_second, i) for results in bm],
            'cpu_time': [nth(results.real_time, i) for results in bm],
            'real_time': [nth(results.real_time, i) for results in bm],
            'iterations': [nth(results.iterations, i) for results in bm],
            'threads': [nth(results.threads, i) for results in bm],
        })
        table_sources.append(c)
    selected_x_index = 8  # FIXME (currently 2000 pixels)
    table_source = copy.deepcopy(table_sources[selected_x_index])
    relvalues = Toggle(label="Table: Relative values")
    px_title = input_title("Table: number of pixels")
    px_radiogroup = RadioGroup(labels=x_axis_values, active=selected_x_index)
    table_inputs = [relvalues, px_title, px_radiogroup]
    #
    table_cols = [
        TableColumn(field='name', title='Name'),
        TableColumn(field='bytes_per_second', title='Bytes/second'),
        TableColumn(field='items_per_second', title='Items/second'),
        TableColumn(field='cpu_time', title='CPU time'),
        TableColumn(field='real_time', title='Real time'),
        TableColumn(field='iterations', title='Iterations'),
        TableColumn(field='threads', title='Threads'),
    ]
    data_table = DataTable(source=table_source, columns=table_cols, width=1200)
    callback = CustomJS(args=dict(
        radiogroup=px_radiogroup,
        source=table_source,
        table=table_sources
    ), code="""
    console.log(`active=${radiogroup.active}`);
    /*source.data=table[radiogroup.active];*/
    var nrows = source.data['name'].length;
    var ts = table[radiogroup.active].data;
    var names = ["name", "bytes_per_second", "items_per_second", "cpu_time", "real_time", "iterations", "threads"];
    var ncols = names.length;
    console.log(`names=${names} nrows=${nrows} ncols=${ncols}`);
    for(var i = 0; i < nrows; i++) {
        for(var j = 0; j < ncols; ++j) {
           var name = names[j];
           /*console.log(`i=${i} j=${j} name=${name}`);*/
           source.data[name][i] = ts[name][i];
        }
    }
    source.change.emit();
    """)
    px_radiogroup.js_on_change('active', callback)
    #                        lambda attr, old, new: log(f"attr={attr} old={old} new={new} active={px_radiogroup.active}"))
    #
    layout = column(
        row(column(*inputs), p),
        row(column(*table_inputs), data_table))
    show(layout)


def chain(*iterables):
    for it in iterables:
        for elm in it:
            yield elm


def entry_ids(*bm, transform=None):
    ids = {}
    curr = 0
    for results in bm:
        log(os.path.basename(results.filename), "------------------------------")
        for entry in results.entries:
            log(entry.name)
            if transform is not None:
                ids[entry.name] = transform(entry)
            else:
                if ids.get(entry.name) is None:
                    ids[entry.name] = curr
                    curr += 1
    return ids


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

def add_results(args):
    log("adding results:", args.results)
    col = BenchmarkCollection(args.target)
    col.add(args.results)


def add_meta(args):
    log("adding bm run metadata to results dir:", args.results)
    meta = ResultMeta(results_dir=args.results,
                      cmakecache=args.cmakecache,
                      build_type=args.build_type)
    meta.save(args.results)
    log("adding bm run metadata to results dir: success!")


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

import typing
import enum


class _enum(enum.Enum):
    def __str__(self):
        return str(self.name)
    @classmethod
    def err_unknown(cls, s):
        raise Exception(f"unknown {__class__.__name__}: {s}")


class MatrixOrder(_enum):
    row_major = "row_major"
    col_major = "col_major"
    @property
    def short(self):
        return "rm" if self is MatrixOrder.row_major else "cm"
    @classmethod
    def make(cls, s):
        try:
            return {"rm": cls.row_major, "cm": cls.col_major}[s]
        except:
            cls.err_unknown(s)


class MatrixLayout(_enum):
    compact = "compact"
    strided = "strided"
    @classmethod
    def make(cls, s):
        try:
            return cls[s]
        except:
            cls.err_unknown(s)


class DimensionBinding(_enum):
    compile_time = "compile_time"
    run_time = "run_time"
    @property
    def short(self):
        return "ct" if self is DimensionBinding.compile_time else "rt"
    @classmethod
    def make(cls, s):
        try:
            return {"ct": cls.compile_time, "rt": cls.run_time}[s]
        except:
            cls.err_unknown(s)


class MultType(_enum):
    naive = "naive"
    avx2 = "avx2"
    avx2_unroll2 = "avx2_unroll2"
    avx2_unroll4 = "avx2_unroll4"
    avx2_unroll8 = "avx2_unroll8"
    @classmethod
    def make(cls, s):
        try:
            s = s.replace("dotprod_", "").replace("_naive", "")
            return cls[s]
        except:
            cls.err_unknown(s)


class MatrixMult(typing.NamedTuple):
    title: str
    num_pixels: int
    num_channels: int
    num_features: int
    mult_type: MultType
    layout: MatrixLayout
    dim_binding: DimensionBinding
    ret_order: MatrixOrder
    lhs_order: MatrixOrder
    rhs_order: MatrixOrder

    @classmethod
    def make(cls, bm_title: str):
        # eg:
        # mult_naive_strided_ct_rm_cmcm<250, 8, 16>
        # mult_naive_compact_rt_rm_rmrm/4000/8/16
        rxline = r'mult_(.*)[</](\d+)(?:/|, )(\d+)(?:/|, )(\d+).*'
        rxcase = r"(.*)_(compact|strided)_(ct|rt)_(rm|cm)_(rm|cm)(rm|cm)"
        case = re.sub(rxline, r'\1', bm_title)
        return cls(
            title=case,
            num_pixels=int(re.sub(rxline, r'\2', bm_title)),
            num_channels=int(re.sub(rxline, r'\3', bm_title)),
            num_features=int(re.sub(rxline, r'\4', bm_title)),
            mult_type=MultType.make(re.sub(rxcase, r'\1', case)),
            layout=MatrixLayout.make(re.sub(rxcase, r'\2', case)),
            dim_binding=DimensionBinding.make(re.sub(rxcase, r'\3', case)),
            ret_order=MatrixOrder.make(re.sub(rxcase, r'\4', case)),
            lhs_order=MatrixOrder.make(re.sub(rxcase, r'\5', case)),
            rhs_order=MatrixOrder.make(re.sub(rxcase, r'\6', case))
        )

    def comparison_axes(self):
        return ('num_pixels', 'num_channels', 'num_features')

    def checkbox_groups(self):
        return {
            'multiplication method': [t for t in MultType],
            'layout': [t for t in MatrixLayout],
            'dimension': [d for d in DimensionBinding],
            'return matrix ordering': [o for o in MatrixOrder],
            'lhs matrix ordering': [o for o in MatrixOrder],
            'rhs matrix ordering': [o for o in MatrixOrder],
        }

    @property
    def matrix_size(self):
        return self.num_pixels * self.num_channels

    @property
    def classifier_size(self):
        return self.num_channels * self.num_features

    @property
    def shortname(self):
        m = self
        return f"{m.mult_type}/{m.layout}/{m.dim_binding.short}_{m.ret_order.short}_{m.lhs_order.short}{m.rhs_order.short}"

    @property
    def shortparams(self):
        m = self
        return f"{m.num_pixels:04d}px{m.num_channels:02d}ch{m.num_features:02d}ft"

    @property
    def shorttitle(self):
        return f"{self.shortname}/{self.shortparams}"


def _test():
    def expect(v_, attr, val):
        var = getattr(v_, attr)
        if var != val:
            raise Exception(f"{attr}:  expected={val}   actual={var}")
    #
    v = MatrixMult.make("mult_naive_strided_ct_rm_cmcm<250, 8, 16>")
    expect(v, 'title', 'naive_strided_ct_rm_cmcm')
    expect(v, 'num_pixels', 250)
    expect(v, 'num_channels', 8)
    expect(v, 'num_features', 16)
    expect(v, 'mult_type', MultType.naive)
    expect(v, 'layout', MatrixLayout.strided)
    expect(v, 'dim_binding', DimensionBinding.compile_time)
    expect(v, 'ret_order', MatrixOrder.row_major)
    expect(v, 'lhs_order', MatrixOrder.col_major)
    expect(v, 'rhs_order', MatrixOrder.col_major)
    v = MatrixMult.make("mult_dotprod_avx2_compact_rt_cm_rmcm/4000/16/8")
    expect(v, 'title', 'dotprod_avx2_compact_rt_cm_rmcm')
    expect(v, 'num_pixels', 4000)
    expect(v, 'num_channels', 16)
    expect(v, 'num_features', 8)
    expect(v, 'mult_type', MultType.avx2)
    expect(v, 'layout', MatrixLayout.compact)
    expect(v, 'dim_binding', DimensionBinding.run_time)
    expect(v, 'ret_order', MatrixOrder.col_major)
    expect(v, 'lhs_order', MatrixOrder.row_major)
    expect(v, 'rhs_order', MatrixOrder.col_major)

_test()



def formatMBps(value):
    return value / 1e6



if __name__ == '__main__':
    bms = sorted(sys.argv[2:])
    log(bms)
    bms = BenchmarkPanel(bms, bm_meta_cls=MatrixMult.make)
    fm = bms.runs[0].first.meta
    title = f"Classifier multiplication, {fm.num_channels} channels, {fm.num_features} features: throughput (MB/s)"
    bms.plot_all_lines(title)
    exit()
    main()
