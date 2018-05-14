make clean && \
./autogen.sh && \
./configure CPPFLAGS="-DBOOST_SYSTEM_NO_DEPRECATED" --prefix=`pwd`/depends/i686-w64-mingw32 \ 
--enable-hardening --without-miniupnpc --disable-tests --disable-gui-tests --disable-bench --enable-staking --disable-mining && \
make -j 4

exit 0
