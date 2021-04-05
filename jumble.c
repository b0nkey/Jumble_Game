#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#define BOARDMIN 3 // Minimum board size


int mainMenu();
void generateBoard();
void printBoard();
void genSymbolPool();
void gameLoop();
void removeMatches();
void loadGame();
void gravity();
int okMove();
void repopulate();
int checkBoard();
void calcScore();
void checkTime();
int saveGame();


typedef struct setupData{ // Board variables that are frequently used together
    int boardSize;
    int symbolCount;
    time_t elapsedTime;
}setupData;


int main(){
    setupData setup;
    char allSymbols[]= {'@', '#', '$', '%', '&', '?', '+', '!', '~'}; // All possible symbols
    int score=0;
    setup.elapsedTime = 0;
    srand(time(0));

    while(1){
        if(mainMenu(&setup)==1){ // LOAD
            score=0;
            loadGame(&score);
        }else{
            score=0;
            char board[setup.boardSize][setup.boardSize];
            char symbols[setup.symbolCount];

            genSymbolPool(setup, allSymbols, symbols);
            generateBoard(setup, board, symbols);
            gameLoop(setup, board, symbols, &score);
        }
    }
    return 0;
}

void gameLoop(setupData s, char board[][s.boardSize], char symbols[s.symbolCount], int *pScore){
    time_t startTime;
    time(&startTime); // Start time for this session
    startTime -= s.elapsedTime;
    int coords[4];
    int isOk, lastScore=*pScore;
    char c;

    while(1){
        // Print Board and prompt for command
        printBoard(s, board);
        printf("Commands:\nc - Enter Coordinates\ns - Save\nq - Quit\n");
        printf("Enter Command: ");
        scanf(" %c", &c);

        // Check the command
        if(c=='c'){ // Enter Coordinates
            printf("Enter Coordinates: ");
            scanf("%i%i%i%i", &coords[1], &coords[0], &coords[3], &coords[2]);
            printf("\n");
            // Check if that is an okay move
            isOk = okMove(s, board, coords);
            if(isOk == 0){
                printf("Invalid Move!\n");
                checkTime(&s, startTime);
                printf("Elapsed Time: %li\n", s.elapsedTime);
                continue;
            }else if(isOk == -1){
                printf("Tiles not adjacent! Try again.\n");
                checkTime(&s, startTime);
                printf("Elapsed Time: %li\n", s.elapsedTime);
                continue;
            }
            // if is is, continue, if not, start loop again
            while(checkBoard(s, board)==1){
                removeMatches(s, board);
                calcScore(s, board, pScore);
                gravity(s, board);
                repopulate(s, board, symbols);
            }
            checkTime(&s, startTime);
            printf("Score Gain: %i\nTotal: %i\nElapsed Time: %li seconds\n", 
            *pScore-lastScore, *pScore, s.elapsedTime);
            lastScore = *pScore;
        }else if (c=='q'){ // Quit
            checkTime(&s, startTime);
            printf("\n\nGame Over!\nPoints Earned: %i\nElapsed Time: %li\nScore: %li Points\n\n", *pScore, s.elapsedTime, (*pScore/s.elapsedTime));
            break;
        }else if (c=='s'){ // Save Game
            if(saveGame(s, *pScore, board, symbols) == 1){
                printf("Save Successful\n");
            }else{
                printf("Save was not successful.\n");
            }
        }else{
            printf("Invalid Command!\n");
        }
    }
}

int mainMenu(setupData *s){
    printf("********************\n");
    printf("**     Jumble     **\n");
    printf("********************\n");

    while(1){ // Loop for determining new game or load game
        char nl;
        printf("New Game / Load Game\n(N/L): ");
        scanf(" %c", &nl);

        if(nl == 'l' || nl == 'L'){
            return 1;
        }else if(nl == 'n' || nl == 'N'){
            break;
        }else{
            printf("\n~~~~~~~~~~~~~~~~~~~~\n");
            printf("    Invalid Input   \n");
            printf("~~~~~~~~~~~~~~~~~~~~\n");
        }
    }

    printf("\n~~~~~~~~~~~~~~~~~~~~\n");
    printf("      NEW GAME      ");
    printf("\n~~~~~~~~~~~~~~~~~~~~\n");

    while(1){ // Loop for setting board size in setupData
        int size;

        printf("Board Size (Minimum 3): ");
        scanf("%i", &size);
        s->boardSize=size;
        if(s->boardSize>=BOARDMIN){
            break;
        }else{
            printf("!!!Invalid  Input!!!\n");
        }
        
    }
    
    while(1){
        int amount;
        printf("_Amount of Symbols_\n");
        printf("(3-9): ");

        scanf("%i", &amount);
        s->symbolCount = amount;
        if(s->symbolCount >= 3 && s->symbolCount <= 9){
            break;
        }else{
            printf("!!!Invalid  Input!!!\n");
        }
    }
    printf("\n");
    return 0;
}

void generateBoard(setupData s, char board[][s.boardSize],char symbols[s.symbolCount]){
    int i, j;
    for(i=0;i<s.boardSize;i++){
        for(j=0;j<s.boardSize;j++){
            board[j][i] = symbols[rand() % s.symbolCount];
        }
    }

    // Removes any matches and repopulates until a board with no matches is generated
    while(checkBoard(s, board)==1){
        removeMatches(s, board);
        gravity(s, board);
        repopulate(s, board, symbols);
    }
}

void printBoard(setupData s, char b[][s.boardSize]){ 
    int i,j;
    // Two cases: Board optimised for 3-9 rows/cols OR board for up to 99.

    if(s.boardSize<10){
        printf(" 0 |");
        for(i=0;i<s.boardSize;i++){ 
            printf(" %i |", i+1);
        }
        printf("\n");
        printf("----");
        for(j=0;j<s.boardSize;j++){
            printf("----");
        }
        printf("\n");

        // Board data printing
        for(i=0;i<s.boardSize;i++){
            printf(" %d |",i+1); // Y coordinate
            for(j=0;j<s.boardSize;j++){
                printf(" %c |", b[i][j]);
            }
            printf("\n");
            printf("----");
            for(j=0;j<s.boardSize;j++){
                printf("----");
            }
            printf("\n");
        }
    }else{
        int x=2;
        printf("%*d |", x, 0);
        for(i=0;i<s.boardSize;i++){ 
            printf("%*d |",x, i+1);
        }
        printf("\n");
        printf("----");
        for(j=0;j<s.boardSize;j++){
            printf("----");
        }
        printf("\n");

        // Board data printing
        for(i=0;i<s.boardSize;i++){
            printf("%*d |",x, i+1); // Y coordinate
            for(j=0;j<s.boardSize;j++){
                printf(" %c |", b[i][j]);
            }
            printf("\n");
            printf("----");
            for(j=0;j<s.boardSize;j++){
                printf("----");
            }
            printf("\n");
        }
    }
}

void genSymbolPool(setupData s, char symPool[9], char sym[s.symbolCount]){
    srand(time(0));
    for(int i=0;i<s.symbolCount;i++){ // Main loop
        int isUnique; // Unique flag
        while(1){
            int randNum = rand() % 9; // Generates rand num 0-8

            // Duplicate check. Reset loop if duplicate is found.
            // Assign symbol and move to next index if unique.
            for(int j=0;j<s.symbolCount;j++){ 
                if(sym[j]==symPool[randNum]){
                    isUnique=0;
                    break;
                }else{
                    isUnique=1;
                }
            }
            if(isUnique==1){
                sym[i]=symPool[randNum];
                break;
            }
        }
    }
    // Shows the user what symbols are in their game
    printf("Symbol Pool: ");
    for(int i=0;i<s.symbolCount;i++){
        printf("%c", sym[i]);
    }
    printf("\n\n");
}

void removeMatches(setupData s, char board[][s.boardSize]){
    int i, j, k, combo=1, indexSave;
    char tempBoard[s.boardSize][s.boardSize];

    for(i=0;i<s.boardSize;i++){
        for(j=0;j<s.boardSize;j++){
            tempBoard[i][j]='o';
        }
    }

    // Horizontal
    for(i=0;i<s.boardSize;i++){ // down
        for(j=0;j<(s.boardSize);j++){ // right
            if(j == s.boardSize-1){ // LAST CELL
                if(combo>=3){ // If combo exists
                    for(k=0;k<combo;k++){ 
                        tempBoard[i][indexSave+k]='d'; 
                    }
                    combo=1;
                    continue;
                }else{ // Combo ends and no match
                    combo=1;
                    indexSave=-10;
                    continue;
                }
            }

            if(board[i][j]==board[i][j+1]){ // If index is the same as the space next to it
                if(combo==1){ indexSave=j; } // save index of new combo
                combo++;    // add to counter
            }else if(combo>=3){ // If the combo ends and there's a match
                for(k=0;k<combo;k++){ 
                    tempBoard[i][indexSave+k]='d'; 
                }
                combo=1;
            }else{ // Combo ends and no match
                combo=1;
                indexSave=-10;
            }
        }
    }
    
    // Vertical
    combo=1, indexSave=-1;
    for(j=0;j<s.boardSize;j++){ // right
        for(i=0;i<(s.boardSize);i++){ // down
            if(i == s.boardSize-1){ // LAST CELL
                if(combo>=3){ // If the combo ends and there's a match
                    for(k=0;k<combo;k++){ 
                        tempBoard[indexSave+k][j]='d';
                    }
                    combo=1;
                    continue;
                }else{ // Combo ends and no match
                    combo=1;
                    indexSave=-10;
                    continue;
                }
            }

            if(board[i][j]==board[i+1][j]){ // If index is the same as the space next to it
                if(combo==1){ // save index of new combo
                    indexSave=i;
                } 
                combo++;    // add to counter
            }else if(combo>=3){ // If the combo ends and there's a match
                for(k=0;k<combo;k++){ 
                    tempBoard[indexSave+k][j]='d';
                }
                combo=1;
            }else{ // Combo ends and no match
                combo=1;
                indexSave=-10;
            }
        }
    }

    // Replaces tagged cells with an empty space char for other functions.
    for(i=0;i<s.boardSize;i++){
        for(j=0;j<s.boardSize;j++){
            if(tempBoard[i][j]=='d'){
                board[i][j]=' ';
            }
        }
    }
}

void loadGame(int *pScore){
    FILE *fp; // File pointer
    setupData s; // Creates new setupdata var to pass into gameloop
    int i, j;
    fp=fopen("save.txt", "r");
    if(fp==NULL){ // Null check
        printf("Unabe to load file.\n");
    }else{
        fscanf(fp, " %i%i%li%i", &s.boardSize, &s.symbolCount, &s.elapsedTime, pScore); // first 3 values are setupdata. score is a pointer.
        char loadedSymbols[s.symbolCount];
        fscanf(fp, " %s", loadedSymbols); // board data in in a single string.
        char boardString[s.boardSize * s.boardSize];
        char loadedBoard[s.boardSize][s.boardSize];
        fscanf(fp, " %s", boardString);
        printf("\nGame Loaded!\nBoardsize: %i\nSymbol Count: %i\nElapsed Time: %li\n", s.boardSize, s.symbolCount, s.elapsedTime);
        printf("Points: %i\n\n", *pScore);
        fclose(fp);

        for(i=0;i<s.boardSize;i++){ // turn the string into a 2D array for the gameloop.
            for(j=0;j<s.boardSize;j++){
                loadedBoard[i][j]=boardString[(s.boardSize*i)+j];
            }
        }
        gameLoop(s, loadedBoard, loadedSymbols, pScore); // data passed into game loop.
    }
}

void gravity(setupData s, char board[][s.boardSize]){
    int i, j;
    int empty=0, tempCounter=0;

    for(j=0;j<s.boardSize;j++){ // Right
        empty=0, tempCounter=0;
        for(i=0;i<s.boardSize;i++){ // counts empty cells
            if(board[i][j]==' '){ 
                empty++;
            }
        }

        while(1){
            for(i=(s.boardSize-1);i>0;i--){ // Sort Loop
                if(board[i][j]==' '){
                    board[i][j]=board[i-1][j];
                    board[i-1][j]=' ';
                }
            }
            
            tempCounter=0;
            for(i=0;i<s.boardSize;i++){ // Check Loop
                if(board[i][j]==' '){
                    tempCounter++;
                }else{
                    break;
                }
            }
         
            if(tempCounter==empty){
                break;
            }
        }
    }
}

int okMove(setupData s, char board[][s.boardSize], int coords[4]){
    char hold;
    char testBoard[s.boardSize][s.boardSize];
    int i, j;

    // xy check
    while(1){
        if(coords[0]==coords[2]){
            if(coords[1]==coords[3]+1 || coords[1]==coords[3]-1){
                break;
            }
        }
        if(coords[1]==coords[3]){
            if(coords[0]==coords[2]+1 || coords[0]==coords[2]-1){
                break;
            }
        }
        return -1;
    }

    // make test board
    for(i=0;i<s.boardSize;i++){
        for(j=0;j<s.boardSize;j++){
            testBoard[i][j]=board[i][j];
        }
    }

    for(i=0;i<4;i++){ coords[i]=coords[i]-1; } // User perceives coords as +1 because arrays start at 0.

    hold = board[coords[0]][coords[1]];

    testBoard[coords[0]][coords[1]] = testBoard[coords[2]][coords[3]];
    testBoard[coords[2]][coords[3]] = hold;
    
    if(checkBoard(s, testBoard)==1){
        for(i=0;i<s.boardSize;i++){
            for(j=0;j<s.boardSize;j++){
                board[i][j]=testBoard[i][j];
            }
        }
        return 1;
    }else{
        return 0;
    }
}

void repopulate(setupData s, char b[][s.boardSize], char symbols[s.symbolCount]){
    int i, j;
    for(i=0;i<s.boardSize;i++){
        for(j=0;j<s.boardSize;j++){
            if(b[i][j]==' '){
                b[i][j]= symbols[rand() % s.symbolCount];
            }
        }
    }
}

int checkBoard(setupData s, char board[][s.boardSize]){
    int i, j, combo=1;

    // Horizontal
    for(i=0;i<s.boardSize;i++){ // down
        for(j=0;j<(s.boardSize);j++){ // right
            if(j == s.boardSize-1){ // if last cell
                if(combo>=3){ // If combo exists
                    return 1;
                }else{ // Combo ends and no match
                    combo=1;
                    continue;
                }
            }

            if(board[i][j]==board[i][j+1]){ // If index is the same as the space next to it
                combo++;    // add to counter
            }else if(combo>=3){ // If the combo ends and there's a match
                return 1;
            }else{ // Combo ends and no match
                combo=1;
            }
        }
    }

    // Vertical
    combo=1;
    for(j=0;j<s.boardSize;j++){ // right
        for(i=0;i<(s.boardSize);i++){ // down
            if(i == s.boardSize-1){
                if(combo>=3){ // If the combo ends and there's a match
                    return 1;
                }else{ // Combo ends and no match
                    combo=1;
                }
            }

            if(board[i][j]==board[i+1][j]){ // If index is the same as the space next to it
                combo++;    // add to counter
            }else if(combo>=3){ // If the combo ends and there's a match
                return 1;
            }else{ // Combo ends and no match
                combo=1;
            }
        }
    }
    return 0;
}

void calcScore(setupData s, char board[][s.boardSize], int *pScore){
    int i,j;
    // count empty
    for(i=0;i<s.boardSize;i++){
        for(j=0;j<s.boardSize;j++){
            if(board[i][j] == ' '){
                *pScore+=10;
            }
        }
    }
}

void checkTime(setupData *s, time_t start){
    time_t now;
    time(&now);
    s->elapsedTime = (now-start);
}

int saveGame(setupData s, int score, char b[][s.boardSize], char symbols[s.symbolCount]){
    int i,j;
    FILE *fp;

    fp=fopen("save.txt", "w");

    fprintf(fp, "%i " ,s.boardSize);
    fprintf(fp, "%i ", s.symbolCount);
    fprintf(fp, "%li ", s.elapsedTime);
    fprintf(fp, "%i ", score);
    
    for(i=0;i<s.symbolCount;i++){
        fprintf(fp, "%c" ,symbols[i]);
    }

    fprintf(fp, "%c", ' ');

    for(i=0;i<s.boardSize;i++){
        for(j=0;j<s.boardSize;j++){
            fprintf(fp, "%c" ,b[i][j]);
        }
    }

    fclose(fp);

    return 1;
}