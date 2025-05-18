FROM ubuntu:latest
MAINTAINER Mradul Natani "mradulnatani0@gmail.com"
RUN apt-get update && apt-get install -y build-essential git
RUN git clone https://github.com/mradulnatani/cacheieee.git
WORKDIR cacheieee
RUN make
CMD ./cacheieee
