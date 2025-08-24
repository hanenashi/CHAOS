#!/bin/sh
# rerun the configure generation steps
autoheader
aclocal
autoconf
automake --add-missing
#./configure
#make

