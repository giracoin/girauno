HERE=$(pwd)

make clean && ./autogen.sh \
&& ./configure LDFLAGS="-L${BDB_PREFIX}/lib/" CPPFLAGS="-I${BDB_PREFIX}/include/" \
--without-gui --enable-hardening --without-miniupnpc --with-incompatible-bdb --enable-debug \
--disable-tests --disable-gui-tests --disable-bench \
&& make -j 4 #&& sudo make install
