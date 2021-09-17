ARG SPIDER_VERSION

FROM ghcr.io/spider-all/spider-cplusplus:base-${SPIDER_VERSION} as base

FROM buildpack-deps:stable as builder

WORKDIR /app

COPY . .
COPY --from=base /app/pkgs ./pkgs

RUN apt-get update -y && apt-get install -y --no-install-recommends jq ccache cmake libsasl2-dev && \
  rm -rf /var/lib/apt/lists/* && \
  make release

FROM debian:bullseye-slim

ENV TZ=Asia/Shanghai

RUN apt-get update -y && apt-get install -y --no-install-recommends libsasl2-2 ca-certificates && \
  rm -rf /var/lib/apt/lists/*

COPY etc/config.yaml.sample /etc/spider-cplusplus/config.yaml
COPY --from=builder /app/release/spider /usr/bin

CMD ["spider", "-c", "/etc/spider-cplusplus/config.yaml"]
