#include "library.h"
#include "smallchesslib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    testFunctions();
    return 0;
}

char* getBoard(const char* PGN, int N){
    char* solution = (char*)malloc(70 * sizeof(char));//allocation of size 70 char(FEN notation does not exceed 70 character)
    strcpy(solution,"");//Copy empty stream into the char* to remove unwanted artifacts
    SCL_Record record;//Hold record of the game
    char* firstMoves = getFirstMoves(PGN,N);
    SCL_recordFromPGN(record,firstMoves);
    SCL_Board board;//Holds state of the board
    SCL_boardInit(board);
    uint16_t halfMoves = (uint16_t)N;
    SCL_recordApply(record, board, halfMoves);//Applies the record of moves one after another on a board
    int t = SCL_boardToFEN(board, solution);//Get the FEN notation as a string of the board state and returns the size of the string

    char buffer[2] = "0";                        //Fix for the half move
    sprintf(buffer, "%d", N);       //that is not taken into
    const char* halfMove = buffer;               //account by the smallchesslib
    solution[(t-4)] = *halfMove;                 //librarie.

    free(firstMoves);
    return solution;
}

const char* getFirstMoves(const char* PGN, int N){
    if(N<0){N=0;}//Negative numbers do not make sense
    int hMovesRemaining = N % 2;
    int fullMoves = (N / 2) + hMovesRemaining;//Moves to take into account
    if (hMovesRemaining == 0) { hMovesRemaining = 2; }//Number of half moves to take into account for the last move to account for

    char firstMoves[N+fullMoves][7];//list of moves (Move #, half move white, half move black)
    char* firstMovesSolution = (char*)malloc(((N+fullMoves)*7) * sizeof(char));
    strcpy(firstMovesSolution, "");//Copy empty stream into the char* to remove unwanted artifacts

    if(N>0) {//only if we need at least one half-move
        size_t length = strlen(PGN);
        char PGNcopy[length];
        strcpy(PGNcopy, PGN);//copy the const PGN so whe can modify it

        const char *separators = " .";//separators are " " and ".", so the notation accepted are {1. e3 E5 2. a3 ...} and {1.e3 E5 2.a3 ...}
        char strFullMoves[10];
        sprintf(strFullMoves, "%d", fullMoves);//string containing the move where we are stopping, converted to string

        char *strToken = strtok(PGNcopy, separators);//recuperating the first character before a point or a space
        int finalMove = strcmp(strToken, strFullMoves);

        char period[] = ".";
        char space[] = " ";
        int count = 0;
        while (strToken != NULL &&
               hMovesRemaining > 0) {//stop when we read all the PGN or we read all the half moves we needed
            strcpy(firstMoves[count], strToken);//adding Move number to the list
            strcat(firstMoves[count], period);//adding point after the move number to return in PGN notation

            strToken = strtok(NULL, separators);//recuperating the first half move
            if (strToken != NULL) {
                count += 1;
                strcpy(firstMoves[count], strToken);//adding First half move to the list
                strcat(firstMoves[count], space);//adding space between two half moves
                if (finalMove == 0) {//if we have arrived at the last move
                    hMovesRemaining -= 1;//remove a half move
                }

                if (hMovesRemaining >= 1) {//verify that we still have half moves left to copy
                    strToken = strtok(NULL, separators);//recuperating the second half move
                    if (strToken != NULL) {
                        count += 1;
                        strcpy(firstMoves[count], strToken);//adding Second half move to the list
                        strcat(firstMoves[count], space);//adding space between second half move and next move number

                        if (finalMove == 0) {//if we have arrived at the last move
                            hMovesRemaining -= 1;//remove a half move
                        } else {
                            strToken = strtok(NULL, separators);//recuperating the next Move number
                            finalMove = strcmp(strToken,
                                               strFullMoves);//compare the next move number with the one where we should stop
                            count += 1;//+3 in total because we have (move number, first half move, second half move)
                        }
                    }
                }
            }
        }
        for (int i = 0; i < count + 1; i++) {//iteration to concatenate the list of move numbers and half moves into a single string
            strcat(firstMovesSolution, firstMoves[i]);
        }
    }
    return firstMovesSolution;
}

bool hasOpening(const char* PGNone, const char* PGNtwo){
    bool solution = true;

    size_t lengthOne = strlen(PGNone);
    char PGNoneCopy[lengthOne+1];
    strcpy(PGNoneCopy, PGNone);//copy the const PGN so whe can modify it
    char* pointerOne = NULL;

    size_t lengthTwo = strlen(PGNtwo);
    char PGNtwoCopy[lengthTwo+1];
    strcpy(PGNtwoCopy, PGNtwo);//copy the const PGN so whe can modify it
    char* pointerTwo = NULL;

    const char *separators = " .";//separators are " " and ".", so the notation accepted are {1. e3 E5 2. a3 ...} and {1.e3 E5 2.a3 ...}

    char *strTokenOne = strtok_r(PGNoneCopy, separators,&pointerOne);//recuperating the first character before a point or a space
    char *strTokenTwo = strtok_r(PGNtwoCopy, separators,&pointerTwo);//recuperating the first character before a point or a space
    while (strTokenOne != NULL && strTokenTwo != NULL && solution) {//stop when we read all the PGN or we read all the half moves we needed
        if(strcmp(strTokenOne, strTokenTwo) != 0) {//compare the next move number with the one where we should stop
            solution = false;
        }
        strTokenOne = strtok_r(NULL, separators, &pointerOne);//recuperating the next character before a point or a space
        strTokenTwo = strtok_r(NULL, separators, &pointerTwo);//recuperating the next character before a point or a space
    }
    if(strTokenOne == NULL && strTokenTwo != NULL && solution){//if the chess game we are looking into finishes before the reference chess game
        solution = false;
    }
    return solution;
}



/*TODO Function can be greatly optimized by registering and playing every movement one
 * at a time instead of recreating the board for the PGN notation for every half moves
 * until we arrive at N*/
bool hasBoard(const char* PGN, const char* FEN, int N){
    bool solution = true;
    int countHalfMoves = 0;//start the half moves counter at one
    int found = 1;//found the game state is false by default
    while((countHalfMoves<N+1 && found != 0)){//stop when we have verified all the half moves or if we find the board state
        char* tempFirstMovesBoard = getBoard(PGN, countHalfMoves);//getting the FEN of the board state for every half move played
        found = strcmp(FEN, tempFirstMovesBoard);//compare the board state every time a move is played
        countHalfMoves += 1;
        free(tempFirstMovesBoard);
    }
    if((countHalfMoves == N+1 && found != 0) || N<0){//verify if we have found the board state or if we have finished iterating without finding anything
        solution = false;
    }
    return solution;
}


void testFunctions(){
    char PGN[] = "1. h4 g5 2. hxg5 Nf6 3. Nf3 Bg7 4. e3 O-O 5. Nc3 c5 "
                 "6. d3 c4 7. b4 cxb3 8. Ba3 Qc7 9. Qd2 Nc6 10. O-O-O Nd5 "
                 "11. cxb3 Ncb4 12. g6 Qxc3+ 13. Qc2 d6 14. gxh7+ Kh8 "
                 "15. Qxc3 Bxc3 16. Bxb4 Kg7 17. h8=R Nxb4 18. Be2 Kf6 "
                 "19. Rhf1 Kg7 20. Rfh1 Na6 21. R1h4*";


    printf("getBoard :\n");
    int hMoves = 5;
    char* temp1 = getBoard(PGN,hMoves);
    printf("half moves : %d\ninput : %s\noutput : %s\n",hMoves,PGN,temp1);
    free(temp1);


    printf("\n\ngetFirstMoves :\n");
    hMoves = 8;
    char* temp2 = getFirstMoves(PGN,hMoves);
    printf("half moves : %d\ninput : %s\noutput : %s\n",hMoves,PGN,temp2);
    free(temp2);

    printf("\n\nhasOpening :\n");
    //TruePGN is the chess game until the ninth half-move.
    char SecondTruePGN[] = "1.h4 g5 2.hxg5 Nf6 3.Nf3 Bg7 4.e3 O-O 5.Nc3 ";
    //FalsePGN is impossible.(always false)
    char SecondFalsePGN[] = "1. h4 g5 2. hxg5 Nf6 3. Nf3 Bg7 4. e3 O-O 5. Nc3 c5 "
                          "6. d3 c4 7. b4 cxb3 8. Ba3 Qc7 9. Qd2 Nc6 10. O-O-O Nd5 "
                          "11. cxb3 Ncb4 12. g6 Qxc3+ 13. Qc2 d6 14. gxh7+ Kh8 "
                          "15. Qxc3 Bxc3 16. Bxb4 Kg7 17. h8=R Nxb4 18. Be2 Kf6 "
                          "19. Rhf1 Kg7 20. Rfh1 Na6 21. R1h4* a6";
    bool temp3 = hasOpening(PGN,SecondTruePGN);
    bool temp4 = hasOpening(PGN,SecondFalsePGN);
    printf("input : %s\noutput : \n%s for %s\n%s for %s",PGN,temp3 ? "true" : "false",SecondTruePGN,temp4 ? "true" : "false",SecondFalsePGN);


    printf("\n\nhasBoard :\n");
    //TrueFEN is the board state for the fifth half move.
    char TrueFEN[] = "rnbqkb1r/pppppp1p/5n2/6P1/8/5N2/PPPPPPP1/RNBQKB1R b KQkq - 5 3";
    //FalseFEN is impossible.(always false)
    char FalseFEN[] = "rnbqkb1r/pppppp1p/5n2/6P1/8/5N2/1PP1PPP1/RNBQKB1R b KQkq - 1 3";
    hMoves = 490;
    bool temp5 = hasBoard(PGN,TrueFEN,hMoves);
    bool temp6 = hasBoard(PGN,FalseFEN,hMoves);
    printf("half moves : %d\ninput : %s\noutput : \n%s for %s\n%s for %s",hMoves,PGN,temp5 ? "true" : "false",TrueFEN,temp6 ? "true" : "false",FalseFEN);
}