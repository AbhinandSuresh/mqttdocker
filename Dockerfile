FROM ubuntu:latest

WORKDIR /mqttdocker

COPY scriptformqttdocker.sh /mqttdocker

RUN chmod 777 scriptformqttdocker.sh &&\
./scriptformqttdocker.sh

COPY . /mqttdocker

CMD ["supervisord","-n","-c","supervisord.conf"]

