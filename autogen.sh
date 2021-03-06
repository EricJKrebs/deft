set -ev

# automake wants a file called README.
cp README.md README

aclocal
autoheader
automake --add-missing || true #workaround for buggy old automake
autoconf

#CXXFLAGS='-ansi -pipe -W -g -Wall -O2 -Werror' ./configure
#CXXFLAGS='-ansi -pipe -W -Wall -O2 -Werror' ./configure
if env | grep CCACHE_; then
    echo Using ccache to speed up compilation.
    CXX='ccache g++' CXXFLAGS='-ansi -pipe -W -Wall -O1 -Werror' ./configure
else
    echo 'Consider installing ccache to speed up compilation!'
    CXXFLAGS='-ansi -pipe -W -Wall -O1 -Werror' ./configure
fi

set +v
echo If you are not a deft developer, you should now run ./configure
echo to avoid overly-pedantic compiler errors!
