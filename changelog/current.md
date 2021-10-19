
### New features
- Add support for CPU architectures aarch64, ppc64le, s390x
- Update c4core to [0.1.5](https://github.com/biojppm/c4core/releases/tag/v0.1.5)
- Add sample showing how to load a file and parse with ryml

### Fixes

- Fix parsing of tags followed by comments in sequences:
```yaml
# test case 735Y
- !!map # Block collection
  foo : bar
```
