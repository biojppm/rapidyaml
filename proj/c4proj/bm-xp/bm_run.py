import copy
import os.path

# https://stackoverflow.com/questions/11351032/named-tuple-and-default-values-for-optional-keyword-arguments
from dataclasses import dataclass

from munch import Munch

from bm_util import load_json, first, _enum


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class QuantityType(_enum):
    neutral = ""
    more_is_better = "more is better"
    less_is_better = "less is better"

    @property
    def comment(self):
        return f" ({self.value})" if self.name else ""


_more = QuantityType.more_is_better
_less = QuantityType.less_is_better


@dataclass
class BenchmarkPropertyPlotData:
    human_name: str = ""
    human_name_short: str = ""
    qty_type: QuantityType = QuantityType.neutral


class BenchmarkRun(Munch):
    """results of an individual run"""

    def __init__(self, json_file: str, meta_class):
        """
        meta_class is a class to extract property values from the benchmark run
        """
        self._filename = json_file
        props = load_json(json_file)
        assert hasattr(props, "context")
        assert hasattr(props, "benchmarks")
        super().__init__(**props)
        setattr(self, 'property_names', list(__class__._properties.keys()))
        for bm in self.benchmarks:
            if meta_class is not None:
                setattr(bm, 'meta', meta_class.make(bm.name))
            else:
                setattr(bm, 'meta', None)

    _properties = {
        'filename': None,
        'basename': None,
        'dirname': None,
        'meta': None,
        'shorttitle': None,
        'name': None,
        'run_name': None,
        'run_type': None,
        'repetitions': None,
        'repetition_index': None,
        'threads': BenchmarkPropertyPlotData('number of threads', 'threads'),
        'iterations': BenchmarkPropertyPlotData('number of iterations', 'iterations', _more),
        'real_time': BenchmarkPropertyPlotData('real time', 'real time', _less),
        'cpu_time': BenchmarkPropertyPlotData('CPU time', 'cpu time', _less),
        'real_time_ms': BenchmarkPropertyPlotData('real time', 'real time', _less),
        'cpu_time_ms': BenchmarkPropertyPlotData('CPU time', 'cpu time', _less),
        'time_unit': None,
        'bytes_per_second': BenchmarkPropertyPlotData('Bytes/s', 'B/s', _more),
        'items_per_second': BenchmarkPropertyPlotData('items/s', 'items/s', _more),
        'mega_bytes_per_second': BenchmarkPropertyPlotData('MBytes/s', 'MB/s', _more),
        'mega_items_per_second': BenchmarkPropertyPlotData('Mega items/s', 'Mega items/s', _more),
        'counters': None,
    }

    def property_plot_data(self, property_name: str):
        pd = copy.deepcopy(__class__._properties.get(property_name, BenchmarkPropertyPlotData()))
        if property_name.endswith('_time'):
            time_unit = first(self.entries).time_unit
            pd.human_name += f' ({time_unit})'
            pd.human_name_short += f' ({time_unit})'
        elif property_name.endswith('_time_ms'):
            pd.human_name += ' (ms)'
            pd.human_name_short += ' (ms)'
        return pd

    def extract_plot_series(self, property_name_or_getter,
                            relative_to_entry = None,
                            percent_of_entry = None,
                            ):
        if isinstance(property_name_or_getter, str):
            series = getattr(self, property_name_or_getter)
        else:
            series = property_name_or_getter(self)
        series = list(series)
        def getrefval(ref):
            assert ref in self.entries, ref.name
            pos = self.pos(ref)
            assert pos in range(len(series)), (pos, len(series))
            return series[pos]
        if relative_to_entry:
            refval = getrefval(relative_to_entry)
            for v in series:
                yield v / refval
        elif percent_of_entry:
            refval = getrefval(percent_of_entry)
            for v in series:
                yield 100.0 * ((v - refval) / refval)
        else:
            for v in series:
                yield v

    def extract_plot_series_with_threads(self, property_name_or_getter,
                                         relative_to: str = None,
                                         percent_of: str = None,
                                         ):
        series = self.extract_plot_series(property_name_or_getter, relative_to=relative_to, percent_of=percent_of)
        for y, n in zip(series, self.threads):
            yield y, n

    def pos(self, entry):
        for i, e in enumerate(self.entries):
            if e == entry:
                return i
        raise Exception("entry not found")

    @property
    def filename(self):
        return self._filename

    @property
    def basename(self):
        return os.path.basename(self._filename)

    @property
    def dirname(self):
        return os.path.dirname(self._filename)

    @property
    def entries(self):
        for entry in self.benchmarks:
            yield entry

    @property
    def meta(self):
        for entry in self.benchmarks:
            yield entry.meta

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
    def real_time_ms(self):
        for entry in self.benchmarks:
            assert entry.time_unit == "ns"
            yield entry.real_time / 1e6

    @property
    def cpu_time_ms(self):
        for entry in self.benchmarks:
            assert entry.time_unit == "ns"
            yield entry.cpu_time / 1e6

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

    @property
    def mega_bytes_per_second(self):
        for entry in self.benchmarks:
            yield entry.bytes_per_second / 1e6

    @property
    def mega_items_per_second(self):
        for entry in self.benchmarks:
            yield entry.items_per_second / 1e6


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------

class BenchmarkPanel:

    def __init__(self, runs, bm_meta_cls=None):
        self.runs = [BenchmarkRun(a, bm_meta_cls) for a in runs]

    @property
    def first_run(self):
        return first(self.runs)
