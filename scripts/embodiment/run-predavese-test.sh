#!/bin/tcsh

./killpb.csh
./cleanup.csh

echo "Start router, please wait..."
../bin/src/Control/MessagingSystem/router &
sleep 5
echo "Start LSMocky, please wait..."
../bin/src/Learning/LearningServer/LSMocky &
sleep 5
echo "Start PredaveseTest.rb, will start OAC..."
./run-predavese-mocky-proxy.rb &
sleep 2
echo "Start OAC, will start OAC..."
../bin/src/Control/OperationalAvatarController/opc 1 16330 &
