import sys
import os
import re

thisdir = os.path.dirname(os.path.abspath(__file__))
moddir = os.path.abspath(f"{thisdir}/../ext/c4core/cmake/bm-xp")
sys.path.insert(0, moddir)
import bm_plot as bm
from bm_util import first

from dataclasses import dataclass
import prettytable


def get_function_benchmark(libname, run: bm.BenchmarkRun):
    for rbm in run.entries:
        if rbm.meta.library == libname:
            return rbm
    raise Exception(f"lib not found: {libname}. Existing: {[rbm.meta.function for rbm in run.entries]}")


@dataclass
class ParseMeta:
    title: str
    library: str
    variant: str

    @classmethod
    def make(cls, bm_title: str):
        # eg:
        #-----------------------
        # RESERVE
        #   bm_ryml_inplace_reuse
        #   bm_ryml_arena_reuse
        #   bm_ryml_inplace
        #   bm_ryml_arena
        #   bm_libyaml_arena
        #   bm_libyaml_arena_reuse
        #   bm_libfyaml_arena
        #   bm_yamlcpp_arena
        #   bm_rapidjson_arena
        #   bm_rapidjson_inplace
        #   bm_sajson_arena
        #   bm_sajson_inplace
        #   bm_jsoncpp_arena
        #   bm_nlohmann_arena
        #-----------------------
        # EMIT
        #   bm_ryml_str_reserve
        #   bm_ryml_str
        #   bm_ryml_ostream
        #   bm_fyaml_str_reserve
        #   bm_fyaml_str
        #   bm_fyaml_ostream
        #   bm_yamlcpp
        #   bm_rapidjson
        #   bm_jsoncpp
        #   bm_nlohmann
        if not hasattr(__class__, '_rx'):
            __class__._rx = re.compile(r'bm_(ryml|libyaml|libfyaml|fyaml|yamlcpp|rapidjson|sajson|jsoncpp|nlohmann)_?(.*)')
        rx = __class__._rx
        if not rx.fullmatch(bm_title):
            raise Exception(f"cannot understand bm title: {bm_title}")
        lib = rx.sub(r'\1', bm_title)
        variant = rx.sub(r'\2', bm_title)
        return cls(
            title=bm_title,
            library=lib,
            variant=variant,
        )

    @property
    def shortname(self):
        return f"{self.library}_{self.variant}"

    @property
    def shortparams(self):
        return "params"

    @property
    def shorttitle(self):
        return self.shortname

EmitMeta = ParseMeta



def plot_bm_bars(bm_panel: bm.BenchmarkPanel, getref,
                 panel_title_human: str,
                 outputfile_prefix: str):
    assert os.path.isabs(outputfile_prefix), outputfile_prefix
    # make a comparison table
    atitle = lambda run: first(run.meta).shorttitle
    anchor = lambda run: f"{os.path.basename(outputfile_prefix)}-{atitle(run)}"
    anchorlink = lambda run: f"<pre><a href='#{anchor(run)}'>{atitle(run)}</a></pre>"
    with open(f"{outputfile_prefix}.txt", "w") as tablefile:
        with open(f"{outputfile_prefix}.md", "w") as mdfile:
            print(f"## {panel_title_human}\n\n<p>Data type benchmark results:</p>\n<ul>\n",
                  "\n".join([f"  <li>{anchorlink(run)}</li>" for run in bm_panel.runs]),
                  "</ul>\n\n", file=mdfile)
            for run in bm_panel.runs:
                tabletitle = f"{outputfile_prefix}"
                table = prettytable.PrettyTable(title=f"{panel_title_human}")
                table.add_column("function", [m.shorttitle for m in run.meta], align="l")
                for prop in ("mega_bytes_per_second", "cpu_time_ms"):
                    ref = getref(run)
                    bar_values = list(run.extract_plot_series(prop))
                    bar_values_rel = list(run.extract_plot_series(prop, relative_to_entry=ref))
                    bar_values_pc = list(run.extract_plot_series(prop, percent_of_entry=ref))
                    pd = bm_panel.first_run.property_plot_data(prop)
                    hns = pd.human_name_short
                    table.add_column(hns, [f"{v_:7.2f}" for v_ in bar_values], align="r")
                    hns = hns.replace(" (ms)", "")
                    table.add_column(f"{hns}(x)", [f"{v_:5.2f}x" for v_ in bar_values_rel], align="r")
                    table.add_column(f"{hns}(%)", [f"{v_:7.2f}%" for v_ in bar_values_pc], align="r")
                print(table, "\n\n")
                print(table, "\n\n", file=tablefile)
                pfx_bps = f"{os.path.basename(outputfile_prefix)}-mega_bytes_per_second"
                pfx_cpu = f"{os.path.basename(outputfile_prefix)}-cpu_time_ms"
                print(f"""
<br/>
<br/>

---

<a id="{anchor(run)}"/>

### {panel_title_human}

* Interactive html graphs
  * [MB/s](./{pfx_bps}.html)
  * [CPU time](./{pfx_cpu}.html)

[![{outputfile_prefix}: MB/s](./{pfx_bps}.png)](./{pfx_bps}.png)
[![{outputfile_prefix}: CPU time](./{pfx_cpu}.png)](./{pfx_cpu}.png)

```
{table}
```
""", file=mdfile)
    # make plots
    for prop in ("mega_bytes_per_second", "cpu_time_ms"):
        ps, ps_ = [], []
        pd = bm_panel.first_run.property_plot_data(prop)
        bar_label = f"{pd.human_name_short}{pd.qty_type.comment}"
        outfilename = f"{outputfile_prefix}-{prop}"
        for run in bm_panel.runs:
            bar_names = [m.shorttitle for m in run.meta]
            bar_values = list(run.extract_plot_series(prop))
            runtitle = f"{outfilename}"
            # to save each bokeh plot separately and also
            # a grid plot with all of them, we have to plot
            # twice because bokeh does not allow saving twice
            # the same plot from multiple pictures.
            plotit = lambda: bm.plot_benchmark_run_as_bars(run, title=f"{panel_title_human}\n{bar_label}",
                                                           bar_names=bar_names, bar_values=bar_values, bar_label=bar_label)
            # make one plot to save:
            p, p_ = plotit()
            bm._bokeh_save_html(f"{runtitle}.html", p)
            bm._plt_save_png(f"{runtitle}.png")
            bm._plt_clear()
            # and another to gather:
            p, p_ = plotit()
            ps.append(p)
            ps_.append(p_)
            bm._plt_clear()
        bm.bokeh_plot_many(ps, f"{outfilename}.html")




def plot_parse(dir_: str, filename, json_files):
    fcase = f"parse-{filename}"
    panel = bm.BenchmarkPanel(json_files, ParseMeta)
    ref = lambda bmrun: get_function_benchmark("yamlcpp", run=bmrun)
    plot_bm_bars(panel, ref,
                 f"parse benchmark: {filename}",
                 f"{dir_}/ryml-bm-{fcase}")


def plot_emit(dir_: str, filename, json_files):
    fcase = f"emit-{filename}"
    panel = bm.BenchmarkPanel(json_files, EmitMeta)
    ref = lambda bmrun: get_function_benchmark("yamlcpp", run=bmrun)
    plot_bm_bars(panel, ref,
                 f"emit benchmark: {filename}",
                 f"{dir_}/ryml-bm-{fcase}")


if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args) < 2:
        raise Exception(f"usage: {sys.executable} {sys.argv[0]} <filename> <jsonfile>")
    cmd = args[0]
    print(cmd)
    filename = args[1]
    print(filename)
    json_files = args[2:]
    print(json_files)
    dir_ = os.path.dirname(json_files[0])
    for jf in json_files:
        print("jf:", jf, flush=True)
        assert os.path.dirname(jf) == dir_, (os.path.dirname(jf), dir_)
        assert os.path.exists(jf), jf
    if cmd == "emit":
        plot_emit(dir_, filename, json_files)
    elif cmd == "parse":
        plot_parse(dir_, filename, json_files)
    else:
        raise Exception(f"not implemented: {cmd}")
