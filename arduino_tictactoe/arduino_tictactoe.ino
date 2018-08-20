//
// PIN SETUP
//

//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = 10;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 12;
//Pin connected to Data in (DS) of 74HC595
const int dataPin = 11;

//button pins, e.g. button21 = button in column 2 row 1
//Pin PC0 Arduino Uno
const int button11 = 14;
//Pin PC1 Arduino Uno
const int button12 = 15;
//Pin PC2 Arduino Uno
const int button13 = 16;
//Pin PC3 Arduino Uno
const int button21 = 17;
//Pin PC4 Arduino Uno
const int button22 = 18;
//Pin PC5 Arduino Uno
const int button23 = 19;
//Pin PD5 Arduino Uno
const int button31 = 5;
//Pin PD6 Arduino Uno
const int button32 = 6;
//Pin PD7 Arduino Uno
const int button33 = 7;

// Array with pin numbers for LEDs per row
int ledRows[] = {2, 3, 4};
// Array with pin numbers for red LEDs per column
int redLedColumns[] = {0, 2, 4};
// Array with pin numbers for green LEDs per column
int greenLedColumns[] = {1, 3, 5};

//
// GENERAL VARIABLES
//

// Nr of leds on the board
const int BOARD_SIZE = 9;
// Current board status
int ticTacToe[3][3] = {{0, 0, 0},
                       {0, 0, 0},
                       {0, 0, 0}};
// Winning row formations
const int WINNING_FORMATIONS_NR = 8;
int rowWins[8][3][2] = 
  {
    {
      {0, 0},
      {0, 1},
      {0, 2}
    },
    {
      {1, 0},
      {1, 1},
      {1, 2}
    },
    {
      {2, 0},
      {2, 1},
      {2, 2}
    },
    {
      {0, 0},
      {1, 0},
      {2, 0}
    },
    {
      {0, 1},
      {1, 1},
      {2, 1}
    },
    {
      {0, 2},
      {1, 2},
      {2, 2}
    },
    {
      {0, 0},
      {1, 1},
      {2, 2}
    },
    {
      {0, 2},
      {1, 1},
      {2, 0}
    }
  };
// Winning data
int winningRow[3][2];
int winningPlayer = 0;

// Reset data
bool resetReady = false;
bool gameReady = true;

// Timing variables
unsigned long previousMillis = 0;
const int interval = 500; // in ms
int blinking = 0;

// Indication for waiting for ML application move
bool waitingForApp = false;

// Incoming byte for move from ML application
uint8_t incomingByte;

// Board state to write to ML application
uint8_t boardState[BOARD_SIZE];
// Board constants
const int BOARD_DIMENSION = 3;
const int VACANT_CELL = 0;
const int MINUS_PLAYER = 1;
const int PLUS_PLAYER = 2;
const int DRAW = -1;

//
// INITIALIZATION
//

void setup() {
  // Set 74HC595 pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  Serial.begin(19200);
 
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);  
  pinMode(4, OUTPUT);

  // Set 9 buttons as input
  pinMode(button11, INPUT);
  pinMode(button12, INPUT);
  pinMode(button13, INPUT);
  pinMode(button21, INPUT);
  pinMode(button22, INPUT);
  pinMode(button23, INPUT);
  pinMode(button31, INPUT);
  pinMode(button32, INPUT);
  pinMode(button33, INPUT);
}

//
// GAME FUNCTIONS
//

void checkDraw(){
  int nr_of_board_pos_filled = 0;
  for(int i = 0; i < BOARD_DIMENSION; i++){
    for(int j = 0; j < BOARD_DIMENSION; j++){
      if(ticTacToe[i][j] > 0){
        nr_of_board_pos_filled++;
      }
    }
  }
  if(nr_of_board_pos_filled == BOARD_DIMENSION * BOARD_DIMENSION){
      //indicate game ended in draw
      winningPlayer = DRAW;
      
      // Wait resetting until it is ready
      resetReady = false;
  }
}

// Check if game is won by someone
void checkWin() {
  // Check for every possible winning row formation
  for(int i = 0; i < WINNING_FORMATIONS_NR; i++) {
    int currentRowPinsSetNr = 0;
    int checkedPlayer = 0;

    // For each coördinate in formation
    for(int j = 0; j < BOARD_DIMENSION; j++) {
      // Get led value of coördinate
      int coor1 = rowWins[i][j][0];
      int coor2 = rowWins[i][j][1];
      int value = ticTacToe[coor1][coor2];
      
      // If a pin in current checked possible winning row is set by a player
      if(value != 0) {
        // If this is first checked coördinate of the formation
        if(checkedPlayer == 0) {
          // Set first found player as checked player
          checkedPlayer = value;
          currentRowPinsSetNr++;
        }
        
        // If this is not the first checked coördinate of the formation
        else {
          // If new value in formation was set by same player as last value
          if(value == checkedPlayer) {
            // Increase set pin in possible winning formation
            currentRowPinsSetNr++;
          }
        }
      }
    }

    // If all 3 pins of a possible winning formation are set
    if(currentRowPinsSetNr == 3) {
      // Copy winning row formation
      winningRow[0][0] = rowWins[i][0][0];
      winningRow[0][1] = rowWins[i][0][1];
      winningRow[1][0] = rowWins[i][1][0];
      winningRow[1][1] = rowWins[i][1][1];
      winningRow[2][0] = rowWins[i][2][0];
      winningRow[2][1] = rowWins[i][2][1];

      // Indicate game is won
      winningPlayer = checkedPlayer;

      // Wait resetting until it is ready
      resetReady = false;
    }
  }
}

// Reset game
void resetGame() {
  // Reset board
  ticTacToe[0][0] = 0;
  ticTacToe[0][1] = 0;
  ticTacToe[0][2] = 0;
  ticTacToe[1][0] = 0;
  ticTacToe[1][1] = 0;
  ticTacToe[1][2] = 0;
  ticTacToe[2][0] = 0;
  ticTacToe[2][1] = 0;
  ticTacToe[2][2] = 0;

  // Reset winner
  winningPlayer = 0;

  // Indicate users turn
  waitingForApp = false;

  // Wait game start until it is ready
  gameReady = false;
  
}

//
// LED FUNCTIONS
//

// Light up a led
void lightLed(int result, int col, int row) {
  switch(result) {
    case MINUS_PLAYER:
      digitalWriteCol(redLedColumns[col], HIGH);
      digitalWriteRow(ledRows[row]);
      digitalWriteCol(redLedColumns[col], LOW);
      break;
    case PLUS_PLAYER:
      digitalWriteCol(greenLedColumns[col], HIGH);
      digitalWriteRow(ledRows[row]);
      digitalWriteCol(greenLedColumns[col], LOW);
      break;
    default:
    if(gameReady){
      digitalWriteCol(redLedColumns[col], LOW);
      digitalWriteCol(greenLedColumns[col], LOW);
    }

      break;
  }
}

// Display board status on leds
void loadTicTacToeLeds(){
  for(int i = 0; i < BOARD_DIMENSION; i++) {
    for(int j = 0; j < BOARD_DIMENSION; j++) {
      lightLed(ticTacToe[i][j], i, j);
    }
  }
}

// Activate a LED row
void digitalWriteRow(int outputNr){
  for(int i = 0; i < BOARD_DIMENSION; i++) {
    if(outputNr == ledRows[i]) {
      digitalWrite(ledRows[i], HIGH);
    }
    else {
      digitalWrite(ledRows[i], LOW);
    }
  }
}

// Activate a LED column
void digitalWriteCol(int outputNr, int state) {
  // Bits you want to send
  byte bitsToSend = 0;

  // Turn on the output so the LEDs can light up:
  digitalWrite(latchPin, HIGH);

  // Turn on the next highest bit in bitsToSend:
  bitWrite(bitsToSend, outputNr, state);

  // Shift the bits out to the shift register
  shiftOut(dataPin, clockPin, MSBFIRST, bitsToSend);

  // Turn off the output so the pins don't light up, while you're shifting bits
  digitalWrite(latchPin, LOW);
}

//
// USER INPUT
//

// User set cell
void userSetCell(int i, int j) {
    if(ticTacToe[i][j] == 0 && waitingForApp == false) {
        ticTacToe[i][j] = PLUS_PLAYER;
        writeBoardState();
    }
}

//
// ML APPLICATION COMMUNICATION
//

// Get and write board state to ML application
void writeBoardState() {
  // Get board state  
  for(int i = 0; i < BOARD_DIMENSION; i++){
    for(int j = 0; j < BOARD_DIMENSION; j++){
      int flatPosition = i * BOARD_DIMENSION + j;
      boardState[flatPosition] = (int8_t)ticTacToe[i][j];
    }
  }
   delay(10);
  // Write board state to ML application each byte at a time
  for (uint8_t i = 0; i < BOARD_SIZE; i++){
    Serial.write(boardState[i]);
  }

  waitingForApp = true;
}

// Read move from ML application
void readAndSetMove() {
  Serial.flush();
   delay(10);
  if(Serial.available() == 1) {
    // Get move data
    incomingByte = Serial.read();
    
    // Write move to board state
    int ledrow = incomingByte / BOARD_DIMENSION;
    int ledcol = incomingByte % BOARD_DIMENSION;
    ticTacToe[ledrow][ledcol] = MINUS_PLAYER;    

    // Enable player move
    waitingForApp = false;

    checkDraw();
    // Check if application wins after move was set
    checkWin();
  } 
}

// Method to read button presses
void readButtonInput() {
  // This variable is unset later when no buttons were pressed
  bool buttonPressed = true;
  
  if(digitalRead(button11) == HIGH){
      userSetCell(0, 0);
  }
  else if(digitalRead(button12) == HIGH){
      userSetCell(0, 1);
  }
  else if(digitalRead(button13) == HIGH){
      userSetCell(0, 2);
  }
  else if(digitalRead(button21) == HIGH){
      userSetCell(1, 0);
  }
  else if(digitalRead(button22) == HIGH){
      userSetCell(1, 1);
  }
  else if(digitalRead(button23) == HIGH){
      userSetCell(1, 2);
  }
  else if(digitalRead(button31) == HIGH){
      userSetCell(2, 0);
  }
  else if(digitalRead(button32) == HIGH){
      userSetCell(2, 1);
  }
  else if(digitalRead(button33) == HIGH){
      userSetCell(2, 2);
  }
  else {
    buttonPressed = false;
  }

  // Check if player wins after button was pressed
  if(buttonPressed == true) {
    checkDraw();
    checkWin();
  }
}

// Check button press
bool isButtonPressed() {
  if( digitalRead(button11) == HIGH || 
      digitalRead(button12) == HIGH || 
      digitalRead(button13) == HIGH ||
      digitalRead(button21) == HIGH ||
      digitalRead(button22) == HIGH ||
      digitalRead(button23) == HIGH ||
      digitalRead(button31) == HIGH ||
      digitalRead(button32) == HIGH ||
      digitalRead(button33) == HIGH) {
        return true;
      }
      else {
        return false;
      }
}

//
// MAIN LOOP
//

void loop() {
  // Poll for user input
  if(gameReady == true && waitingForApp == false && winningPlayer == 0) {
    readButtonInput();
  }
  
  // Wait for app input
  if(waitingForApp == true && winningPlayer == 0) {
    readAndSetMove();
  }

  // Reset game if button is pressed after game end
  if(resetReady == true && winningPlayer != 0 && isButtonPressed()) {
    resetGame();
  }

  // Increase counter after time
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {

    // Blink leds when game is won
    if(winningPlayer != 0) {
      if(winningPlayer == DRAW){
        for(int i = 0; i < BOARD_DIMENSION; i++){
          for(int j = 0; j < BOARD_DIMENSION; j++){
            ticTacToe[i][j] = blinking;
          }
        }
          // Blink on or off depending on current state
        if(blinking == 0) {
          blinking = 1;
        } else if (blinking == 1){
            blinking = 2;
        }
          else {
            blinking = 0;
        }
      } else {
        // Blink leds of winning row
        for(int i = 0; i < BOARD_DIMENSION; i++) {
            ticTacToe[winningRow[i][0]][winningRow[i][1]] = blinking;
          } 

        // Blink on or off depending on current state
        if(blinking == 0) {
          blinking = winningPlayer;
        }
        else {
          blinking = 0;
        }
      }
    
    
    }
    
    // Save the last time you had an interval
    previousMillis = currentMillis;
  }

  // Load board state into leds
  loadTicTacToeLeds();

  // Set reset ready after user stops pressing button after game is won
  if(resetReady == false && winningPlayer != 0 && isButtonPressed() == false) {
    resetReady = true;
  }

  // Set game ready after user stops pressing button after game reset
  if(gameReady == false && winningPlayer == 0 && isButtonPressed() == false) {
    gameReady = true;
  }
}
