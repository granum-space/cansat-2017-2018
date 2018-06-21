CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export PYTHONPATH=$PYTHONPATH:$CURRENT_DIR/src/

export FLASK_APP=drainbow_mcc.webapp
export MAVLINK20=True
export MAVLINK_DIALECT=granum
export DRAINBOW_MCC_CONFIG=drainbow_mcc.configs.dev
