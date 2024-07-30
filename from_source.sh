#!/bin/bash
set -e 
#--- for curl 
rm -r -f /tmp/curl/ \
 && cd /tmp/ \
 && git clone https://github.com/curl/curl.git \
 && cd /tmp/curl/ \
 && autoreconf -fi \
 && ./configure --with-gnutls \
 && make \
 && sudo make install

wget -O /tmp/lua-5.1.5.tar.gz "https://www.lua.org/ftp/lua-5.1.5.tar.gz" \
&& cd /tmp/  \
&& echo "2e115fe26e435e33b0d5c022e4490567 lua-5.1.5.tar.gz" | md5sum -c - \
&& mkdir -p /tmp/lua/ \
&& tar -zxvf /tmp/lua-5.1.5.tar.gz -C /tmp/lua/ --strip-components=1 \
&& cd /tmp/lua/ \
&& make linux \
&& sudo make install \
&& cd /tmp/ \
&& rm -rf /tmp/lua*gz /tmp/lua/

## libevent
wget -O /tmp/libevent.tar.gz "https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz" \ 
&&  cd /tmp/ \
&&  mkdir -p /tmp/libevent/ \
&&  tar -zxf /tmp/libevent.tar.gz -C /tmp/libevent/ --strip-components=1  \
&&  cd /tmp/libevent/ \
&&  ./configure  \
&&  make  \
&&  sudo make install  \
&& cd /tmp/ \
&&  rm -rf /tmp/libevent.tar.gz  \
&&  rm -rf /tmp/libevent/ 

## LuaJIT
wget -O /tmp/LuaJIT-2.1.0-beta3.tar.gz "https://luajit.org/download/LuaJIT-2.1.0-beta3.tar.gz" \
 &&   cd /tmp \
&& echo "1ad2e34b111c802f9d0cdf019e986909123237a28c746b21295b63c9e785d9c3 LuaJIT-2.1.0-beta3.tar.gz" | sha256sum -c - \
 && mkdir -p /tmp/LuaJIT \
 &&  tar -zxvf /tmp/LuaJIT-2.1.0-beta3.tar.gz -C /tmp/LuaJIT --strip-components=1  \
 &&  cd /tmp/LuaJIT/ \
 &&  make  \
 &&  sudo make install  \
 &&  sudo ln -sf luajit-2.1.0-beta3 /usr/local/bin/luajit \
 &&  rm -rf /tmp/LuaJIT*gz /tmp/LuaJIT 

