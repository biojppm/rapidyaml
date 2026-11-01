import sys
import os
import re
import yaml
try:
    from yaml import CLoader as Loader, CDumper as Dumper
except ImportError:
    from yaml import Loader, Dumper
from filelock import FileLock

thisdir = os.path.dirname(os.path.abspath(__file__))
moddir = os.path.abspath(f"{thisdir}/../proj/c4proj/bm-xp")
sys.path.insert(0, moddir)
import bm_plot as bm
from bm_util import first

from dataclasses import dataclass
import prettytable


def get_function_benchmark_(libname, run: bm.BenchmarkRun):
    for i, rbm in enumerate(run.entries):
        if rbm.meta.library == libname:
            return i, rbm
    raise Exception(f"lib not found: {libname}. Existing: {[rbm.meta.function for rbm in run.entries]}")
def get_function_benchmark(libname, run: bm.BenchmarkRun):
    return get_function_benchmark_(libname, run)[1]


_dbg = False
def dbg(*args, **kwargs):
    if _dbg:
        print(*args, **kwargs)


@dataclass
class ParseMeta:
    title: str
    library: str
    variant: str

    @classmethod
    def make(cls, bm_title: str):
        # eg:
        #-----------------------
        # PARSE
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


def add_cols(table, propdata, vals, vals_rel=None, vals_pc=None):
    hns = propdata.human_name_short
    table.add_column(hns, [f"{v_:7.2f}" for v_ in vals], align="r")
    hns = hns.replace(" (ms)", "")
    if vals_rel is not None:
        table.add_column(f"{hns}(x)", [f"{v_:5.2f}x" for v_ in vals_rel], align="r")
    if vals_pc is not None:
        table.add_column(f"{hns}(%)", [f"{v_:7.2f}%" for v_ in vals_pc], align="r")



def plot_bm_bars(bm_panel: bm.BenchmarkPanel,
                 getref,
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
                table = prettytable.PrettyTable(title=f"{panel_title_human}")
                table.add_column("function", [m.shorttitle for m in run.meta], align="l")
                for prop in ("mega_bytes_per_second", "cpu_time_ms"):
                    ref = getref(run)
                    bar_values = list(run.extract_plot_series(prop))
                    bar_values_rel = list(run.extract_plot_series(prop, relative_to_entry=ref))
                    bar_values_pc = list(run.extract_plot_series(prop, percent_of_entry=ref))
                    pd = bm_panel.first_run.property_plot_data(prop)
                    add_cols(table, pd, bar_values, bar_values_rel, bar_values_pc)
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


def plot_scatter_cmd(dir_: str, cmd: str):
    datafile = f"{dir_}/ryml-bm-{cmd}-scatter-_specs.yml"
    with open(datafile) as filein:
        data = yaml.load(filein, Loader=Loader)
    values = {}
    getref = lambda bmrun: get_function_benchmark_("yamlcpp", run=bmrun)
    values[cmd] = {}
    if cmd == "emit":
        meta = EmitMeta
        title = "Emit comparison"
        names = {
            'rapidyaml': 'ryml_yaml_str_reserve',
            'yamlcpp': 'yamlcpp'
        }
        if os.name != 'nt':
            names['fyaml'] = 'fyaml_str_reserve'
    elif cmd == "parse":
        meta = ParseMeta
        title = "Parse comparison"
        names = {
            'rapidyaml': 'ryml_yaml_inplace_reuse_reserve',
            'yamlcpp': 'yamlcpp_arena',
            'libyaml': 'libyaml_arena_reuse',
        }
        if os.name != 'nt':
            names['fyaml'] = 'libfyaml_arena'
    else:
        raise Exception("unknown command")
    for r in data:
        assert len(r['json']) == 1
    json_files = [r['json'][0] for r in data]
    panel = bm.BenchmarkPanel(json_files, meta)
    run_names = sorted(panel.all_entry_names)
    dbg(cmd, "\n  ".join(run_names))
    assert len(run_names) == len(set(run_names)), run_names
    for index, (specs, panel_run) in enumerate(zip(data, panel.runs)):
        for propname, propvals in (('mega_bytes_per_second', list(panel_run.mega_bytes_per_second)),
                                   ('cpu_time_ms', list(panel_run.cpu_time_ms))):
            ref_index = getref(panel_run)[0]
            ref = propvals[ref_index]
            for runname in run_names:
                pos = None
                for i, rn in enumerate(panel_run.run_names):
                    if rn == runname:
                        pos = i
                        break
                if pos is None:
                    continue
                if values[cmd].get(propname) is None:
                    values[cmd][propname] = {}
                if values[cmd][propname].get(runname) is None:
                    values[cmd][propname][runname] = {
                        'files': [], 'sizes': [], 'libs': [],
                        'vals': [], 'pc': [], 'rel': []
                    }
                if specs['name'] in values[cmd][propname][runname]['files']:
                    raise Exception("repeated file?")
                values[cmd][propname][runname]['files'].append(specs['name'])
                values[cmd][propname][runname]['sizes'].append(specs['size'])
                #values[cmd][propname][runname]['libs'].append(panel_run.benchmarks[]meta.library)
                values[cmd][propname][runname]['vals'].append(propvals[i])
                values[cmd][propname][runname]['pc'].append(100.0 * (propvals[i] - ref) / ref)
                values[cmd][propname][runname]['rel'].append(propvals[i] / ref)
    for propname, propresults in values[cmd].items():
        with open(f"{dir_}/ryml-bm-{cmd}-scatter-{propname}-data.yml", "w") as fileout:
            yaml.dump(values[cmd][propname], fileout, Dumper=Dumper)
    #import pprint
    #pprint.pp(values[cmd])
    varspecs = {
        'vals': {
            'mega_bytes_per_second': {'ylabel': 'MB/s', 'title': 'MB/s (more is better)'},
            'cpu_time_ms': {'ylabel': 'CPU time, ms', 'title': 'CPU time (less is better)', 'ylog': True},
        },
        'pc': {
            'mega_bytes_per_second': {'ylabel': '% of yamlcpp', 'title': 'MB/s\nas % of yamlcpp (more is better)'},
            'cpu_time_ms': {'ylabel': '% of yamlcpp', 'title': 'CPU time\nas % of yamlcpp (less is better)'},
        },
        'rel': {
            'mega_bytes_per_second': {'ylabel': '× yamlcpp', 'title': 'speed\nMB/s as multiple of yamlcpp (more is better)'},
            'cpu_time_ms': {'ylabel': '× yamlcpp', 'title': 'CPU time\nCPU time as multiple of yamlcpp (less is better)'},
        },
    }
    import matplotlib.pyplot as plt
    for propname, propresults in values[cmd].items():
        table = prettytable.PrettyTable(title=f"{cmd} {varspecs['vals'][propname]['title']}: all files")
        runname = names['rapidyaml']
        reffiles = values[cmd][propname]["bm_" + runname]['files']
        refsizes = values[cmd][propname]["bm_" + runname]['sizes']
        table.add_column("file", reffiles, align="l")
        table.add_column("size (KB)", [f"{sz / 1000:.1f}" for sz in refsizes], align="r")
        for var in ('vals', 'rel'):
            for libname, runname in names.items():
                result = values[cmd][propname]["bm_" + runname]
                files = result['files']
                sizes = result['sizes']
                if files != reffiles or sizes != refsizes:
                    raise Exception("files not matching")
                propdata = panel.first_run.property_plot_data(propname)
                propdata.human_name_short = f"{libname}(x)" if var == 'rel' else libname
                add_cols(table, propdata, result[var])
        tablename = f"{dir_}/ryml-bm-{cmd}-scatter-{propname}.txt"
        with open(tablename, "w") as tablefile:
            print(table)
            print(table, file=tablefile)
        for var in ('vals', 'rel'):
            fig, ax = plt.subplots()
            vspecs = varspecs[var][propname]
            for libname, runname in names.items():
                results = propresults["bm_" + runname]
                x = [r / 1000 for r in results['sizes']]
                y = results[var]
                ax.scatter(x, y, label=libname)
            ax.set(title=f"{title}: {vspecs['title']}")
            ax.set_xscale('log')
            if vspecs.get('ylog'):
                ax.set_yscale('log')
            ax.set_xlabel('file size, KB')
            ax.set_ylabel(vspecs['ylabel'])
            ax.legend()
            ax.grid(True)
            if _dbg:
                plt.show()
            plotname = f"{dir_}/ryml-bm-{cmd}-scatter-{propname}-{var}.png"
            print(plotname)
            plt.savefig(plotname, bbox_inches='tight', dpi=100)
            plt.clf()


def plot_scatter(dir_: str):
    plot_scatter_cmd(dir_, "emit")
    plot_scatter_cmd(dir_, "parse")


def add_to_scatter(dir_: str, cmd: str, filename: str, filepath: str, json_files):
    datafile = f"{dir_}/ryml-bm-{cmd}-scatter-_specs.yml"
    data = []
    if os.path.exists(datafile):
        with FileLock(f"{datafile}.lock") as lock:
            with open(datafile) as filein:
                data = yaml.load(filein, Loader=Loader)
        if data is None:
            data = []
    json_files = sorted(json_files)
    for entry in data:
        if entry['json'] == json_files:
            assert entry['name'] == filename
            assert entry['path'] == filepath
            return
    data.append({
        'name': filename,
        'path': filepath,
        'size': os.path.getsize(filepath),
        'json': json_files,
    })
    data.sort(key=lambda x: x['path'])
    with FileLock(f"{datafile}.lock") as lock:
        with open(datafile, "w") as fileout:
            yaml.dump(data, fileout, Dumper=Dumper)


if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args) < 1:
        raise Exception(f"usage: {sys.executable} ...")
    cmd = args[0]
    print("command:", cmd)
    if cmd == "scatter":
        if len(args) < 2:
            raise Exception(f"usage: {sys.executable} <resultdir>")
        dir_ = args[1]
        plot_scatter(dir_)
        exit()
    if len(args) < 3:
        raise Exception(f"usage: {sys.executable} {sys.argv[0]} <filename> <filepath> <jsonfile>")
    filename = args[1]
    filepath = args[2]
    json_files = args[3:]
    print("filename:", filename)
    print("filepath:", filepath)
    print("json_files:", json_files)
    dir_ = os.path.dirname(json_files[0])
    for jf in json_files:
        print("json_file:", jf, flush=True)
        assert os.path.dirname(jf) == dir_, (os.path.dirname(jf), dir_)
        assert os.path.exists(jf), jf
    add_to_scatter(dir_, cmd, filename, filepath, json_files)
    if cmd == "emit":
        plot_emit(dir_, filename, json_files)
    elif cmd == "parse":
        plot_parse(dir_, filename, json_files)
    else:
        raise Exception(f"not implemented: {cmd}")
