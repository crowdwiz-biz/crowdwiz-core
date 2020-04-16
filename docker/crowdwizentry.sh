#!/bin/bash
CROWDWIZD="/usr/local/bin/witness_node"

# For blockchain download
VERSION=`cat /etc/crowdwiz/version`

## Supported Environmental Variables
#
#   * $CROWDWIZD_SEED_NODES
#   * $CROWDWIZD_RPC_ENDPOINT
#   * $CROWDWIZD_PLUGINS
#   * $CROWDWIZD_REPLAY
#   * $CROWDWIZD_RESYNC
#   * $CROWDWIZD_P2P_ENDPOINT
#   * $CROWDWIZD_WITNESS_ID
#   * $CROWDWIZD_PRIVATE_KEY
#   * $CROWDWIZD_TRACK_ACCOUNTS
#   * $CROWDWIZD_PARTIAL_OPERATIONS
#   * $CROWDWIZD_MAX_OPS_PER_ACCOUNT
#   * $CROWDWIZD_ES_NODE_URL
#   * $CROWDWIZD_ES_START_AFTER_BLOCK
#   * $CROWDWIZD_TRUSTED_NODE
#

ARGS=""
# Translate environmental variables
if [[ ! -z "$CROWDWIZD_SEED_NODES" ]]; then
    for NODE in $CROWDWIZD_SEED_NODES ; do
        ARGS+=" --seed-node=$NODE"
    done
fi
if [[ ! -z "$CROWDWIZD_RPC_ENDPOINT" ]]; then
    ARGS+=" --rpc-endpoint=${CROWDWIZD_RPC_ENDPOINT}"
fi

if [[ ! -z "$CROWDWIZD_REPLAY" ]]; then
    ARGS+=" --replay-blockchain"
fi

if [[ ! -z "$CROWDWIZD_RESYNC" ]]; then
    ARGS+=" --resync-blockchain"
fi

if [[ ! -z "$CROWDWIZD_P2P_ENDPOINT" ]]; then
    ARGS+=" --p2p-endpoint=${CROWDWIZD_P2P_ENDPOINT}"
fi

if [[ ! -z "$CROWDWIZD_WITNESS_ID" ]]; then
    ARGS+=" --witness-id=$CROWDWIZD_WITNESS_ID"
fi

if [[ ! -z "$CROWDWIZD_PRIVATE_KEY" ]]; then
    ARGS+=" --private-key=$CROWDWIZD_PRIVATE_KEY"
fi

if [[ ! -z "$CROWDWIZD_TRACK_ACCOUNTS" ]]; then
    for ACCOUNT in $CROWDWIZD_TRACK_ACCOUNTS ; do
        ARGS+=" --track-account=$ACCOUNT"
    done
fi

if [[ ! -z "$CROWDWIZD_PARTIAL_OPERATIONS" ]]; then
    ARGS+=" --partial-operations=${CROWDWIZD_PARTIAL_OPERATIONS}"
fi

if [[ ! -z "$CROWDWIZD_MAX_OPS_PER_ACCOUNT" ]]; then
    ARGS+=" --max-ops-per-account=${CROWDWIZD_MAX_OPS_PER_ACCOUNT}"
fi

if [[ ! -z "$CROWDWIZD_ES_NODE_URL" ]]; then
    ARGS+=" --elasticsearch-node-url=${CROWDWIZD_ES_NODE_URL}"
fi

if [[ ! -z "$CROWDWIZD_ES_START_AFTER_BLOCK" ]]; then
    ARGS+=" --elasticsearch-start-es-after-block=${CROWDWIZD_ES_START_AFTER_BLOCK}"
fi

if [[ ! -z "$CROWDWIZD_TRUSTED_NODE" ]]; then
    ARGS+=" --trusted-node=${CROWDWIZD_TRUSTED_NODE}"
fi

## Link the bitshares config file into home
## This link has been created in Dockerfile, already
ln -f -s /etc/bitshares/config.ini /var/lib/bitshares

# Plugins need to be provided in a space-separated list, which
# makes it necessary to write it like this
if [[ ! -z "$CROWDWIZD_PLUGINS" ]]; then
   exec $CROWDWIZD --data-dir ${HOME} ${ARGS} ${CROWDWIZD_ARGS} --plugins "${CROWDWIZD_PLUGINS}"
else
   exec $CROWDWIZD --data-dir ${HOME} ${ARGS} ${CROWDWIZD_ARGS}
fi
