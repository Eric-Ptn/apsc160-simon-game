/* Author: Eric Petersen
 * Lab 8, L1M
 *
 * Purpose: Plays the Simon game with the user.
 *
 * 		Rules: The 4 main LEDs light up in sequence,
 *		and the user presses buttons in the order that
 *      the LEDs flashed. If the user gets the sequence
 *		correct, then the sequence gets longer up to a
 *		maximum size. If the user gets the sequence right
 *		at the maximum size, then the top right green LED
 *		flashes and the user wins, and a new game is started.
 *		If at any point the user gets the sequence wrong, the
 *		top left red LED flashes and a new game is started.
 *
 *
*/

#define SEQUENCE_SIZE 5 //max LED sequence length
#define numberLEDs 4
#define SECOND 1000

//define light pins
int const RED = 12;
int const YELLOW = 11;
int const BLUE = 10;
int const GREEN = 9;

int const winLight = 8;
int const loseLight = 13;

//define button pins
int const redButton = 7;
int const yellowButton = 6;
int const blueButton = 5;
int const greenButton = 4;

//create a random sequence of numbers (later translated into buttons and LEDs)
void createSequence(int sequence[]) {

    for (int index = 0; index < SEQUENCE_SIZE; index++) {
        sequence[index] = random(0, numberLEDs);
    }

}

//convert number sequence into LED pins and button pins
void outputSequences(int LEDSequence[], int buttonSequence[], int sequence[]) {

    for (int index = 0; index < SEQUENCE_SIZE; index++) {

        if (sequence[index] == 0) {
            LEDSequence[index] = GREEN;
            buttonSequence[index] = greenButton;
        }
        else if (sequence[index] == 1) {
            LEDSequence[index] = RED;
            buttonSequence[index] = redButton;
        }
        else if (sequence[index] == 2) {
            LEDSequence[index] = YELLOW;
            buttonSequence[index] = yellowButton;
        }
        else if (sequence[index] == 3) {
            LEDSequence[index] = BLUE;
            buttonSequence[index] = blueButton;
        }

    }

}

//flash "winning" green light 3 times
void win() {

    for (int i = 0; i < 3; i++) {

        digitalWrite(winLight, HIGH);
        delay(SECOND);
        digitalWrite(winLight, LOW);
        delay(SECOND);

    }

    delay(SECOND);

}

//flash "losing" red light 3 times
void loss() {

    for (int i = 0; i < 3; i++) {

        digitalWrite(loseLight, HIGH);
        delay(SECOND);
        digitalWrite(loseLight, LOW);
        delay(SECOND);

    }

    delay(SECOND);

}

//reset the "user input" sequence, which stores the pins of the buttons pressed by the user
void initUserInput(int userInput[]) {

    for (int i = 0; i < SEQUENCE_SIZE; i++) {
        userInput[i] = -1;
    }

}

//function to reset the user input, create a new sequence, and translate that new sequence into pins
void reset(int userInput[], int sequence[], int LEDSequence[], int buttonSequence[]) {

    initUserInput(userInput);
    createSequence(sequence);
    outputSequences(LEDSequence, buttonSequence, sequence);

}

//function to check if a button has been pressed or not
void checkButton(int button, int arrayIndex, int loopIndex, int buttonStatus[], int userInput[]) {

    //if a button is being pressed and it was off previously, it is now on (button status "1")
    if (digitalRead(button) == HIGH && buttonStatus[arrayIndex] == 0) {

        buttonStatus[arrayIndex] = 1;

    } //if a button is not being pressed and it was on previously, it is now off (button status "0")
      //the user input is stored as the button that was released
    else if (digitalRead(button) == LOW && buttonStatus[arrayIndex] == 1) {

        userInput[loopIndex] = button;
        buttonStatus[arrayIndex] = 0;

    }

}

void setup()
{
    //set the random seed
    randomSeed(analogRead(0));

    //initialize pins
    pinMode(GREEN, OUTPUT);
    pinMode(RED, OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(BLUE, OUTPUT);

    pinMode(greenButton, INPUT);
    pinMode(redButton, INPUT);
    pinMode(yellowButton, INPUT);
    pinMode(blueButton, INPUT);

}

void loop() {

    static int correct = -1;
    /* tracks if entered sequence is correct,
    -1 for no answer yet, 0 for wrong, 1 for correct*/

    static int gameStage = 1; //keeps track of game "level"

    int gameState = 0;
    // 0 for displaying light, 1 for user input, 2 for wins/losses

    int sequence[SEQUENCE_SIZE];
    // sequence of random numbers from 0 to numberLEDs - 1

    int LEDSequence[SEQUENCE_SIZE];
    // sequence[] converted to LED pins

    int buttonSequence[SEQUENCE_SIZE];
    // sequence[] converted to buttons

    int userInput[SEQUENCE_SIZE];
    // stores user's button inputs

    static int buttonStatus[numberLEDs] = { 0 };
    // to keep track of pushbutton states

    /* generate sequences at start of game or when a game is won/lost,
    and reset buttons and game Stage */

    if ((correct == 1 && gameStage > SEQUENCE_SIZE) || correct != 1) {

        reset(userInput, sequence, LEDSequence, buttonSequence);
        gameStage = 1;
        for (int i = 0; i < numberLEDs; i++) {
            buttonStatus[i] = 0;
        }

    }

    //after every run of the loop, reset the answer variable
    correct = -1;

    if (gameState == 0) {

        //display LED sequence
        for (int i = 0; i < gameStage; i++) {

            digitalWrite(LEDSequence[i], HIGH);
            delay(SECOND);
            digitalWrite(LEDSequence[i], LOW);
            delay(SECOND);

        }

        gameState = 1;

    }

    if (gameState == 1) {

        int index = 0;

        //check if each button has been pressed
        while (index < gameStage) {

            checkButton(greenButton, 0, index, buttonStatus, userInput);
            checkButton(redButton, 1, index, buttonStatus, userInput);
            checkButton(yellowButton, 2, index, buttonStatus, userInput);
            checkButton(blueButton, 3, index, buttonStatus, userInput);

            //if the user pressed a button, then wait for next input
            if (userInput[index] != -1) {
                index++;
            }

        }

        gameState = 2;

    }

    if (gameState == 2) {

        //check if any of the inputs are wrong
        for (int i = 0; i < gameStage; i++) {

            if (userInput[i] != buttonSequence[i]) {

                correct = 0;

            }

        }

        /*if none of the inputs are wrong and the correct variable
        hasn't been updated, then the input is right*/
        if (correct == -1) {

            correct = 1;

        }

        if (correct == 1 && gameStage == SEQUENCE_SIZE) {

            win();
            gameStage++;

        }
        else if (correct == 1 && gameStage < SEQUENCE_SIZE) {

            gameStage++;
            initUserInput(userInput);
            for (int i = 0; i < numberLEDs; i++) {
                buttonStatus[i] = 0;
            }

        }
        else {

            loss();

        }

    }

}
