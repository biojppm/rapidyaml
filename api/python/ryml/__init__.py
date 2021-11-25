import json
from enum import IntFlag
from typing import Dict, List, Optional, Union
from warnings import warn

from . import ryml
from .ryml import *

# pylint:disable=unused-argument

NodeType = IntFlag("NodeType", {k: getattr(ryml, k) for k in ("NOTYPE", "VAL", "KEY", "MAP", "SEQ", "DOC", "STREAM", "KEYREF", "VALREF", "KEYANCH", "VALANCH", "KEYTAG", "VALTAG", "VALQUO", "KEYQUO",)})

ValueT = Optional[Union["MappingT", "SeqT", str]]
SeqT = List[ValueT]
MappingT = Dict[str, ValueT]
MappingTOpt = Optional[MappingT]


def map_mapper(tree: Tree, nodeId: int, nodeType: NodeType, container: MappingTOpt) -> MappingT:
    newCont = {}
    for n in children(tree, nodeId):
        _walk_tree(tree, n, newCont)
    return newCont


def seq_mapper(tree: Tree, nodeId: int, nodeType: NodeType, container: MappingTOpt) -> SeqT:
    newCont = []
    for n in children(tree, nodeId):
        newCont.append(_walk_tree(tree, n, None))
    return newCont


def val_mapper(tree: Tree, nodeId: int, nodeType: NodeType, container: MappingTOpt) -> Optional[str]:
    v = tree.val(nodeId)
    return process_value_item(v, bool(nodeType & NodeType.VALQUO))


def process_value_item(v, isQuote: bool):
    if v is None:
        return None
    v = v.tobytes().decode("utf-8")
    if not isQuote:
        if v == "":
            return None

        try:
            v = json.loads(v)
        except json.JSONDecodeError:
            pass
    return v


mappers = {
    NodeType.MAP: map_mapper,
    NodeType.SEQ: seq_mapper,
    NodeType.VAL: val_mapper,
    NodeType.DOC | NodeType.VAL: val_mapper,
    NodeType.NOTYPE: None,
}

DROP_MODIFIERS_MASK = ~(NodeType.KEYQUO | NodeType.VALQUO | NodeType.VALTAG)


def _walk_tree_item(tree: Tree, t: NodeType, nodeId: int, container: MappingTOpt) -> ValueT:
    m = mappers.get(t & DROP_MODIFIERS_MASK, None)
    if not m:
        raise NotImplementedError(nodeId, tree.val(nodeId), t)

    return m(tree, nodeId, t, container)


def _walk_tree(tree: Tree, nodeId: int, container: MappingTOpt) -> ValueT:
    t = NodeType(tree.type(nodeId))
    if t & NodeType.KEY:
        k = process_value_item(tree.key(nodeId), bool(t & NodeType.KEYQUO))
        v = _walk_tree_item(tree, t & ~NodeType.KEY & ~NodeType.KEYQUO, nodeId, container)
        container[k] = v
        return None

    return _walk_tree_item(tree, t, nodeId, None)


def to_python(tree: Tree) -> ValueT:
    return _walk_tree(tree, nodeId=tree.root_id(), container=None)


def parse_to_python(source: str) -> ValueT:
    warn("Currently our incomplete rudimentary impl of python convertor in `ryml` is much slower than `ruamel.yaml` `safe` parser (so it is recommended to use it) and is likely to be even slower when it'd be more feature complete. See https://github.com/biojppm/rapidyaml/pull/169#issuecomment-1129741892 for more info.")
    tree = parse_in_arena(source)
    return to_python(tree)
