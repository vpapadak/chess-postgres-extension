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
    SCL_recordFromPGN(record,getFirstMoves(PGN,N));
    SCL_Board board;
    SCL_boardInit(board);
    uint16_t halfMoves = (uint16_t)N;
    SCL_recordApply(record, board, halfMoves);
    int t = SCL_boardToFEN(board, solution);
    char buffer[2] = "0";
    sprintf(buffer, "%d", (N%2));
    const char* halfMove = buffer;
    solution[(t-4)] = *halfMove;
    return solution;
}

/*TO DO handle case when half moves asked exceeds the ones played
 * TO DO handle 0 half moves
 * TO DO handle wrong input i.e. -1...*/
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
}