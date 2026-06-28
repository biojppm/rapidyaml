import os
import os.path as osp
import subprocess
import shutil
import filecmp
from typing import List
from dataclasses import dataclass
import yaml


@dataclass
class SyncGroup:
    origdir: str
    copydir: str
    filenames: List[str]


def filelist(yaml_scalar_or_seq) -> List[str]:
    if isinstance(yaml_scalar_or_seq, list):
        return yaml_scalar_or_seq
    else:
        with open(yaml_scalar_or_seq) as file:
            return [line.rstrip() for line in file]


def load_manifest(filename: str) -> List[SyncGroup]:
    with open(filename) as stream:
        groups = yaml.safe_load(stream)
    return [
        SyncGroup(g["origdir"], g["copydir"],
                  filelist(g["files"])) for g in groups
    ]


def unique(files: List[str]):
    return sorted(list(set(files)))


def group_check(g: SyncGroup):
    syncfiles = group_check_sync(g)
    listfiles = group_check_list(g)
    return unique(syncfiles + listfiles)


def group_check_list(g: SyncGroup):
    origfiles = check_list_(g.filenames, g.origdir)
    copyfiles = check_list_(g.filenames, g.copydir)
    return unique(origfiles + copyfiles)


def check_list_(filenames: List[str], dir: str):
    if not osp.isdir(dir):
        print(f"{dir}: directory not found")
        return False
    badfiles = []
    for filename in filenames:
        path = osp.join(dir, filename)
        if not osp.exists(path):
            print(f"{dir}: {filename} not found")
            badfiles.append(path)
        else:
            dbg(f"{dir}: {filename} ok")
    return badfiles



def group_check_sync(g: SyncGroup):
    badfiles = []
    for filename in g.filenames:
        if not check_sync_(filename, g.origdir, g.copydir):
            badfiles.append(f"{g.copydir}/{filename}")
    return badfiles


def check_sync_(filename: str, origdir: str, copydir: str):
    if not osp.isdir(origdir):
        print(f"{origdir}: directory not found")
        return False
    if not osp.isdir(copydir):
        print(f"{copydir}: directory not found")
        return False
    origpath = osp.join(origdir, filename)
    copypath = osp.join(copydir, filename)
    ok = True
    if not osp.isfile(origpath):
        ok = False
        print(f"{origdir}: {filename} not found")
    if not osp.isfile(copypath):
        ok = False
        print(f"{copydir}: {filename} not found")
    if ok:
        result = runcmd("colordiff", "-u", origpath, copypath,
                        stderr=subprocess.STDOUT)
        if result.returncode != 0:
            ok = False
            print(f"{origdir} vs {copydir}: {filename} differs")
        else:
            dbg(f"{origdir} vs {copydir}: {filename} ok")
    return ok


def group_import(g: SyncGroup):
    return group_copy(g, g.origdir, g.copydir)


def group_export(g: SyncGroup):
    return group_copy(g, g.copydir, g.origdir)


def group_copy(g: SyncGroup, srcdir: str, dstdir: str):
    badfiles = []
    for filename in g.filenames:
        if not copy(filename, srcdir, dstdir):
            badfiles.append(f"{dstdir}/{filename}")
    return badfiles


def copy(filename: str, srcdir: str, dstdir: str):
    ok = True
    srcfile = osp.join(srcdir, filename)
    dstfile = osp.join(dstdir, filename)
    if osp.isfile(dstfile):
        if git_knows(dstfile):
            if not git_isclean(dstfile):
                ok = False
                print(f"{dstfile}: err, dirty, refusing to overwrite")
        else:
            ok = False
            print(f"{dstfile}: err, not git-controlled, refusing to overwrite")
    else:
        ok = False
        print(f"{dstfile}: err, not a file")
    # https://stackoverflow.com/questions/36821178/how-to-shutil-copyfile-only-if-file-differ
    if ok and (not os.path.exists(dstfile) or not filecmp.cmp(srcfile, dstfile)):
        print(f"{{{dstdir} <- {srcdir}}}/{filename}")
        dbg(f"{dstfile}   <--   {srcfile}/")
        shutil.copy(srcfile, dstfile)
    return ok


def git_knows(filename: str):
    # https://stackoverflow.com/questions/2405305/how-to-tell-if-a-file-is-git-tracked-by-shell-exit-code
    repodir = os.path.dirname(filename)
    filename = os.path.basename(filename)
    ret = runcmd("git", "ls-files", "--error-unmatch", filename,
                 stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                 cwd=repodir)
    return ret.returncode == 0


def git_isclean(filename: str):
    repodir = os.path.dirname(filename)
    filename = os.path.basename(filename)
    ret = runcmd("git", "diff", "--exit-code", "--color=always", filename,
                 cwd=repodir)
    return ret.returncode == 0


def runcmd(*args, **kwargs):
    args = list(args)
    cmd = " ".join(args)
    dbg(f"+ {cmd}")
    result = subprocess.run(args, **kwargs)
    dbg(f"+ => {result.returncode}")
    return result


verbose = False
def dbg(*args, **kwargs):
    if verbose:
        print(*args, **kwargs)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    actions = {
        "check-list": group_check_list,
        "check-sync": group_check_sync,
        "check": group_check,
        "import": group_import,
        "export": group_export,
    }
    this_dir = osp.dirname(osp.realpath(__file__))
    root_dir = osp.dirname(this_dir)
    parser.add_argument("-m", "--manifest", required=True)
    parser.add_argument("-r", "--root", default=root_dir)
    parser.add_argument("-v", "--verbose", help="be verbose", action="store_true")
    parser.add_argument("command")
    args = parser.parse_args()
    verbose = args.verbose
    dbg(args)
    # the manifest has all paths relative to the root dir.
    # so just change the cwd to the root dir
    if not osp.isdir(args.root):
        raise Exception(f"{args.root}: root dir is not a dir")
    os.chdir(args.root)
    groups = load_manifest(args.manifest)
    func = actions[args.command]
    badfiles = []
    for group in groups:
        dbg(group)
        badfiles += func(group)
    badfiles = unique(badfiles)
    if badfiles:
        print(f"ERROR: {args.command}: failed these files:\n{'\n'.join(badfiles)}")
    exit(1 if badfiles else 0)
