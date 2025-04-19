SHELL := bash
RYML_DIR = $(shell cd .. ; pwd)
GIT := git
COLORDIFF := colordiff

# function to copy files using tar
# $1=src_dir
# $2=src_files
# $3=dst_dir
tarcopy = \
	cd $1 ; tar cf - $2 | ( cd $3 ; tar xvfp - )


# function to sync files safely
# $1=src_files (relative paths to both src and dst dirs)
# $2=src_dir
# $3=dst_dir
safe_sync = \
	set -xe -o pipefail ; \
	( cd $3 ; $(GIT) diff --exit-code --color=auto $1 || ( echo "$$(pwd): destination files are dirty" ; exit 1 ) ) ; \
	for f in $1 ; do \
	   $(COLORDIFF) -u $2/$$f $3/$$f || ( cp -fav $2/$$f $3/$$f ) ; \
	done
