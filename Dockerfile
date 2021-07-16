FROM ubuntu:latest

RUN apt update

RUN apt install -y  git

WORKDIR /MyDoc

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -y install tzdata

RUN git clone https://github.com/AbhinandSuresh/mqttdocker.git

WORKDIR /MyDoc/mqttdocker

RUN chmod 777 scriptformqttdocker.sh &&\
./scriptformqttdocker.sh

RUN cp supervisord.conf /etc/supervisord.conf

CMD ["supervisord","-n"]
