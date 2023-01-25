//  wordleGuessSecretWord.c
//  Have the program do the guessing to discover the secret wordle word.
//
//  Author: Aniket Jasani
//  System: CLion and Replit
//  UIC Fall 2022 Dale Reed
//
/*  Running the program looks like:
        Using file wordsLarge.txt with 12947 words. 
        -----------------------------------------------------------
        
        enter a secret word or just r to choose one at random: dream
        Trying to find secret word: 
               d r e a m 
        
            1. s o r e s 
                   * *   
            2. s e r e r 
                 * *     
            3. r a r e r 
               * *   *   
            4. r a r e e 
               * *   *   
            5. s a r e e 
                 * * *   
            6. p a r e r 
                 * * *   
            7. c a r e r 
                 * * *   
            8. b a r e r 
                 * * *   
            9. f a r e r 
                 * * *   
           10. D a r e r 
                 * * *   
           11. D e a r e 
                 * * *   
           12. D e b A r 
                 *     * 
           13. D R E A r 
                       * 
           14. D R E A d 
                         
           15. D R E A M 
                         
        Got it!

     ... and then it runs two more times ...
 */
#include <stdio.h>    // for printf(), scanf()
#include <stdlib.h>   // for exit( -1)
#include <string.h>   // for strcpy
#include <assert.h>   // for assert() sanity checks
#include <ctype.h>    // for toupper()
#include <time.h>     // for time()

// Declare globals
#define WORD_LENGTH 5     // All words have 5 letters, + 1 NULL at the end when stored
#define WORDS_FILE_NAME  "wordsLarge.txt"
#define MAX_NUMBER_OF_WORDS 12947   // Number of words in the full set of words file
#define true 1   // Make boolean logic easier to understand
#define false 0  // Make boolean logic easier to understand

typedef struct wordCount wordCountStruct;
struct wordCount{
    char word[ WORD_LENGTH + 1];   // The word length plus NULL
    int score;                     // Score for the word
};


//-----------------------------------------------------------------------------------------
// Read in words from file into array.  We've previously read the data file once to
// find out how many words are in the file.
void readWordsFromFile(
            char fileName[],        // Filename we'll read from
            wordCountStruct *words, // Array of words where we'll store words we read from file
            int *wordCount)          // How many words.  Gets updated here and returned
{
    FILE *inFilePtr  = fopen(fileName, "r");  // Connect logical name to filename
    assert( inFilePtr != NULL);               // Ensure file open worked correctly

    // Read each word from file and store into array, initializing the score for that word to 0.
    char inputString[ 6];
    *wordCount = 0;
    while( fscanf( inFilePtr, "%s", inputString) != EOF) {
        strcpy( words[ *wordCount].word, inputString);
        words[ *wordCount].score = true;
        (*wordCount)++;
    }

    // Close the file
    fclose( inFilePtr);
} // end readWordsFromFile(..)


//-----------------------------------------------------------------------------------------
// Comparator for use in built-in qsort(..) function.  Parameters are declared to be a
// generic type, so they will match with anything.
// This is a two-part comparison.  First the scores are compared.  If they are the same,
// then the words themselves are also compared, so that the results are in descending
// order by score, and within score they are in alphabetic order.
int compareFunction( const void * a, const void * b) {
    // Before using parameters we have cast them into the actual type they are in our program
    // and then extract the numerical value used in comparison
    int firstScore = ((wordCountStruct *) a)->score;
    int secondScore = ((wordCountStruct *) b)->score;

    // If scores are different, then that's all we need for our comparison.
    if (firstScore != secondScore) {
        // We reverse the values, so the result is in descending vs. the otherwise ascending order
        // return firstScore - secondScore;   // ascending order
        return secondScore - firstScore;      // descending order
    }
    else {
        // Scores are equal, so check words themselves, to put them in alphabetical order
        return strcmp( ((wordCountStruct *)a)->word,  ((wordCountStruct *)b)->word );
    }
} //end compareFunction(..)


//-----------------------------------------------------------------------------------------
// Compares the word against the secret word. Prints a capital letter if the letter matches and is in the
// right position. Prints an '*' underneath if the letter is in the secret word, but it is in the wrong position
// in the guessed word. Based on the feedback, removes words from consideration if they do not contain any of
// the matched letters.
void processWord(
        char computerGuess[],           // Guessed word
        char secretWord[],              // The word to be guessed
        int *guessNumber,               // Number of guesses taken
        int wordCount,                  // How many words there are in allWords
        int *found,                     // Tracks if the word has been correctly guessed
        wordCountStruct allWords[]      // Array of all the words
        ) {

    // Make a copy of the computer guess and of the secret word
    char computerGuessCopy[ WORD_LENGTH + 1];
    char secretWordCopy[ WORD_LENGTH + 1];
    strcpy( computerGuessCopy, computerGuess);
    strcpy( secretWordCopy, secretWord);

    printf("%5d.", *guessNumber);
    for ( int i = 0; i < WORD_LENGTH; i++) {  // Each character in the guess word
        if (computerGuessCopy[i] != secretWordCopy[i]) {
            printf(" %c", computerGuessCopy[i]);
        }
        else {  // Character matches exactly in position
            // Go through all words and set words without the exact match as invalid guess words
            for( int j = 0; j < wordCount; j++) {
                if (allWords[j].score) {  // Still a valid guess
                    if (allWords[j].word[i] != computerGuessCopy[i]) {
                        allWords[j].score = false;
                    }
                }
            }
            printf(" %c", toupper(computerGuessCopy[i]));
            computerGuessCopy[i] = '_';
            secretWordCopy[i] = '-';
        }
    }
    printf("\n");
    printf("      ");

    // Display '*' if in wrong position
    for( int i = 0; i < WORD_LENGTH; i++) {  // Each character in guess word
        char displayCharacter = ' ';
        for( int j = 0; j < WORD_LENGTH; j++) {  // Each character in secret word
            if( computerGuessCopy[i] == secretWord[j]) {
                computerGuessCopy[i] = '_';
                secretWordCopy[j] = '-';
                displayCharacter = '*';
                break;
            }
        }
        // For every character that does NOT exist in the secret word, go through all words
        // and set words with those letters as invalid guess words
        for( int j = 0; j < wordCount; j++) {
            if( allWords[j].score) {    // Still a valid guess
                if (displayCharacter == ' ') {
                    if (strchr(allWords[j].word, computerGuessCopy[i])) {
                        allWords[j].score = false;
                    }
                }
            }
        }
        printf(" %c", displayCharacter);
    }
    printf("\n");

    // Check if guess is correct
    if( strcmp ( computerGuess, secretWord) == 0) {
        *found = true;
    }
    (*guessNumber)++;
}

// -----------------------------------------------------------------------------------------
// Find a secret word. Keep guessing until guess is correct. Narrow list of valid guesses
// during each guess.
void findSecretWord(
        wordCountStruct allWords[],     // Array of all the words
        int wordCount,                  // How many words there are in allWords
        char secretWord[])              // The word to be guessed
{
    char computerGuess[ 6];  // Allocate space for the computer guess
    int found = false;
    int guessNumber = 1;
    
    printf("Trying to find secret word: \n");
    // Display secret word with a space between letters, to match the guess words below.
    printf("       ");
    for( int i = 0; i < WORD_LENGTH; i++) {
        printf("%c ", secretWord[ i]);
    }
    printf("\n");
    printf("\n");

    // Top first word to guess
    strcpy( computerGuess, "salet");
    processWord(computerGuess, secretWord, &guessNumber, wordCount, &found, allWords);

    // Loop until the word is found
    for( int i = 0; i < wordCount; i++) {
        // Choose next guess from one of the valid guess words
        if (allWords[i].score) {
            strcpy( computerGuess, allWords[i].word);
            processWord(computerGuess, secretWord, &guessNumber, wordCount, &found, allWords);
            if (found) {
                break;
            }
        }
    }
    // Reset scores for all words
    for (int i = 0; i < wordCount; i++) {
        allWords[i].score = true;
    }
} //end findSecretWord


// -----------------------------------------------------------------------------------------
int main() {
    char wordsFileName[81];                   // Stores the answers file name
    strcpy(wordsFileName, WORDS_FILE_NAME);   // Set the filename, defined at top of program.
    srand( (unsigned) time( NULL));           // Seed the random number generator to be current time
    // Declare space for all the words, of a maximum known size.
    wordCountStruct allWords[ MAX_NUMBER_OF_WORDS];
    // Start out the wordCount to be the full number of words.  This will decrease as
    //    play progresses each time through the game.
    int wordCount = 0;
    // The secret word that the computer will try to find, plus the return character from fgets.
    char secretWord[ WORD_LENGTH + 1];
    char userInput[ 81];                // Used for menu input of secret word

    // Read in words from file, update wordCount and display information
    readWordsFromFile( wordsFileName, allWords, &wordCount);
    printf("Using file %s with %d words. \n", wordsFileName, wordCount);

    // Run the word-guessing game three times
    for( int i=0; i<3; i++) {
        // Reset secret Word
        strcpy( secretWord, "");
        // Display prompt
        printf("-----------------------------------------------------------\n");
        printf("\n");
        printf("Enter a secret word or just r to choose one at random: ");
        scanf(" %s", userInput);
        // Eliminate the return character at end or userInput if it is there
        int length = (int) strlen( userInput);
        if( userInput[ length] == '\n') {
            userInput[ length] = '\0';
        }
        strcpy( secretWord, userInput);   // Store the secret word from user input
                
        // If input was 'r' then choose a word at random.
        if( strlen( secretWord) <= 1) {
            // Randomly select a secret word to be guessed.
            int randomIndex = rand() % wordCount;
            strcpy( secretWord, allWords[ randomIndex].word);
        }

        // Run the game once with the current secret word
        findSecretWord( allWords, wordCount, secretWord);
        printf("Got it!\n");
    }
    printf("Done\n");
    printf("\n");
    return 0;
} // end main()