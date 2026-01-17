echo "Starting 4 test processes..."

tail -f /dev/null &
PID1=$!
echo "1. tail -f /dev/null started with PID: $PID1"

yes > /dev/null &
PID2=$!
echo "2. yes > /dev/null started with PID: $PID2"

sleep 300 &
PID3=$!
echo "3. sleep 300 started with PID: $PID3"

ping localhost > /dev/null &
PID4=$!
echo "4. ping localhost started with PID: $PID4"

echo ""
echo "All processes started!"