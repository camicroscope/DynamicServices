FROM sbubmi/test_segmentation:latest

RUN yum makecache fast 

RUN yum -y install curl-devel jsoncpp-devel libev-devel && \
    ln -s /usr/include/jsoncpp/json /usr/include/json

WORKDIR /tmp/

RUN git clone https://github.com/redis/hiredis.git && \
    cd hiredis && make -j4 && make install 

WORKDIR /tmp/

RUN git clone https://github.com/hmartiro/redox.git && \
    cd redox && \
    mkdir build && cd build && \
    cmake ../ && \
    make -j4 && make install 

WORKDIR /tmp/

RUN git clone https://github.com/camicroscope/DynamicServices.git && \
    cd DynamicServices/ && \
    mkdir images && \
    mkdir configs && \ 
    mkdir obj && make -j4 

WORKDIR /tmp/

ENV LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib64:$LD_LIBRARY_PATH


