FROM ubuntu:bionic

ARG VERSION=2019.12
ARG DEPS="openssl zlib yaml-cpp spdlog nlohmann-json rocksdb[zstd] sqlitecpp cpr boost-beast mongo-cxx-driver"

WORKDIR /app

RUN apt-get update -y && apt-get install -y --no-install-recommends \
  ca-certificates file git curl tar wget curl unzip fish locales build-essential \
  tree vim bash-completion apt-utils man-db cmake && \
  rm -rf /var/lib/apt/lists/*

RUN echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen && \
  echo "zh_CN.UTF-8 UTF-8" >> /etc/locale.gen && locale-gen

RUN sed -i "s/bin\/bash/usr\/bin\/fish/" /etc/passwd

RUN rm -f /etc/localtime && ln -s /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

ADD https://github.com/microsoft/vcpkg/archive/${VERSION}.tar.gz .

VOLUME /app/spider

RUN tar zxvf ${VERSION}.tar.gz && cd vcpkg-${VERSION} && ./bootstrap-vcpkg.sh && \
  ./vcpkg install ${DEPS} && \
  ./vcpkg list && ./vcpkg export ${DEPS} --raw --output=pkgs && \
  mv pkgs .. && cd .. && \
  rm -rf vcpkg-${VERSION} ${VERSION}.tar.gz pkgs.zip

CMD /usr/bin/fish
