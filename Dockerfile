FROM ubuntu:latest

RUN apt-get update
RUN apt-get -y install python3 python3-pip wget

WORKDIR "/home"
ENV PATH="/home:${PATH}"
ENV FBX_SDK_ROOT=/home/fbx-sdk-linux
ENV LD_LIBRARY_PATH=$FBX_SDK_ROOT/lib/gcc4/x64/release

RUN wget -O premake5.tar.gz https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-linux.tar.gz \
      && tar -xf premake5.tar.gz
RUN wget -O fbx20190_fbxsdk_linux.tar.gz http://download.autodesk.com/us/fbx/2019/2019.0/fbx20190_fbxsdk_linux.tar.gz \
      && tar -xf fbx20190_fbxsdk_linux.tar.gz \
      && chmod +x fbx20190_fbxsdk_linux \
      && mkdir fbx-sdk-linux \
      && yes yes | ./fbx20190_fbxsdk_linux fbx-sdk-linux

COPY . /home
RUN ./generate_makefile \
       && cd build/gmake/ \
       && make config=release

FROM ubuntu:latest
COPY --from=0 /home/build/gmake/bin/Release/fbx-conv /usr/bin/fbx-conv
COPY --from=0 /home/fbx-sdk-linux/lib/gcc4/x64/release/* /usr/lib/
WORKDIR "/home"
ENTRYPOINT ["/usr/bin/fbx-conv"]
