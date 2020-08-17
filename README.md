# dragonfly
A fast realtime communication and schedule framework

# compile
 - cd src/asio
 - ./autogen.sh
 - ./configure
 - make VERBOSE=1
 - sudo make install
 - cd dragonfly/build
 - cmake .. && make -j8



# test latency
 - local machine, sync send & async recv: 60.x - 80.x us
 - local machine, async send & async recv: 80.x - 200 us


