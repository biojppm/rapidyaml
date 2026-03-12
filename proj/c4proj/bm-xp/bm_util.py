import os
import json
import yaml
import shutil
import mmh3
import itertools
import munch
import enum


# --------------------------------------------------------

class _enum(enum.Enum):
    def __str__(self):
        return str(self.name)

    @property
    def short(self):
        return self.name

    @classmethod
    def make(cls, s):
        try:
            return cls[s]
        except:
            cls.err_unknown(s)

    @classmethod
    def err_unknown(cls, s):
        raise Exception(f"unknown {__class__.__name__}: {s}")


class FundamentalTypes(_enum):
    float = "float"
    double = "double"
    int8_t = "int8_t"
    uint8_t = "uint8_t"
    int16_t = "int16_t"
    uint16_t = "uint16_t"
    int32_t = "int32_t"
    uint32_t = "uint32_t"
    int64_t = "int64_t"
    uint64_t = "uint64_t"
    @property
    def short(self):
        return self.name.replace("uint", "u").replace("int", "i").replace("_t", "")


# --------------------------------------------------------

def log(*args, **kwargs):
    print(*args, **kwargs, flush=True)


def myhash_combine(curr, value):
    return curr ^ (value + 0x9e3779b9 + (curr << 6) + (curr >> 2))


def first(iterable):
    """Returns the first item"""
    if isinstance(iterable, list):
        return iterable[0]
    return next(iterable)


def chain(*iterables):
    for it in iterables:
        for elm in it:
            yield elm


def nth(iterable, n, default=None):
    """Returns the nth item or a default value"""
    return next(itertools.islice(iterable, n, None), default)


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
    log("looking for file:", f)
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
    return munch.munchify(yaml.safe_load(yml))


def dump_json(data, filename):
    with open(filename, "w") as f:
        f.write(json.dumps(data, indent=2, sort_keys=True))


def load_json(filename):
    with open(filename, "r") as f:
        try:
            return munch.munchify(json.load(f))
        except Exception as exc:
            raise Exception(f"could not load file: {filename}: {exc}")
