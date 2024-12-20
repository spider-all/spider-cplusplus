FROM ghcr.io/spider-all/spider-cplusplus:base-20241028 AS base

FROM docker.io/library/buildpack-deps:bookworm

WORKDIR /app

COPY --from=base /pkgs /pkgs

RUN set -eux && sed -i "s/deb.debian.org/mirrors.aliyun.com/g" /etc/apt/sources.list.d/debian.sources

RUN apt-get update -y && apt-get install -y --no-install-recommends sudo ccache cmake libsasl2-dev jq && \
  rm -rf /var/lib/apt/lists/* && \
  wget -O /usr/bin/vcpkg https://github.com/microsoft/vcpkg-tool/releases/download/2023-03-14/vcpkg-muslc && \
  chmod +x /usr/bin/vcpkg

RUN sh -c "$(wget -O- https://github.com/deluan/zsh-in-docker/releases/download/v1.2.1/zsh-in-docker.sh)" -- \
  -t robbyrussell -p git -p 'history-substring-search' \
  -a 'bindkey "\$terminfo[kcuu1]" history-substring-search-up' \
  -a 'bindkey "\$terminfo[kcud1]" history-substring-search-down' \
  -p https://github.com/zsh-users/zsh-autosuggestions \
  -p https://github.com/zsh-users/zsh-completions

ENV SHELL=/bin/zsh
