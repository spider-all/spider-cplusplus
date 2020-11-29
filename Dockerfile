FROM alpine:edge

LABEL maintainer="Tosone <i@tosone.cn>"

WORKDIR /app

RUN sed -i "s/dl-cdn.alpinelinux.org/mirrors.aliyun.com/g" /etc/apk/repositories

RUN apk add --no-cache build-base ccache coreutils ca-certificates cmake tzdata \
  openssl-dev yaml-cpp-dev spdlog-dev nlohmann-json gnu-libiconv-dev hiredis-dev \
  leveldb-dev sqlite-dev && \
  rm -f /etc/localtime && ln -s /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

VOLUME /app

CMD /bin/sh
