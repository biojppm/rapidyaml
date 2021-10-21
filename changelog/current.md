
### New features
- Add support for CPU architectures aarch64, ppc64le, s390x.
- Fix compilation with `C4CORE_NO_FAST_FLOAT` ([PR #163](https://github.com/biojppm/rapidyaml/pulls/163))
- Update c4core to [0.1.6](https://github.com/biojppm/c4core/releases/tag/v0.1.6)
- Add sample showing how to load a file and parse with ryml.

### Fixes

#### Tags

- Fix parsing of tags followed by comments in sequences:
```yaml
# test case 735Y
- !!map # Block collection
  foo : bar
```

#### Plain scalars
- Fix parsing whitespace within plain scalars:
```yaml
# test case NB6Z
key:
  value
  with
   	
  tabs
  tabs
   	
    foo
   	
      bar
        baz
   	
# now correctly parsed as "value with\ntabs tabs\nfoo\nbar baz"
```

### Thanks

--- @mbs-c
