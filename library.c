#include "library.h"
#include "smallchesslib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int main(){
    testFunctions();
    return 0;
}

char* getBoard(const char* PGN, int N){
    char* solution = (char*)malloc(70 * sizeof(char));
    strcpy(solution,"");
    SCL_Record record;
    char* firstMoves = getFirstMoves(PGN,N);
    SCL_recordFromPGN(record,firstMoves);
    SCL_Board board;
    SCL_boardInit(board);
    uint16_t halfMoves = (uint16_t)N;
    SCL_recordApply(record, board, halfMoves);
    int t = SCL_boardToFEN(board, solution);
    char buffer[2] = "0";
    sprintf(buffer, "%d", (N%2));
    const char* halfMove = buffer;
    solution[(t-4)] = *halfMove;
    free(firstMoves);
    return solution;
}

/*TODO handle case when half moves asked exceeds the ones played
 *TODO handle 0 half moves
 *TODO handle wrong input i.e. -1...*/
const char* getFirstMoves(const char* PGN, int N){
    int hMovesRemaining = N % 2;
    int fullMoves = (N/2)+hMovesRemaining;
    if(hMovesRemaining == 0){hMovesRemaining=2;}

    size_t length = strlen(PGN);
    char PGNcopy[length];
    strcpy(PGNcopy,PGN);

    char firstMoves[N+fullMoves][7];
    char* firstMovesSolution = (char*)malloc(((N+fullMoves)*7) * sizeof(char));
    strcpy(firstMovesSolution,"");

    const char* separators = " .";
    char strFullMoves[10];
    sprintf(strFullMoves, "%d", fullMoves);

    char* strToken = strtok(PGNcopy, separators);
    int finalMove = strcmp(strToken, strFullMoves);

    char period[] = ".";
    char space[] = " ";
    int count = 0;
    while(strToken != NULL && hMovesRemaining > 0){
        strcpy(firstMoves[count],strToken);//Move number
        strcat(firstMoves[count], period);

        strToken = strtok(NULL, separators);
        strcpy(firstMoves[count+1],strToken);//First half move
        strcat(firstMoves[count+1], space);
        if(finalMove == 0){
            hMovesRemaining -= 1;
        }

        if(hMovesRemaining >= 1) {
            strToken = strtok(NULL, separators);
            strcpy(firstMoves[count + 2], strToken);//Second half move
            strcat(firstMoves[count + 2], space);

            if(finalMove == 0){
                hMovesRemaining -= 1;
            }else {
                strToken = strtok(NULL, separators);//Next Move number
                finalMove = strcmp(strToken, strFullMoves);
            }
        }
        count += 3;
    };
    for(int i = 0;i<N+fullMoves;i++){
        strcat(firstMovesSolution,firstMoves[i]);
    };
    return firstMovesSolution;
}
//TODO do not allow an number of half moves greater that what we have
/*TODO Function can be greatly optimized by registering and playing every movement one
 * at a time instead of recreating the board for the PGN notation for every half moves
 * until we arrive at N*/
bool hasBoard(const char* PGN, const char* FEN, int N){
    bool solution = true;
    int countHalfMoves = 1;
    int found = 1;
    while(countHalfMoves<N+1 && found != 0){
        char* tempFirstMovesBoard = getBoard(PGN, countHalfMoves);
        found = strcmp(FEN, tempFirstMovesBoard);
        countHalfMoves += 1;
        free(tempFirstMovesBoard);
    }
    if(countHalfMoves == N+1 && found != 0){
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


    printf("\n\nhasBoard :\n");
    //TrueFEN is the board state for the fifth half move.
    char TrueFEN[] = "rnbqkb1r/pppppp1p/5n2/6P1/8/5N2/PPPPPPP1/RNBQKB1R b KQkq - 1 3";
    //FalseFEN is impossible.(always false)
    char FalseFEN[] = "rnbqkb1r/pppppp1p/5n2/6P1/8/5N2/1PP1PPP1/RNBQKB1R b KQkq - 1 3";
    hMoves = 8;
    bool temp3 = hasBoard(PGN,TrueFEN,hMoves);
    bool temp4 = hasBoard(PGN,FalseFEN,hMoves);
    printf("half moves : %d\ninput : %s\noutput : \n%s for %s\n%s for %s",hMoves,PGN,temp3 ? "true" : "false",TrueFEN,temp4 ? "true" : "false",FalseFEN);
}