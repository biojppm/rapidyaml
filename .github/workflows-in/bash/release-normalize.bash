sdist_orig=`find dist -type f -name 'rapidyaml-*.zip'`
[ ! -f $sdist_orig ] && exit 1
sdist=`echo $sdist_orig | sed 's:\.zip:-python_src.zip:'`
mv -fv $sdist_orig $sdist
