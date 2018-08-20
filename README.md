# README DeepTicTacToe Board

## Arduino
- [Install Arduino Studio](https://www.arduino.cc/en/Main/Software)
- Open the TicTacToe.ino file from the arduino folder in this project in Arduino Studio
- Connect your Arduino to your computer
- Select the correct port for the Arduino via: `Tools -> Port`. The arduino port will be the one starting with `cu.usbmode`.
- Compile and upload the Sketch via `Sketch -> Upload`.
- Insert the DeepTicTacToe board into the Arduino

## Docker
- Start your docker containers via the terminal/cmd by with the command  `docker-compose up` in the docker folder of this repo

## DeepTicTacToe Proxy
- Start the DeepTicTacToe proxy in the folder go_proxy.
  - Windows: `proxy_deeptictactoe.exe`
  - Mac Terminal: `./go_proxy`

## That's all folks
You should now be able to play against the trained network by using the hardware!
