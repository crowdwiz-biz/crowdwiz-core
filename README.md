<div id="header" align="left">
  <img src="images/coingecko_logo.png" width="100px"/>
</div>

CrowdWiz Core
==============

CrowdWiz is an open source blockchain project based on Graphene/Bitshare high-speed DPOS technology, the basic functionality: cryptography, consensus and voting algorithms, API implementation and even basic operations remained the same as in the original Bitshares.

Getting Started
---------------

We recommend building on Ubuntu 18.04 LTS (64-bit) 

**Build Dependencies**:

    sudo apt-get update
    sudo apt-get install autoconf cmake make automake libtool git libboost-all-dev libssl-dev g++ libcurl4-openssl-dev

**Build Script:**

    git clone https://github.com/crowdwiz-biz/crowdwiz-core.git
    cd crowdwiz-core
    git checkout master # may substitute "master" with current release tag
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .
    make

**Build Script:**

    Minimal RAM (now) required for building full node is 16GB 
