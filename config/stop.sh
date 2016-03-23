BASE_DIR=/spare/local/FoodThought/

# get pid
PID=`ps | grep FoodThought | cut -d" " -f1`

# send SIGINT
kill -2 ${PID}
