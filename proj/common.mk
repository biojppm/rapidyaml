SHELL := bash

RYML_DIR = $(shell cd $(dir $(lastword $(MAKEFILE_LIST)))/.. ; pwd)

GIT := git
CMAKE := cmake
COLORDIFF := colordiff


# function to copy files using tar
# $1=src_dir
# $2=src_files
# $3=dst_dir
mk_tarcopy = \
	cd $1 ; tar cf - $2 | ( cd $3 ; tar xvfp - )


# function to sync files safely
# $1=src_files (relative paths to both src and dst dirs)
# $2=src_dir
# $3=dst_dir
mk_safe_sync = \
	set -xe -o pipefail ; \
	( cd $3 ; $(GIT) diff --exit-code --color=auto $1 || ( echo "$$(pwd): destination files are dirty" ; exit 1 ) ) ; \
	for f in $1 ; do \
	   $(COLORDIFF) -u $2/$$f $3/$$f || ( chmod u+w $3/$$f ; cp -fav $2/$$f $3/$$f ; chmod a-w $3/$$f ) ; \
	done


# function to clone repo on a certain tag
# $1=dst_dir
# $2=tag
# $3=repo_url
mk_git_clone = \
	echo "clone $(shell basename $1) from repo: $3" ; \
	echo "clone $(shell basename $1) from tag: $2" ; \
	echo "clone $(shell basename $1) to dir: $1" ; \
	\
	set -xeo pipefail ; \
	\
	if [ -d $1 ] ; then \
	   echo "$(pwd): $1 already exists" ; \
	   exit 1 ; \
	fi ; \
	\
	mkdir -p $1.tmp ; \
	git -C $1.tmp init -q . ; \
	git -C $1.tmp remote add origin $3 ; \
	\
	git -C $1.tmp fetch --tags origin $2 ; \
	git -C $1.tmp reset --hard FETCH_HEAD ; \
	git -C $1.tmp submodule update --init --recursive ; \
	\
	git -C $1.tmp tag --points-at HEAD || echo "(no tag)" ; \
	git -C $1.tmp rev-parse HEAD ; \
	git -C $1.tmp rev-parse --short HEAD ; \
	\
	mv -v $1.tmp $1
