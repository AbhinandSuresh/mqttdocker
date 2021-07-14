apt update &&\
apt-get update &&\
apt-get install -y mosquitto &&\
apt-get install -y  mosquitto-clients &&\
apt clean &&\
apt update &&\
apt install -y software-properties-common &&\
apt update &&\
apt install -y python3.8 &&\
apt install -y python3-pip &&\
apt-get install -y libsqlite3-dev &&\
pip3 install paho-mqtt &&\
apt update &&\
apt install -y build-essential &&\
apt-get install -y manpages-dev &&\
apt install -y git &&\
git clone https://github.com/azadkuh/sqlite-amalgamation &&\
git clone https://github.com/eclipse/paho.mqtt.c.git &&\
apt-get install -y libssl-dev&&\
cd ./paho.mqtt.c &&\
make &&\
make install &&\
apt install -y sqlite &&\
apt install -y  supervisor &&\
apt clean
