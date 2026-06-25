# External source files

All the source files under [the ext folder](https://github.com/biojppm/rapidyaml/tree/master/ext) are from [c4core](https://github.com/biojppm/c4core), an external project. Because some users prefer to avoid submodules, these source files are explicitly copied into rapidyaml. This README explains how to handle the synchronization scenarios.


## Contents of ext folder

 - Files to help manage workflow, by order of importance:
   - `c4core.mk`: specifies the c4core repo + commit to sync with. This is equivalent to the .gitmodules file. Needs to be set when upgrading the c4core version.  Take note that it must be either a git tag like `v0.4.0` or **a full SHA1 hash** of a git commit like `2d123704c0594f818162324ef7805a8cc6f5895d`; cannot be a short hash or a branch name.
   - `Makefile`: provides targets to sync files and check sync status. (If you work in Windows, and a make executable is not available, you can [install one on the git bash bin directory](https://gist.github.com/evanwill/0207876c3243bbb6863e65ec5dc3f058), for example.)
   - `sync.py`: helper script used by the makefile
 - Folders with c4core source files
   - `c4core.src`: files used by the rapidyaml library
   - `c4core.dev`: files used only by internal development targets
   - `../proj/c4proj` and `../proj/toolchains` also contain c4core project files. See the full manifest for the single source of truth regarding this.
 - Manifests used by the makefile/sync tool and c4core.cmake helper:
   - `c4core.src.txt`: manifest of c4core source files used by the rapidyaml library
   - `c4core.dev.txt`: manifest of c4core source files used only by internal development targets
   - `c4core.yml`: full manifest of c4core files to keep in sync between c4core and rapidyaml
 - CMake helper files
   - `c4core.cmake`: cmake functions to inject source files into cmake targets, as specified in the src or dev manifests
   - `testbm.cmake`: unrelated to c4core; recipes for building external targets used in internal development targets

rapidyaml's CI ensures that the local copies in rapidyaml are in sync with the c4core version specified by `c4core.mk`.


## Check sync status

The makefile provides the following targets to check sync aspects:
```
make help                 # get makefile help
make c4core-check-clone   # check clone version against c4core.mk
make c4core-check-sync    # check sync status of original and copied files
make c4core-check-list    # check manifest list vs copied files

# to debug problems:
make c4core-check-clone SYNC_OPTS=--verbose
make c4core-check-sync SYNC_OPTS=--verbose
make c4core-check-list SYNC_OPTS=--verbose
```


## Upgrading the c4core version

  1. Edit `c4core.mk` to point at the intended c4core version.
  2. If the new version adds files, remember to update the appropriate manifest.
  3. Import the c4core files, overwriting the local copies (creating a c4core clone if necessary):
     ```
     make c4core-import
     # to debug problems:
     make c4core-import SYNC_OPTS=--verbose
     ```


## Modifying files locally

  1. Edit the local copies as needed (ie under `c4core.src`).
  2. If adding or removing files, remember to update the appropriate manifest.
  3. When done, export to the local c4core clone (creating a c4core clone if necessary):
     ```
     make c4core-export
     # to debug problems:
     make c4core-import SYNC_OPTS=--verbose
     ```
  4. Commit the changes in the c4core clone.
  5. Edit `c4core.mk` to point at the intended c4core version.


## Direct use of sync.py

The `Makefile` is simply a wrapper over `sync.py`. You can use this tool directly; see the commands issued by the makefile for usage examples, or run `sync.py --help`.
