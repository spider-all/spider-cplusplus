FROM alpine:edge

LABEL maintainer="Tosone <i@tosone.cn>"

WORKDIR /app

# RUN sed -i "s/https:\/\/dl-cdn.alpinelinux.org/https:\/\/mirrors.aliyun.com/g" /etc/apk/repositories && \
#   echo "https://mirrors.aliyun.com/alpine/edge/testing" >> /etc/apk/repositories

RUN apk add --no-cache build-base coreutils ca-certificates cmake tree vim git fish tzdata \
  openssl-dev yaml-cpp-dev spdlog-dev nlohmann-json gnu-libiconv-dev hiredis-dev boost-dev leveldb-dev && \
  sed -i "s/bin\/ash/usr\/bin\/fish/" /etc/passwd && \
  rm -f /etc/localtime && ln -s /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

ENV SHELL /usr/bin/fish

VOLUME /app/spider

CMD /usr/bin/fish
