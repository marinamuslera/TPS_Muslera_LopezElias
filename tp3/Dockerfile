FROM ubuntu:22.04
RUN apt update -y && apt upgrade -y
RUN apt install g++ valgrind make -y
WORKDIR /tp
CMD make valgrind
