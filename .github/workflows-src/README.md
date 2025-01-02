.github/workflows
=================

Source files for generating `.github/workflows/*.yml` files.


## Synopsis

```bash
$ make build
$ make test
```


## Description

Edit files in this directory and run `make build` to generate the files used by
GitHub Actions (the files in `.github/workflows/`).

Running `make test` runs `make build` and shows the diff in the
`.github/workflows/` directory.
If you are only refactoring the source files, you should see no diff when you
run this.
