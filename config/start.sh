BASE_DIR=/spare/local/FoodThought/

DAY_WEEK=`date +%w`
if [ $DAY_WEEK == 1 ] ; then
  LOOK_BACK=3
else
  LOOK_BACK=1
fi

TODAY=`date +%Y%m%d`
YESTERDAY=`date -d "${LOOK_BACK} day ago" +%Y%m%d`

# Create log dir
TODAY_LOGDIR=${BASE_DIR}/logs/${TODAY}
mkdir -p ${TODAY_LOGDIR}

# copy client store
YESTERDAY_LOGDIR=${BASE_DIR}/logs/${YESTERDAY}
cp ${YESTERDAY_LOGDIR}/client_store.today ${TODAY_LOGDIR}/client_store


# start the servr
export DATE=$TODAY
BIN_DIR=${BASE_DIR}/bin/
CONFIG_DIR=${BASE_DIR}/config/
${BIN_DIR}/FoodThought ${CONFIG_DIR}/config.ini &
