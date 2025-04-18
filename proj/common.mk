SHELL := bash
RYML_DIR = $(shell cd .. ; pwd)

COLORDIFF := colordiff

# function to copy files using tar
# $1=src_dir
# $2=src_files
# $3=dst_dir
tarcopy = \
	cd $1 ; tar cf - $2 | ( cd $3 ; tar xvfp - )
