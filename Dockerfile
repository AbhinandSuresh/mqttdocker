FROM ubuntu:latest
RUN apt update
RUN apt install -y mosquitto
CMD ["mosquitto","-p","2021"]

