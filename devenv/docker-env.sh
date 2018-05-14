#!/usr/bin/env bash

######################################################################################
## Example args: --project-name giracoin --peers 3 --rpcauth '<rpcauth>' --mining true
## Example command (local wallet): ./giracoind-mining -connect=127.0.0.1:12017 &
## Node-to-bash mining pool env: #!/usr/bin/env $HOME/.nvm/v0.10.25/bin/node
######################################################################################

HERE=$(pwd)

##########################
## Command-line arguments
##########################
PEERS=4
MIINING=false
RPC_AUTH_STRING=""
NET_NAME="blockchain"
PROJECT_NAME="giracoin"

##################
## Read arguments
##################
while [ $# -gt 1 ]
  do
    key="$1"

    case $key in
      --project-name)
      PROJECT_NAME="$2"
      shift
      ;;

      --net-name)
      NET_NAME="$2"
      shift
      ;;

      --peers)
      PEERS="$2"
      shift
      ;;

      --rpcauth)
      RPC_AUTH_STRING="$2"
      shift
      ;;

      --mining)
      MIINING=true
      shift
      ;;

      --daemon)
      HOST_DAEMON="$2"
      shift
      ;;

      --cli)
      HOST_CLI="$2"
      shift
      ;;
      *)
      ;;
    esac
  shift
done

####################
## Build variables
####################
ROOT_DOCKER_DIR="$HOME/docker-env/$PROJECT_NAME"
ROOT_DATA_DIR="$ROOT_DOCKER_DIR/data"

DOCKER_IMAGE="novalabio/wallet-runner"
DOCKER_NET_NAME="$NET_NAME"
DOCKER_SUBNET="172.19.0.0/16"
DOCKER_GATEWAY="172.19.0.1"
PEER_IPV4_ADDRESS="172.19.0.10"
#WALLET_HOME="/home/runner"
WALLET_HOME="/root"
CONTAINER_DATA_DIR="$WALLET_HOME/.giracoin-testnet"
CONTAINER_DAEMON="$WALLET_HOME/giracoind"
CONTAINER_CLI="$WALLET_HOME/giracoin-cli"

######################
## Create default dirs
######################
if [ ! -d "$ROOT_DATA_DIR" ]; then
  mkdir -p $ROOT_DATA_DIR
  echo "Created root folder for persistent storage under $ROOT_DATA_DIR"
fi

for (( i = 0; i < $PEERS; i++ ))
  do
    if [ ! -d "$ROOT_DATA_DIR/peer$i" ]; then
      mkdir -p $ROOT_DATA_DIR/peer$i
      echo "Created persistent storage for peer $i"
    fi
  done

cd $ROOT_DOCKER_DIR

##############################
## Create wallet's conf file
##############################

echo "rpcauth=$RPC_AUTH_STRING" > giracoin.conf
echo "rpcallowip=$DOCKER_SUBNET" >> giracoin.conf
echo "" >> giracoin.conf
echo "listen=1" >> giracoin.conf
echo "server=1" >> giracoin.conf
echo "" >> giracoin.conf
for (( i = 0; i < $PEERS; i++ ))
  do
    echo "addnode=$PEER_IPV4_ADDRESS$i" >> giracoin.conf
  done

for (( i = 0; i < $PEERS; i++ ))
  do
    cp $ROOT_DOCKER_DIR/giracoin.conf $ROOT_DATA_DIR/peer$i
  done

##############################
## Create docker-compose file
##############################

echo "version: \"2\"" > docker-compose.yml
echo $'\n' >> docker-compose.yml
echo "services:" >> docker-compose.yml
echo $'\n' >> docker-compose.yml

for (( i = 0; i < $PEERS; i++ ))
  do
    CONNECT_IP=$((i - 1))
    echo "  peer$i:" >> docker-compose.yml
    echo "    image: $DOCKER_IMAGE" >> docker-compose.yml
    echo "    volumes:" >> docker-compose.yml
    echo "      - $ROOT_DATA_DIR/peer$i:$CONTAINER_DATA_DIR" >> docker-compose.yml
    echo "      - ${HOST_DAEMON}:$CONTAINER_DAEMON" >> docker-compose.yml
    echo "      - ${HOST_CLI}:$CONTAINER_CLI" >> docker-compose.yml
    echo "    networks:" >> docker-compose.yml
    echo "      $DOCKER_NET_NAME:" >> docker-compose.yml
    echo "        ipv4_address: $PEER_IPV4_ADDRESS$i" >> docker-compose.yml

    if [[ $i -eq 0 ]]; then
      if [ "$MIINING" = true ]; then
        echo "    ports:" >> docker-compose.yml
        echo "      - \"127.0.0.1:12017:12016\"" >> docker-compose.yml
      fi
      echo "    command: sh -c \"$CONTAINER_DAEMON -debug -listen=1\"" >> docker-compose.yml
    else
      echo "    command: sh -c \"$CONTAINER_DAEMON -debug -listen=1 -connect=$PEER_IPV4_ADDRESS$CONNECT_IP\"" >> docker-compose.yml
    fi

    echo $'\n' >> docker-compose.yml
  done

echo "networks:" >> docker-compose.yml
echo "  $DOCKER_NET_NAME:" >> docker-compose.yml
echo "    ipam:" >> docker-compose.yml
echo "      driver: default" >> docker-compose.yml
echo "      config:" >> docker-compose.yml
echo "        - subnet: $DOCKER_SUBNET" >> docker-compose.yml
echo "          gateway: $DOCKER_GATEWAY" >> docker-compose.yml

echo "docker-compose file successfully created"
exit 0
