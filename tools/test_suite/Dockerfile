FROM alpine:latest

# to run:
#    docker run --rm -it -v $PWD:/host alpine sh
#    docker run --rm -it -v $PWD:/host -w /host -v /tmp/bash_history:/root/.bash_history yts-importing bash
#

RUN apk add build-base \
 && apk add \
        bash \
        curl \
        fortune \
        git \
        jq \
        perl \
        perl-app-cpanminus \
        tig \
        vim \
        wget \
        python \
        cmake \
        ninja \
 && true

RUN cpanm -n \
        boolean \
        Capture::Tiny \
        XXX \
        YAML::PP \
 && true


ENV PYTHONPATH=/python/lib/python3.7/site-packages
