`.github/workflows/*.ys`
========================

Use YAMLScript for GHA workflows


## Synopsis

```bash
$ make build
$ make test
$ make diff
```


## Description

Edit the `*.ys` files in this directory and run `make build` to generate the
files used by GitHub Actions (`.*.yml`).

Running `make test` runs `make build` and shows the diff in the of the workflow
`.foo.yml` files.

If you are only refactoring the `*.ys` source files, you should see no diff
when you run this.
