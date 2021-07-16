apt update &&\
apt-get update &&\
echo ~~~~~~~~~~~~~~~~~~~~~~~~ mosquitto~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~&&\
apt-get install -y mosquitto &&\
apt-get install -y  mosquitto-clients &&\
apt clean &&\
echo ~~~~~~~~~~~~~~~~~~~~~~~~ mosquitto installed~~~~ Nxt Software properties~~~~~~~~~~~~~~~~~~~~~~~~~~&&\
apt update &&\
apt-get  install -y  software-properties-common &&\
apt update &&\
echo ~~~~~~~~~~~~~~~~~~~~~~~~ mosquitto installed~~~~ Nxt Software properties~~~~~~~~~~~~~~~~~~~~~~~~~~&&\
apt install -y python3.8 &&\
apt install -y python3-pip &&\
echo ~~~~~~~~~~~~~~~~~~~~~~~~ python installed~~~~~~~~~~~~~~~~~~~~~~~~~~~&&\
apt-get install -y libsqlite3-dev &&\
pip3 install paho-mqtt &&\
apt update &&\
echo ~~~~~~~~~~~~~~~~~~~~~~~~paho mqtt installed~~~~~~~~~~~~~~~~~~~~~~~~&&\
apt install -y build-essential &&\
apt-get install -y manpages-dev &&\
echo ~~~~~~~~~~~~~~~~~~~~~~~~manpages installed~~~~~~~~~~~ nxt git from sh~~~~~~~~~~~~~&&\
git clone https://github.com/azadkuh/sqlite-amalgamation &&\
git clone https://github.com/eclipse/paho.mqtt.c.git &&\
apt-get install -y libssl-dev&&\
cd ./paho.mqtt.c &&\
make &&\
make install &&\
apt install -y sqlite &&\
apt install -y  supervisor &&\
apt clean
