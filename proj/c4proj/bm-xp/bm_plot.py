import os
import sys
import copy
import re
import itertools
import typing
import enum

# https://stackoverflow.com/questions/11351032/named-tuple-and-default-values-for-optional-keyword-arguments
from dataclasses import dataclass

from munch import Munch, munchify

import bokeh.io as bki
import bokeh.models as bkm
import bokeh.plotting as bkp
import bokeh.transform as bkt
import bokeh.layouts as bkl
from bokeh.models.markers import marker_types as bk_markers
# https://docs.bokeh.org/en/latest/docs/reference/palettes.html
from bokeh.palettes import d3 as bk_palette_d3
bk_palette = bk_palette_d3['Category20c'][20]

# saving bokeh to png is not working, so we save png using matplotlib
import matplotlib.pyplot as plt
import matplotlib.ticker as plttck
plt_markers = [c for c in ".,ov^<>1234spP*hH+xXDdl"]

from bm_util import _enum
from bm_util import *
from bm_run import BenchmarkRun, BenchmarkPanel


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------


# https://stackoverflow.com/questions/11351032/named-tuple-and-default-values-for-optional-keyword-arguments
@dataclass
class BarChartSpecs:
    horizontal: bool = True
    bar_width: float = 0.9


@dataclass
class LineChartSpecs:
    width: int = 1000
    xlog: bool = False
    ylog: bool = False
    xlabel: str = ""
    ylabel: str = ""


def _plt_save_png(name):
    log(name)
    plt.savefig(name, bbox_inches='tight', dpi=100)


def _plt_clear():
    plt.clf()


def _bokeh_save_html(name, p):
    log(name)
    bki.save(p, name)


def _bokeh_adjust_figure_props(p):
    p.toolbar.autohide = True
    #p.toolbar.active_inspect = [hover_tool, crosshair_tool]
    p.toolbar.active_drag = "auto"
    p.toolbar.active_scroll = "auto"
    p.legend
    p.legend.click_policy = "hide"
    p.legend.label_text_font_size = "10px"


def bokeh_plot_many(plots, name: str, ncols: int = 2):
    layout = bkl.gridplot(plots, ncols=ncols)
    _bokeh_save_html(name, layout)
    #bkp.show(layout)


def plot_benchmark_run_as_bars(bm: BenchmarkRun, title: str,
                               bar_names, bar_values, bar_label,
                               **kwargs):
    kwargs = BarChartSpecs(**kwargs)
    #
    palette = itertools.cycle(bk_palette)
    colors = [next(palette) for _ in bar_names]
    #
    fig_args_bokeh = {
        "title": title,
        #"toolbar_location": None,
        #"tools": ""
    }
    if kwargs.horizontal:
        #
        # plot with bokeh (interactive, but cannot export png)
        rnames = list(reversed(bar_names))
        rvalues = list(reversed(bar_values))
        rcolors = list(reversed(colors))
        p = bkp.figure(y_range=rnames, **fig_args_bokeh)
        p.hbar(y=rnames, right=rvalues, fill_color=rcolors,
               line_color=rcolors, height=kwargs.bar_width)
        p.ygrid.grid_line_color = None
        p.x_range.start = 0
        p.xaxis.axis_label = bar_label
        #
        # plot with matplotlib (to export png)
        p_ = plt.barh(y=rnames, width=rvalues, color=rcolors,
                      height=kwargs.bar_width)
        plt.gca().xaxis.grid(True)
        plt.gca().xaxis.set_minor_locator(plttck.AutoMinorLocator())
        plt.xlabel(bar_label, fontsize='small')
        plt.yticks(fontsize='x-small')
        plt.title(title)
    else:
        #
        # plot with bokeh (interactive, but cannot export png)
        p = bkp.figure(x_range=bar_names, **fig_args_bokeh)
        p.vbar(x=bar_names, top=bar_values, fill_color=colors,
               line_color=colors, width=kwargs.bar_width)
        p.xaxis.major_label_orientation = 1
        p.xgrid.grid_line_color = None
        p.y_range.start = 0
        p.yaxis.axis_label = bar_label
        #
        # plot with matplotlib (to export png)
        p_ = plt.bar(x=bar_names, height=bar_values, color=colors,
                     width=kwargs.bar_width)
        plt.gca().yaxis.grid(True)
        plt.gca().yaxis.set_minor_locator(plttck.AutoMinorLocator())
        plt.ylabel(bar_label, fontsize='small')
        plt.xticks(fontsize='x-small')
        plt.title(title)
    _bokeh_adjust_figure_props(p)
    return p, p_


def plot_benchmark_panel_as_lines(bm_panel: BenchmarkPanel, title: str,
                                  xget, yget, nameget,
                                  **kwargs):
    kwargs = LineChartSpecs(**kwargs)
    #
    colors = itertools.cycle(bk_palette)
    markers = itertools.cycle(bk_markers)
    markers_ = itertools.cycle(plt_markers)
    #
    # plot with bokeh (interactive, but cannot export png)
    p = bkp.figure(title=title,
                   x_axis_type="log" if kwargs.xlog else "linear",
                   y_axis_type="log" if kwargs.ylog else "linear",
                   #background_fill_color="#fafafa",
                   x_axis_label=kwargs.xlabel,
                   y_axis_label=kwargs.ylabel,
                   plot_width=kwargs.width,
                   )
    # plot with matplotlib (to export png)
    plt.title(title)
    for bm in bm_panel.runs:
        x = xget(bm)
        y = yget(bm)
        line_name = nameget(bm)
        color = next(colors)
        marker = next(markers)
        marker_ = next(markers_)
        # plot with bokeh (interactive, but cannot export png)
        #legends.append(LegendItem(name=c, label=line_name))
        p.scatter(x, y, marker=marker, size=8, color=color,
                  legend_label=line_name)
        p.line(x, y, color=color, alpha=0.9,
               #muted_color=c, muted_alpha=0.05,
               legend_label=line_name)
        #
        # plot with matplotlib (to export png)
        plt.plot(x, y, f'-{marker_}', color=color, label=line_name)
        plt.gca().xaxis.grid(True)
        plt.gca().yaxis.grid(True)
        plt.xscale("log" if kwargs.xlog else "linear")
        plt.yscale("log" if kwargs.ylog else "linear")
        plt.xlabel(kwargs.xlabel, fontsize='small')
        plt.ylabel(kwargs.ylabel, fontsize='small')
    plt.gca().legend(loc='center left', bbox_to_anchor=(1, 0.5), fontsize='x-small')
    _bokeh_adjust_figure_props(p)
    return p


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

####### old code: to remove and tidy up

@dataclass
class CharconvMeta:  # also for atox
    title: str
    subject: str
    function: str
    data_type: FundamentalTypes

    @classmethod
    def make(cls, bm_title: str):
        # eg:
        #   xtoa_c4_write_dec<uint8_t>
        #   xtoa_c4_utoa<uint8_t>
        #   xtoa_c4_xtoa<uint8_t>
        #   xtoa_c4_to_chars<uint8_t>
        #   xtoa_std_to_chars<uint8_t>
        #   xtoa_std_to_string<uint8_t>
        #   xtoa_sprintf<uint8_t>
        #   xtoa_sstream_reuse<uint8_t>
        #   xtoa_sstream<uint8_t>
        rx = re.compile(r'(atox|xtoa|xtoahex|xtoaoct|xtoabin)_(.*?)<(u?int\d+_t)>')
        if not rx.fullmatch(bm_title):
            raise Exception(f"cannot understand bm title: {bm_title}")
        subject = rx.sub(r'\1', bm_title)
        function = rx.sub(r'\2', bm_title)
        data_type = rx.sub(r'\3', bm_title)
        return cls(
            title=bm_title,
            subject=subject,
            function=function.replace("c4_", "c4::").replace("std_", "std::"),
            data_type=FundamentalTypes.make(data_type)
        )

    def checkbox_groups(self):
        return {
            'data_type': [t for t in FundamentalTypes],
        }

    @property
    def shortname(self):
        return self.function

    @property
    def shortparams(self):
        return str(self.data_type.short)

    @property
    def shorttitle(self):
        return f"{self.shortname}<{self.shortparams}>"


@dataclass
class CharconvThreadsMeta:
    function: str
    num_threads: int

    @classmethod
    def make(cls, bm_title: str):
        # eg:
        #   c4_itoa/real_time/threads:4
        rx = re.compile(r'(.*?)/real_time/threads:(\d+)')
        if not rx.fullmatch(bm_title):
            raise Exception(f"cannot understand bm title: {bm_title}")
        function = rx.sub(r'\1', bm_title)
        num_threads = int(rx.sub(r'\2', bm_title))
        return cls(
            function=function.replace("c4_", "c4::").replace("std_", "std::"),
            num_threads=num_threads
        )

    def checkbox_groups(self):
        return {}

    @property
    def shortname(self):
        return self.function

    @property
    def shorttitle(self):
        return self.shortname


def plot_charconv_bars(bm_panel: BenchmarkPanel, panel_title_human: str, outputfile_prefix: str):
    assert os.path.isabs(outputfile_prefix), outputfile_prefix
    for prop in ("mega_bytes_per_second", "cpu_time_ms"):
        ps, ps_ = [], []
        pd = bm_panel.first_run.property_plot_data(prop)
        bar_label = f"{pd.human_name_short}{pd.what_is_better}"
        outfilename = f"{outputfile_prefix}-{prop}"
        for bm in bm_panel.runs:
            bar_names = [m.shorttitle for m in bm.meta]
            bar_values = list(getattr(bm, prop))
            data_type = first(bm.meta).data_type
            # to save each bokeh plot separately and also
            # a grid plot with all of them, we have to plot
            # twice because bokeh does not allow saving twice
            # the same plot from multiple pictures.
            plotit = lambda: plot_benchmark_run_as_bars(bm, title=f"{panel_title_human}: {data_type}\n{bar_label}",
                                                        bar_names=bar_names, bar_values=bar_values, bar_label=bar_label)
            # make one plot to save:
            p, p_ = plotit()
            _bokeh_save_html(f"{outfilename}-{data_type.short}.html", p)
            _plt_save_png_and_clear(f"{outfilename}-{data_type.short}.png")
            # and another to gather:
            p, p_ = plotit()
            ps.append(p)
            ps_.append(p_)
        layout = bkl.gridplot(ps, ncols=2)
        _bokeh_save_html(f"{outfilename}.html", layout)
        # now show
        #bkp.show(layout)


def plot_charconv_threads_(bm_panel: BenchmarkPanel, panel_title_human: str, outputfile_prefix: str):
    assert os.path.isabs(outputfile_prefix), outputfile_prefix
    orig = lambda yprop: lambda bm: list(bm.extract_plot_series(yprop))
    divnt = lambda yprop: lambda bm: [v / n for v, n in bm.extract_plot_series_with_threads(yprop)]
    mulnt = lambda yprop: lambda bm: [v * n for v, n in bm.extract_plot_series_with_threads(yprop)]
    xprop = "threads"
    xpd = bm_panel.first_run.property_plot_data(xprop)
    xlabel = f"{xpd.human_name_short}"
    for yprop, ylog, yget in (
            #("mega_items_per_second", False, orig),
            ("mega_bytes_per_second", False, orig),
            #("iterations", False, divnt),
            #("real_time_ms", True, mulnt),
            ("cpu_time_ms", True, orig),):
        ypd = bm_panel.first_run.property_plot_data(yprop)
        ylabel = f"{ypd.human_name_short}{ypd.what_is_better}"
        p = plot_benchmark_panel_as_lines(
            bm_panel, f"{panel_title_human}\n{ylabel}",
            xget=orig("threads"),
            yget=yget(yprop),
            nameget=lambda bm: first(bm.meta).function,
            ylog=ylog,
            xlabel=xlabel,
            ylabel=ylabel
        )
        name = f"{outputfile_prefix}-lines-{yprop}"
        # save png using matplotlib
        _plt_save_png_and_clear(f"{name}.png")
        # save html using bokeh
        _bokeh_save_html(f"{name}.html", p)
        #bkp.show(p)
    return p


def plot_charconv_threads(json_files, case: str = ""):
    case = f" [{case}]" if case else ""
    dir_ = os.path.dirname(first(json_files))
    panel = BenchmarkPanel(json_files, CharconvThreadsMeta)
    plot_charconv_threads_(panel,
                           f"itoa benchmark: convert 2M 32b integers to string{case}",
                           f"{dir_}/c4core-bm-charconv_threads")


def plot_charconv_xtoa(json_files, case: str = ""):
    case = f" [{case}]" if case else ""
    dir_ = os.path.dirname(first(json_files))
    panel = BenchmarkPanel(json_files, CharconvMeta)
    plot_charconv_bars(panel,
                       f"xtoa benchmark: convert 2M numbers to strings{case}",
                       f"{dir_}/c4core-bm-charconv-xtoa")


def plot_charconv_atox(json_files, case: str = ""):
    case = f" [{case}]" if case else ""
    dir_ = os.path.dirname(first(json_files))
    panel = BenchmarkPanel(json_files, CharconvMeta)
    plot_charconv_bars(panel,
                       f"atox benchmark: convert 2M strings to numbers{case}",
                       f"{dir_}/c4core-bm-charconv-atox")


def threads_data(dir_: str):
    assert os.path.exists(dir_), dir_
    return [
        f"{dir_}/c4core-bm-charconv_threads-c4_write_dec.json",
        f"{dir_}/c4core-bm-charconv_threads-c4_itoa.json",
        f"{dir_}/c4core-bm-charconv_threads-c4_xtoa.json",
        f"{dir_}/c4core-bm-charconv_threads-c4_to_chars.json",
        f"{dir_}/c4core-bm-charconv_threads-fmtlib_format_to.json",
        f"{dir_}/c4core-bm-charconv_threads-std_to_chars.json",
        f"{dir_}/c4core-bm-charconv_threads-snprintf.json",
        f"{dir_}/c4core-bm-charconv_threads-stb_snprintf.json",
        f"{dir_}/c4core-bm-charconv_threads-sstream.json",
        f"{dir_}/c4core-bm-charconv_threads-sstream_naive_reuse.json",
        f"{dir_}/c4core-bm-charconv_threads-sstream_naive.json",
    ]


def xtoa_data(dir_: str):
    assert os.path.exists(dir_), dir_
    return [
        f"{dir_}/c4core-bm-charconv-xtoa-int8.json",
        f"{dir_}/c4core-bm-charconv-xtoa-uint8.json",
        f"{dir_}/c4core-bm-charconv-xtoa-int16.json",
        f"{dir_}/c4core-bm-charconv-xtoa-uint16.json",
        f"{dir_}/c4core-bm-charconv-xtoa-int32.json",
        f"{dir_}/c4core-bm-charconv-xtoa-uint32.json",
        f"{dir_}/c4core-bm-charconv-xtoa-int64.json",
        f"{dir_}/c4core-bm-charconv-xtoa-uint64.json",
    ]


def atox_data(dir_: str):
    assert os.path.exists(dir_), dir_
    return [
        f"{dir_}/c4core-bm-charconv-atox-int8.json",
        f"{dir_}/c4core-bm-charconv-atox-uint8.json",
        f"{dir_}/c4core-bm-charconv-atox-int16.json",
        f"{dir_}/c4core-bm-charconv-atox-uint16.json",
        f"{dir_}/c4core-bm-charconv-atox-int32.json",
        f"{dir_}/c4core-bm-charconv-atox-uint32.json",
        f"{dir_}/c4core-bm-charconv-atox-int64.json",
        f"{dir_}/c4core-bm-charconv-atox-uint64.json",
    ]


def examples_dir():
    this_dir = os.path.dirname(os.path.abspath(__file__))
    exdir = f"{this_dir}/examples"
    assert os.path.exists(exdir), exdir
    return exdir


if __name__ == '__main__':
    xdir = examples_dir()
    #
    plot_charconv_threads(threads_data(f"{xdir}/lines/gcc11.2"), "gcc11.2")
    plot_charconv_threads(threads_data(f"{xdir}/lines/vs2022"), "vs2022")
    #
    plot_charconv_xtoa(xtoa_data(f"{xdir}/bars/xtoa/gcc11.2"), "gcc11.2")
    plot_charconv_xtoa(xtoa_data(f"{xdir}/bars/xtoa/vs2022"), "vs2022")
    #
    plot_charconv_atox(atox_data(f"{xdir}/bars/atox/gcc11.2"), "gcc11.2")
    plot_charconv_atox(atox_data(f"{xdir}/bars/atox/vs2022"), "vs2022")
    #
    exit()


# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------
# ------------------------------------------------------------------------------


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
