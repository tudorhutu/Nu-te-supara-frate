#ifndef GAMELOGIC_H
#define GAMELOGIC_H

class Pawn
{
public:
    int distance_traveled;
    int position;
    int final_positon;
};

class GameBoard
{

public:
    int leaderboard[4] = {0, 0, 0, 0};
    // int leaderboard[4] = {0, 1, 0, 0};
    int board[47]; //  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0};
    int end[4][4];
    // int end[4][4] = {
    //     {0, 12, 13, 14},
    //     {21, 22, 23, 24},
    //     {0, 0, 0, 0},
    //     {0, 0, 0, 0},
    // };
    Pawn pawns[4][4]; //player number, pawn number
    int start[4][4] = {
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1},
    };
    // int start[4][4] = {
    //     {0, 0, 0, 0},
    //     {0, 0, 0, 0},
    //     {0, 0, 0, 0},
    //     {0, 0, 0, 0},
    // };
    GameBoard()
    {
        //real constructor
        for (int i = 0; i < 47; i++)
            board[i] = 0;

        // pawns[0][0].distance_traveled = pawns[0][0].position = 45;
    }
    int diceRoll()
    {
        return rand() % (6) + 1;
    }

    int makeMove(int player_number, int pawn_number, int vallue_rolled)
    {
        player_number++;

        for (int i = 0; i < 4; i++) //can't move a pawn if it's in end
            for (int j = 0; j < 4; j++)
            {
                if (end[i][j] == player_number * 10 + pawn_number)
                {
                    std::cout << "pawn already in end" << std::endl;
                    return 0;
                }
            }

        if (start[player_number - 1][pawn_number - 1] == 1) //can't move a pawn from start if it's not a 6
            if (vallue_rolled < 6)
            {
                printf("Can't move pawn from start if the value rolled isn't a 6\n");
                return 0;
            }
            else
                start[player_number - 1][pawn_number - 1] = 0;                                       //if we move the pawn, we remove it from the starting area
        if (46 - (pawns[player_number - 1][pawn_number - 1].distance_traveled + vallue_rolled) >= 0) //before reaching the last 4 spaces or any space that can reach them
        {
            if (board[pawns[player_number - 1][pawn_number - 1].position + vallue_rolled] != 0) //if the space the pawn is going to move in is not empty
            {
                if (board[pawns[player_number - 1][pawn_number - 1].position + vallue_rolled] / 10 == player_number) //if it's the same player's, skip a space forward
                {
                    vallue_rolled++;
                }
                else //if it's another player's send the pawn to start
                {

                    int pawn_to_remove_number = board[pawns[player_number - 1][pawn_number - 1].position + vallue_rolled] % 10;
                    int pawn_to_remove_player = board[pawns[player_number - 1][pawn_number - 1].position + vallue_rolled] / 10;
                    pawns[pawn_to_remove_player][pawn_to_remove_number].distance_traveled = 0;
                    pawns[pawn_to_remove_player][pawn_to_remove_number].position = 0;
                    start[pawn_to_remove_player - 1][pawn_to_remove_number - 1] = 1;
                    printf("\n");
                    printf("%d\n", start[pawn_to_remove_player - 1][pawn_to_remove_number - 1]);
                    printf("%d      %d\n", pawn_to_remove_number, pawn_to_remove_player);
                    board[pawns[player_number - 1][pawn_number - 1].position + vallue_rolled] = 0;
                }
            }

            pawns[player_number - 1][pawn_number - 1].distance_traveled += vallue_rolled; //we change the distance based on the distance rolled
            board[pawns[player_number - 1][pawn_number - 1].position] = 0;
            pawns[player_number - 1][pawn_number - 1].position += vallue_rolled;                          // we change the stored pawn position
            board[pawns[player_number - 1][pawn_number - 1].position] = player_number * 10 + pawn_number; //we move the pawn on the actual board
            printf("Current board:\n");
            for (int i = 6; i < 47; i++)
                printf("%d ", board[i]);
            printf("\n");
            printf("\n");
            printf("Starting positions:\n");
            for (int i = 0; i < 4; i++)
            {
                printf("player %d ", i + 1);
                for (int j = 0; j < 4; j++)
                    printf("%d", start[i][j]);
                printf("\n");
            }
            printf("%d\n", pawns[player_number - 1][pawn_number - 1].distance_traveled);
            printf("%d spaces until the end\n", 46 - pawns[player_number - 1][pawn_number - 1].distance_traveled);
            printf("Ending positions:\n");
            for (int i = 0; i < 4; i++)
            {
                printf("player %d ", i + 1);
                for (int j = 0; j < 4; j++)
                    printf("%d", end[i][j]);
                printf("\n");
            }
        }
        else
        {
            printf("trying to see if this pawn will win, and the distance traveled is %d, with the previous distance traveled %d\n", pawns[player_number - 1][pawn_number - 1].distance_traveled, pawns[player_number - 1][pawn_number - 1].distance_traveled - vallue_rolled);
            if (50 - pawns[player_number - 1][pawn_number - 1].distance_traveled > 0 && end[player_number - 1][vallue_rolled + pawns[player_number - 1][pawn_number - 1].distance_traveled - 46 - 1] == 0 && vallue_rolled + pawns[player_number - 1][pawn_number - 1].distance_traveled - 46 <= 4) //if a roll doesn't exceed the ending spaces and if said ending space is free, remove pawn from board and add it into ending position
            {
                board[pawns[player_number - 1][pawn_number - 1].position] = 0;
                int positin_in_end = vallue_rolled + pawns[player_number - 1][pawn_number - 1].distance_traveled - 46;
                printf("The positin in end will be %d of player %d\n", positin_in_end, player_number);
                end[player_number - 1][positin_in_end - 1] = player_number * 10 + pawn_number;
                printf("Ending positions:\n");
                for (int i = 0; i < 4; i++)
                {
                    printf("player %d ", i + 1);
                    for (int j = 0; j < 4; j++)
                        printf("%d ", end[i][j]);
                    printf("\n");
                }
            }
            else
            {
                std::cout << player_number - 1 << std::endl;
                std::cout << end[player_number - 1][(vallue_rolled + pawns[player_number - 1][pawn_number - 1].distance_traveled) - 46 - 1] << std::endl;
                printf("rolled too much\n");
                pawns[player_number][pawn_number - 1].distance_traveled = pawns[player_number][pawn_number - 1].distance_traveled - vallue_rolled;
                pawns[player_number][pawn_number - 1].position = pawns[player_number][pawn_number - 1].position - vallue_rolled;
                printf("Ending positions:\n");
                for (int i = 0; i < 4; i++)
                {
                    printf("player %d ", i + 1);
                    for (int j = 0; j < 4; j++)
                        printf("%d ", end[i][j]);
                    printf("\n");
                }
                return 0;
            }

            //when reaching the last 4 spaces
        }
        return vallue_rolled;
    }
};

#endif