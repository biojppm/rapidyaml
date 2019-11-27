Name:           rapidyaml
Version:        0
Release:        0
Summary:        A library to parse and emit YAML, and do it fast.
License:        MIT
URL:            https://github.com/biojppm/rapidyaml
Prefix:         %{_prefix}
Packager: 	    Carsten Burgard
BuildRoot:      %{_tmppath}/%{name}-root

%description
ryml parses both read-only and in-situ source buffers; the resulting
data nodes hold only views to sub-ranges of the source buffer. No
string copies or duplications are done, and no virtual functions are
used. The data tree is a flat index-based structure stored in a single
array. Serialization happens only at your request, after parsing /
before emitting. Internally the data tree representation has no
knowledge of types (but of course, every node can have a YAML type
tag). It is easy and fast to read, write and iterate through the data
tree.

ryml can use custom per-tree memory allocators, and is
exception-agnostic. Errors are reported via a custom error handler
callback (and a default implementation using std::abort is provided,
but you can opt out).

ryml has respect for your compilation times and therefore it is NOT
header-only. (Work is still needed on installing it, but you can just
add it as a subdirectory of your CMake project).

ryml has no dependencies, not even on the STL (although it does use
the libc). But it provides optional headers that let you
serialize/deserialize STL strings and containers (or show you how to
do it).
%prep
git clone --recursive https://github.com/cburgard/rapidyaml
cd rapidyaml
git checkout makepkg
cd extern/c4core
git remote add fork https://github.com/cburgard/c4core
git fetch fork
git checkout -b fork fork/master
cd cmake
git checkout master
%build
mkdir build
cd build
cmake ../rapidyaml -DRYML_LIBRARY_TYPE=SHARED -DC4_LIBRARY_TYPE=SHARED -DCMAKE_INSTALL_PREFIX=/usr 
make
%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
cd build
make DESTDIR=$RPM_BUILD_ROOT install
%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
%files
/usr/share/ryml
/usr/lib/ryml
/usr/include/ryml

