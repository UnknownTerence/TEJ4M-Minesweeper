 /*
	Alex Drikos, Yazhou Liu, Terence Nguyen, Dante Sangoi
    11/13/2023
    Summative Unit Project
    Attempting to create minesweeper
*/

/*
	NOTES AND TO DO LIST
    
    --> USE # FOR CLOSED AND - FOR OPEN
    --> USE LETTERS AND NUMBERS FOR ROWS AND COLUMNS
    --> USE FUNCTION TO CONVERT LETTERS TO NUMBERS
    --> NEED RNG TO PLACE BOMBS
    --> CHECK IF SPOT ALREADY HAS BOMBS
    --> FUNCTION FOR CHECKING ADJACENT SQUARES FOR BOMBS
    
    SCENES
    --> GAME
    --> HELP SCREEN
    
    LATER
    --> FLAGGING
    --> IF HAVE TIME, CREATE A BIGGER BOARD
*/

// VARIABLE DECLARATION
int randint;
int randint2;
int choice;
int inputX;
int inputY;
int inputCell;
bool gameStatus = true;
int counter;

// PLAYER BOARD
String mainBoard[8][8] = { // [y][x]
  //0     1     2     3     4     5     6     7
  {"#", "#", "#", "#", "#", "#", "#", "#"}, // A 0
  {"#", "#", "#", "#", "#", "#", "#", "#"}, // B 1
  {"#", "#", "#", "#", "#", "#", "#", "#"}, // C 2
  {"#", "#", "#", "#", "#", "#", "#", "#"}, // D 3
  {"#", "#", "#", "#", "#", "#", "#", "#"}, // E 4
  {"#", "#", "#", "#", "#", "#", "#", "#"}, // F 5
  {"#", "#", "#", "#", "#", "#", "#", "#"}, // G 6
  {"#", "#", "#", "#", "#", "#", "#", "#"}  // H 7
};

// BEHIND THE SCENES BOARD
String bombBoard[8][8] = { // [y][x]
  //0     1     2     3     4     5     6     7
  {"00", "10", "20", "30", "40", "50", "60", "70"}, // A 0
  {"10", "11", "12", "13", "14", "15", "16", "17"}, // B 1
  {"20", "21", "22", "23", "24", "25", "26", "27"}, // C 2
  {"30", "31", "32", "33", "34", "35", "36", "37"}, // D 3
  {"40", "41", "42", "43", "44", "45", "46", "47"}, // E 4
  {"50", "51", "52", "53", "54", "55", "56", "57"}, // F 5
  {"60", "61", "62", "63", "64", "65", "66", "67"}, // G 6
  {"70", "71", "72", "73", "74", "75", "76", "77"}  // H 7
};

// CUSTOM FUNCTIONS


// line divider

void divider() {
  Serial.println("______________________________");
}

// Prints the board
void printBoard() {
  Serial.println("   0 1 2 3 4 5 6 7"); // X VALUES
  Serial.println("   _ _ _ _ _ _ _ _"); // Y VALUES
  for (int y=0;y<8;y++) { // CHANGES ROWS
    Serial.print(String(y)+" |");
    for (int x=0;x<8;x++) { // CHANGES COLUMNS
      Serial.print(mainBoard[y][x]+" "); // PRINTS VALUE
    }
    Serial.println();
  }
  winCondition(); // CHECKS IF THEY WON
  divider(); // DIVIDER
}

// Generate Mines
void generateBombs(int x, int y) {
  for (int i=0;i<10;i++) {
    randint = random(0,8); // GENERATES BOMB Y
  	randint2 = random(0,8); // GENERATES BOMB X
    while (randint == y && randint2 == x) { // MAKES SURE BOMB DOESNT GENERATE ON FIRST CLICK
      randint = random(0,8);
      randint2 = random(0,8);
    }
    if (bombBoard[randint][randint2] != "XX") { // CHECKS IF BOMB IS NOT ON SPOT
      bombBoard[randint][randint2] = "XX";
    }
    else { // IF IT IS THEN IT REGENERATES IT
      i--;
    }
  }
}

// Finds bombs in adjacent tiles
void findAdjacent(int x, int y) { // a and b, row and column, letter and number
  // Have nested for loop that counts all around the inputted coordinate
  if (y < 0 || y > 7 || x < 0 || x > 7 || bombBoard[y][x] == "OO") { // base case for recursion
    // checks if the cell has already been opened and is not out of bounds
    return; // terminates the function
  }
  int count=0; // counts amt of bombs around
  if (bombBoard[y][x]=="XX") { // checks if input == bomb
    revealBombs(); // REVEAL BOMB LOCATIONS
    printBoard();
    Serial.println("YOU HAVE LOST!");
    gameStatus = false;
  }
  else {
    count += checkTile(y-1,x-1); // top left
    count += checkTile(y-1,x);   // top
    count += checkTile(y-1,x+1); // top right

    count += checkTile(y,x-1); // left
    count += checkTile(y,x+1); // right

    count += checkTile(y+1,x-1); // btm left
    count += checkTile(y+1,x); // btm
    count += checkTile(y+1,x+1); // btm right

    bombBoard[y][x] = "OO"; // BEHIND THE SCENES, MARKS CELL AS OPENED

    if (count > 0) { // CHECKS ITHERE IS BOMB IN THE VICINITY
      mainBoard[y][x] = count;
    }
    else if (count == 0) {
      mainBoard[y][x] = "-"; // marks cell as opened and no bombs
      // uses recursion to continuously check for bombs until a bomb is found
      findAdjacent(x-1,y-1); // top left
      delay(10);
      findAdjacent(x,y-1);   // top
      delay(10);
      findAdjacent(x+1,y-1); // top right
      delay(10);

      findAdjacent(x-1,y); // left
      delay(10);
      findAdjacent(x+1,y); // right
      delay(10);

      findAdjacent(x-1,y+1); // btm left
      delay(10);
      findAdjacent(x,y+1); // btm
      delay(10);
      findAdjacent(x+1,y+1); // btm right
      delay(10);

      // The delays make sure the esp32 isn't overloaded
    }
  }
}

int checkTile(int row, int column) { // Checks the tile of the parameter
  if (row < 0 || row > 7 || column < 0 || column > 7) {
    return 0; // no bomb
  }
  if (bombBoard[row][column] == "XX") {
    return 1; // increases bomb count
  }
  return 0; // no bomb
}

// Reveals bomb if player has lost
void revealBombs() { // replaces all tiles except bombs with "-"
  for (int y=0;y<8;y++) {
    for (int x=0;x<8;x++) {
      if (bombBoard[y][x] == "XX") {
        mainBoard[y][x] = "X"; // replaces bombs with "X"
      }
      else {
        mainBoard[y][x] = "-";
      }
    }
  }
}

// Checks if the player has won
void winCondition() {
  int cells=0;
  for (int y=0;y<8;y++) {
    for (int x=0;x<8;x++) {
      if (bombBoard[y][x] == "XX" || bombBoard[y][x] == "OO") cells++;
    }
  }
  if (cells==64) {
    Serial.println("YOU HAVE WON!");
    gameStatus = false;
  }
}

void setup()
{
  Serial.begin(115200);
  randomSeed(analogRead(0));
  
  while(choice!=2){

    Serial.println("WELCOME TO BOMB SWEEPER(TM): HELP SCREEN(1) PLAY GAME(2)");
    
    while(Serial.available()==0){}
    choice=Serial.parseInt();
    
    if(choice==1){ // Help
      Serial.println("------| HOW TO PLAY |------");
      Serial.println("THIS IS NOT A RIP OFF OF MINESWEEPER, THE DIFFERENCE IS THAT IT IS THE SAME!");
      Serial.println("In Bomb Sweeper there is a grid full of hidden mines that are revealed by adjacent tiles.");
      Serial.println("The number will be shown when there is a bomb in either of the corners or sides connected.");
      Serial.println("The objective is to remove every non bomb space.");
      Serial.println("IF YOU CLICK ON A BOMB YOU WILL DIE");
    }
  }
  if(choice==2){
    while (gameStatus == true) {
      inputX = 8; // resets input to be greater than 7
      inputY = 8; // resets input to be greater than 7
      
      while (inputX < 0 || inputX > 7) { // checks for correct input
        Serial.print("Enter X Coordinate (0-7): ");
        Serial.flush(); // clears the buffer
        delay(100);
        while(Serial.available()==1){}
        inputX=Serial.parseInt();
        Serial.println(inputX);
      }
      
      delay(250);

      while (inputY < 0 || inputY > 7) { // checks for correct input
        Serial.print("Enter Y Coordinate (0-7): ");
        Serial.flush(); // clears the buffer
        delay(100);
        while(Serial.available()==1){}
        inputY=Serial.parseInt();
        Serial.println(inputY);
      }

      delay(500);

      if (counter == 0) { // generates bomb on first input
        generateBombs(inputX, inputY);
        counter++; // increases counter for only first input
      }

      findAdjacent(inputX, inputY); // detects the adjacent tiles and bombs
      if (bombBoard[inputY][inputX] != "XX") printBoard(); // only prints board if bomb has not been pressed
    }
  }
  // End Message
  Serial.println("Thank you for playing BOMBSWEEPER(TM)");
  Serial.println("If you wish to play again, re-run the program");
} // setup end
void loop()
{
  
}
