import ryml
import ruamel.yaml
import yaml
import timeit
import time
import copy
import prettytable
import os.path
from collections import OrderedDict as odict


def _nodbg(*args, **kwargs):
    pass


def _dbg(*args, **kwargs):
    print(*args, **kwargs, file=sys.stderr, flush=True)


dbg = _dbg


class RunResults:

    __slots__ = ('name', 'time_ms', 'count', 'avg', 'MBps')

    def __init__(self, name, time_ms, count, num_bytes):
        self.name = name
        self.time_ms = time_ms
        self.count = count
        self.avg = time_ms / count
        num_megabytes = count * num_bytes / 1.0e6
        num_seconds = time_ms / 1000.0
        self.MBps = num_megabytes / num_seconds

    def __str__(self):
        fmt = "{}: count={} time={:.3f}ms avg={:.3f}ms MB/s={:.3f}"
        fmt = fmt.format(self.name, self.count, self.time_ms, self.avg, self.MBps)
        return fmt


class BmCase:

    def __init__(self, filename):
        with open(filename, "r") as f:
            src = f.read()
        self.filename = filename
        self.src_as_str = src
        self.src_as_bytes = bytes(src, "utf8")
        self.src_as_bytearray = bytearray(src, "utf8")
        self.src_as_bytearray_orig = copy.copy(self.src_as_bytearray)
        self.emittree = ryml.parse_in_arena(self.src_as_bytearray)
        self.emitbuf = bytearray(4 * len(self.src_as_str))  # should be enough

    def run(self, bm_method_name, cls):
        def run_bm(obj, subject):
            obj.count = 0
            t = timeit.Timer(subject)
            delta = time.time()
            result = t.autorange() #lambda number, time_taken: time_taken > 1.0)
            delta = 1000. * (time.time() - delta)
            return delta, obj.count
        obj = cls(self)
        if not hasattr(obj, bm_method_name):
            return None
        name = bm_method_name + ":" + cls.__name__
        dbg(name, "...")
        method = getattr(obj, bm_method_name)
        reset_name = 'reset_' + bm_method_name
        reset_fn = getattr(obj, reset_name, None)
        def bm_fn():
            method(self)
            obj.count += 1
            if reset_fn is not None:
                reset_fn(self)
        delta, count = run_bm(obj, bm_fn)
        # correct the benchmark to account for the time spent
        # resetting
        if reset_fn is not None:
            # find out how much it takes to reset the bytearray
            if not hasattr(obj, 'bm_reset_done'):
                def bm_reset():
                    reset_fn(self)
                    obj.count += 1
                rdelta, rcount = run_bm(obj, bm_reset)
                obj.bm_reset_time_per_iteration = rdelta / rcount
                dbg(name, "reset_time_per_iteration={:.3f}us".format(obj.bm_reset_time_per_iteration * 1000.0))
                obj.bm_reset_done = True
            reset_correction = count * obj.bm_reset_time_per_iteration
            dbg(name, "delta={:.3f}ms".format(delta), "reset_correction={:.3f}ms({:.2f}%)".format(reset_correction, 100.0 * reset_correction / delta))
            delta -= reset_correction
        ret = RunResults(name, delta, count, len(self.src_as_str))
        dbg(name, "ok:", ret)
        return ret


def run(case, benchmarks, approaches):
    for bm in benchmarks:
        results = odict()
        for cls in approaches:
            r = case.run(bm, cls)
            if r is None:
                continue
            results[r.name] = r
        table = prettytable.PrettyTable()
        name = os.path.basename(case.filename)
        table.field_names = [name, "count", "time(ms)", "avg(ms)", "avg(MB/s)"]
        table.align[name] = "l"
        def i(v): return "{:5d}".format(v)
        def f(v): return "{:8.3f}".format(v)
        for v in results.values():
            table.add_row([v.name, i(v.count), f(v.time_ms), f(v.avg), f(v.MBps)])
        print(table)


class BmCaseRun:
    def __init__(self, case):
        self.reset_bytearray = False


class RymlParseInArena(BmCaseRun):

    def parse(self, case):
        _ = ryml.parse_in_arena(case.src_as_bytearray)


class RymlParseInArenaReuse(BmCaseRun):

    def __init__(self, case):
        self.tree = ryml.Tree()

    def parse(self, case):
        ryml.parse_in_arena(case.src_as_bytearray, tree=self.tree)

    def reset_parse(self, case):
        self.tree.clear()
        self.tree.clear_arena()


class RymlParseInPlace(BmCaseRun):

    def parse(self, case):
        _ = ryml.parse_in_place(case.src_as_bytearray)

    def reset_parse(self, case):
        case.src_as_bytearray = copy.copy(case.src_as_bytearray_orig)


class RymlParseInPlaceReuse(BmCaseRun):

    def __init__(self, case):
        self.tree = ryml.Tree()

    def parse(self, case):
        ryml.parse_in_place(case.src_as_bytearray, tree=self.tree)

    def reset_parse(self, case):
        self.tree.clear()
        self.tree.clear_arena()
        case.src_as_bytearray = copy.copy(case.src_as_bytearray_orig)


class RuamelYamlParse(BmCaseRun):

    def parse(self, case):
        _ = ruamel.yaml.load(case.src_as_str, Loader=ruamel.yaml.Loader)


class PyYamlParse(BmCaseRun):

    def parse(self, case):
        _ = yaml.safe_load(case.src_as_str)


class RymlEmitToNewBuffer(BmCaseRun):

    def emit_yaml(self, case):
        _ = ryml.emit_yaml(case.emittree)

    def emit_json(self, case):
        _ = ryml.emit_json(case.emittree)


class RymlEmitReuse(BmCaseRun):

    def emit_yaml(self, case):
        _ = ryml.emit_yaml_in_place(case.emittree, case.emitbuf)

    def emit_json(self, case):
        _ = ryml.emit_json_in_place(case.emittree, case.emitbuf)


class RuamelYamlEmit:

    def __init__(self, case):
        case.ruamel_emittree = ruamel.yaml.load(case.src_as_str, Loader=ruamel.yaml.Loader)

    def emit_yaml(self, case):
        # https://stackoverflow.com/a/47617341/5875572
        class MyToStr:
            def __init__(self, *args, **kwargs):
                self.s = b""
            def write(self, s):
                self.s += s
        dumper = MyToStr()
        ruamel.yaml.YAML().dump(case.ruamel_emittree, MyToStr())


class PyYamlEmit:

    def __init__(self, case):
        case.pyyaml_emittree = yaml.load(case.src_as_str, Loader=yaml.Loader)

    def emit_yaml(self, case):
        _ = yaml.dump(case.pyyaml_emittree)


if __name__ == "__main__":
    import sys
    if len(sys.argv) < 2:
        raise Exception("")
    filename = sys.argv[1]
    if filename.endswith("outer1000_inner1000.yml"):  # this one is too heavy for the Python libs
        exit(0)
    case = BmCase(filename)
    run(case, benchmarks=('parse', ),
        approaches=(RuamelYamlParse,
                    PyYamlParse,
                    RymlParseInArena,
                    RymlParseInArenaReuse,
                    RymlParseInPlace,
                    RymlParseInPlaceReuse))
    run(case, benchmarks=('emit_yaml', 'emit_json', ),
        approaches=(RuamelYamlEmit,
                    PyYamlEmit,
                    RymlEmitToNewBuffer,
                    RymlEmitReuse))
