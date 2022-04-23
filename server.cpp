/* servTCPConcTh2.c - Exemplu de server TCP concurent care deserveste clientii
   prin crearea unui thread pentru fiecare client.
   Asteapta un numar de la clienti si intoarce clientilor numarul incrementat.
	Intoarce corect identificatorul din program al thread-ului.
  
   
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

//g++ server.cpp -pthread  -lpqxx -lpq  -o serv.out

#include <sys/types.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pqxx/pqxx>
#include <stdlib.h>
#include <signal.h>
#include <string>
#include <pthread.h>
#include "gamelogic.h"
using namespace pqxx;
using namespace std;

/* portul folosit */
#define PORT 2907

/* codul de eroare returnat de anumite apeluri */
//extern int errno;

typedef struct thData
{
  int idThread; //id-ul thread-ului tinut in evidenta de acest program
  int cl;       //descriptorul intors de accept
} thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);
int connnumber = 0, saved_game_players = 2;
int nrplayer = 0, testboard[50], teststart[4][4], testend[9][9], testleaderboard[4], testnrplayer, test_saved_game_players = 3;
int databaseid = 0;
bool startgame = false, loadgame = false;
GameBoard *playing_board = new GameBoard;

void create_table()
{
  string sql;
  try
  {
    connection C("dbname = th user = postgres password = myPassword \
      hostaddr = 127.0.0.1 port = 5432");
    if (C.is_open())
    {
      cout << "Opened database successfully: " << C.dbname() << endl;
    }
    else
    {
      cout << "Can't open database" << endl;
      return;
    }

    //instruction to create table(no need to use more than once)

    sql = "CREATE TABLE save_states("
          "ID INT PRIMARY KEY     NOT NULL,"
          "LASTPLAYER            INT     NOT NULL,"
          "ACTIVEPLAYERS            INT     NOT NULL,"
          "LEADERBOARD           TEXT    NOT NULL,"
          "STARTAREA           TEXT    NOT NULL,"
          "BOARD          TEXT    NOT NULL,"
          "ENDAREA           TEXT    NOT NULL );";

    /* Create a transactional object. */
    work W(C);
    /* Execute SQL query */
    W.exec(sql);
    W.commit();
    // cout << "Table created successfully" << endl;
  }
  catch (const std::exception &e)
  {
    // cerr << e.what() << std::endl;
    return;
  }
}

void connect_to_database()
{

  string sql;

  try
  {
    connection C("dbname = th user = postgres password = myPassword \
      hostaddr = 127.0.0.1 port = 5432");
    if (C.is_open())
    {
      cout << "Opened database successfully: " << C.dbname() << endl;
    }
    else
    {
      cout << "Can't open database" << endl;
      return;
    }

    //instruction to create table(no need to use more than once)

    // sql = "CREATE TABLE save_states("
    //       "ID INT PRIMARY KEY     NOT NULL,"
    //       "LASTPLAYER            INT     NOT NULL,"
    //       "ACTIVEPLAYERS            INT     NOT NULL,"
    //       "LEADERBOARD           TEXT    NOT NULL,"
    //       "STARTAREA           TEXT    NOT NULL,"
    //       "BOARD          TEXT    NOT NULL,"
    //       "ENDAREA           TEXT    NOT NULL );";

    // /* Create a transactional object. */
    // work W(C);
    // /* Execute SQL query */
    // W.exec(sql);
    // W.commit();
    // // cout << "Table created successfully" << endl;

    sql = "SELECT ID from save_states ORDER BY ID DESC"
          " LIMIT 1 ";
    // /* Create a non-transactional object. */
    nontransaction N(C);

    /* Execute SQL query */
    result R(N.exec(sql));

    /* List down all the records */
    for (result::const_iterator c = R.begin(); c != R.end(); ++c)
    {
      databaseid = c[0].as<int>() + 1;
    }
    C.disconnect();
  }
  catch (const std::exception &e)
  {
    cerr << e.what() << std::endl;
    return;
  }
}

void insert_in_database()
{
  //to add in insert: board to string, nrplayer, start to string and end to string DO NOT FORGET ABOUT LEADERBAORD!!!!!!!!!!!!!
  std::ostringstream os1, os4;
  for (int i = 0; i < 47; i++)
  {
    if (playing_board->board[i] != 0)
      os1 << playing_board->board[i];
    else
      os1 << "o";
  }
  for (int i = 0; i < 4; i++)
  {
    if (playing_board->leaderboard[i] != 0)
      os4 << playing_board->leaderboard[i];
    else
      os4 << "o";
  }
  std::string board_to_string(os1.str());
  std::string leaderboard_to_string(os4.str());

  std::ostringstream os2;
  std::ostringstream os3;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
    {
      if (playing_board->start[i][j] != 0)
      {
        os2 << playing_board->start[i][j];
      }
      else
        os2 << "o";
      if (playing_board->end[i][j] != 0)
      {
        os3 << playing_board->end[i][j];
      }
      else
        os3 << "o";
    }
  std::string start_to_string(os2.str());
  std::string end_to_string(os3.str());

  string sql;

  try
  {
    connection C("dbname = th user = postgres password = myPassword \
      hostaddr = 127.0.0.1 port = 5432");
    if (C.is_open())
    {
      cout << "Opened database successfully: " << C.dbname() << endl;
    }
    else
    {
      cout << "Can't open database" << endl;
      return;
    }
    work W(C);
    string nrplayer_str = std::to_string(nrplayer);
    string activeplayers_str = std ::to_string(connnumber);
    string id_str = std::to_string(::databaseid);
    string sqlend = ");";
    string comma = ",";

    sql = "INSERT INTO save_states (ID,LASTPLAYER,ACTIVEPLAYERS,LEADERBOARD,STARTAREA,BOARD,ENDAREA) "
          "VALUES (";

    string command = sql + id_str + comma + "\'" + nrplayer_str + "\'" + comma + "\'" + activeplayers_str + "\'" + comma + "\'" + leaderboard_to_string + "\'" + comma + "\'" + start_to_string + "\'" + comma + "\'" + board_to_string + "\'" + comma + "\'" + end_to_string + "\'" + sqlend;
    W.exec(command);
    W.commit();

    databaseid++;
  }
  catch (const std::exception &e)
  {
    cerr << e.what() << std::endl;
    return;
  }
}

void load_from_database()
{
  try
  {
    connection C("dbname = th user = postgres password = myPassword \
      hostaddr = 127.0.0.1 port = 5432");
    if (C.is_open())
    {
      cout << "Opened database successfully: " << C.dbname() << endl;
    }
    else
    {
      cout << "Can't open database" << endl;
      return;
    }
    /* Create SQL statement */
    string sql = "SELECT * from save_states ORDER BY ID DESC"
                 " LIMIT 1 ";

    // /* Create a non-transactional object. */
    nontransaction N(C);

    /* Execute SQL query */
    result R(N.exec(sql));
    string string_to_int_board, string_to_int_leaderboard, string_to_int_start, string_to_int_end, string_to_int_activeplayers;
    /* List down all the records */
    result::const_iterator c;
    for (c = R.begin(); c != R.end(); ++c)
    {
      ::testnrplayer = c[1].as<int>();
      ::test_saved_game_players = c[2].as<int>();
      string_to_int_leaderboard = c[3].as<string>();
      string_to_int_start = c[4].as<string>();
      string_to_int_board = c[5].as<string>();
      string_to_int_end = c[6].as<string>();
    }
    int j = 0, helperstart[100], helpend[100];
    //parsing the read start from the database
    for (std::string::size_type i = 0; i < string_to_int_start.size(); i++)
    {
      if (string_to_int_start[i] == 'o')
        helperstart[i] = 0;
      else
        helperstart[i] = 1;
    }
    for (int i = 0; i < 4; i++)
    {
      ::teststart[0][i] = helperstart[i];
    }

    for (int i = 4; i < 8; i++)
    {
      ::teststart[1][i - 4] = helperstart[i];
    }

    for (int i = 8; i < 12; i++)
    {
      ::teststart[2][i - 8] = helperstart[i];
    }

    for (int i = 12; i < 16; i++)
    {
      ::teststart[3][i - 12] = helperstart[i];
    }

    //parsing the read board from the database
    for (std::string::size_type i = 0; i < string_to_int_board.size(); i++)
    {
      if (string_to_int_board[i] == 'o')
      {
        ::testboard[j] = 0;
        j++;
      }
      else
      {
        ::testboard[j] = 10 * (string_to_int_board[i] - '0') + string_to_int_board[i + 1] - '0';
        j++;
        i++;
      }
    }
    j = 0;
    //parsing the read end from the database
    for (std::string::size_type i = 0; i < string_to_int_end.size(); i++)
      if (string_to_int_end[i] == 'o')
      {
        helpend[j] = 0;
        j++;
      }
      else
      {
        helpend[j] = 10 * (string_to_int_end[i] - '0') + string_to_int_end[i + 1] - '0';
        j++;
        i++;
      }

    for (int i = 0; i < 4; i++)
    {
      ::testend[0][i] = helpend[i];
    }

    for (int i = 4; i < 8; i++)
    {
      ::testend[1][i - 4] = helpend[i];
    }

    for (int i = 8; i < 12; i++)
    {
      ::testend[2][i - 8] = helpend[i];
    }

    for (int i = 12; i < 16; i++)
    {
      ::testend[3][i - 12] = helpend[i];
    }

    //parsing leaderboard
    for (std::string::size_type i = 0; i < string_to_int_leaderboard.size(); i++)
    {
      if (string_to_int_end[i] == 'o')
      {
        ::testleaderboard[i] = 0;
      }
      else
        ::testleaderboard[i] = string_to_int_end[i] - '0';
    }
    cout << endl;
    C.disconnect();
  }
  catch (const std::exception &e)
  {
    cerr << e.what() << std::endl;
    return;
  }
}

void load_into_current_board()
{
  loadgame = true;
  for (int i = 0; i < 47; i++)
  {
    playing_board->board[i] = testboard[i];
    if (testboard[i] != 0)
    {
      playing_board->pawns[testboard[i] / 10 - 1][testboard[i] % 10 - 1].position = i;
      playing_board->pawns[testboard[i] / 10 - 1][testboard[i] % 10 - 1].distance_traveled = i;
    }
  }
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
    {
      playing_board->start[i][j] = teststart[i][j];
      playing_board->end[i][j] = testend[i][j];
    }
  nrplayer = testnrplayer;
  saved_game_players = test_saved_game_players;

  for (int i = 0; i < 4; i++)
    playing_board->leaderboard[i] = testleaderboard[i];
}

bool check_end()
{
  bool continue_game = false;
  int finish_order = 0, end_pawn_number = 0;
  for (int i = 0; i < 4; i++)
    if (playing_board->leaderboard[i] > finish_order)
      finish_order = playing_board->leaderboard[i];
  for (int i = 0; i < saved_game_players; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      if (playing_board->end[i][j] == 0)
        continue_game = true;
      else
        end_pawn_number++;
    }
    if (end_pawn_number == 4 && playing_board->leaderboard[i] == 0)
    {
      finish_order++;
      playing_board->leaderboard[i] = finish_order;
    }
    end_pawn_number = 0;
  }
  if (continue_game == false)
  {
    for (int i = 0; i < 4; i++)
    {
      cout << playing_board->leaderboard[i] << "  ";
    }
    cout << endl;
  }
  return continue_game;
}

int main()
{
  bool OK = false;
  create_table();
  connect_to_database();
  load_from_database();
  cout << "Load game?\n";
  cin >> OK;
  if (OK)
    load_into_current_board();
  cout << "It's player " << nrplayer + 1 << "'s turn to move" << endl;

  {
    struct sockaddr_in server; // structura folosita de server
    struct sockaddr_in from;
    int nr;            //mesajul primit de trimis la client
    int sd;            //descriptorul de socket
    pthread_t th[100]; //Identificatorii thread-urilor care se vor crea
    int i = 0;

    /* crearea unui socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror("[server]Eroare la socket().\n");
      return errno;
    }
    /* utilizarea optiunii SO_REUSEADDR */
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* pregatirea structurilor de date */
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons(PORT);

    /* atasam socketul */
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
      perror("[server]Eroare la bind().\n");
      return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen(sd, 4) == -1)
    {
      perror("[server]Eroare la listen().\n");
      return errno;
    }
    /* servim in mod concurent clientii...folosind thread-uri */

    while (1)
    {
      int client;
      thData *td; //parametru functia executata de thread
      socklen_t length = sizeof(from);

      printf("[server]Waiting at port %d...\n", PORT);
      fflush(stdout);

      // client= malloc(sizeof(int));
      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */

      if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
      {
        perror("[server]Eroare la accept().\n");
        continue;
      }
      else
      {
        if (connnumber >= 4 || startgame == true || (loadgame == true && connnumber >= test_saved_game_players))
        {
          printf("Too many clients\n");
          close(client);
        }

        else
        {
          connnumber++;
          /* s-a realizat conexiunea, se astepta mesajul */

          // int idThread; //id-ul threadului
          // int cl; //descriptorul intors de accept

          td = (struct thData *)malloc(sizeof(struct thData));
          td->idThread = i++;
          td->cl = client;

          pthread_create(&th[i], NULL, &treat, td);
        }
      }
    } //while
  }
};
static void *treat(void *arg)
{
  struct thData tdL;
  tdL = *((struct thData *)arg);
  printf("[thread]- %d - Waiting for client...\n", tdL.idThread);
  fflush(stdout);
  pthread_detach(pthread_self());
  while (1)
  {
    if (connnumber < saved_game_players)
    {
      continue;
    }
    raspunde((struct thData *)arg);
  }
  /* am terminat cu acest client, inchidem conexiunea */
  close((intptr_t)arg);
  return (NULL);
};

void raspunde(void *arg)
{

  if (nrplayer >= connnumber)
  {
    nrplayer = 0;
  }
  int nr, i = 0, pawn_to_move = 0;
  struct thData tdL;
  tdL = *((struct thData *)arg);

  //check to see if client is still connected
  if (write(tdL.cl, &nr, sizeof(int)) <= 0)
  {
    printf("[Thread %d] ", tdL.idThread);
    perror("[Thread]Eroare la write() catre client.\n");
  }
  //read the actual message
  if (read(tdL.cl, &nr, sizeof(int)) <= 0)
  {
    printf("[Thread %d]\n", tdL.idThread);
    perror("Eroare la read() de la client.\n");
  }
  if (nr == 5)
  {
    //insert results of move in database
    insert_in_database();
    exit(EXIT_SUCCESS);
  }
  if (nr == 6)
  {
    exit(EXIT_SUCCESS);
  }
  /* returnam mesajul clientului */
  if (nrplayer == tdL.idThread)
  {
    pawn_to_move = nr;
    /*pregatim mesajul de raspuns */

    //roll dice
    nr = playing_board->diceRoll();

    //make actual move
    int aux;
    aux = playing_board->makeMove(nrplayer, pawn_to_move, nr);
    if (check_end() == false)
    {
      exit(EXIT_SUCCESS);
    }

    if (nr == 6)
      nrplayer--;
    nr = aux;
    printf("[Thread %d]The pawn will move %d spaces\n", tdL.idThread, nr);
    startgame = true;
    if (write(tdL.cl, &nr, sizeof(int)) <= 0)
    {
      printf("[Thread %d] ", tdL.idThread);
      perror("[Thread]Eroare la write() catre client.\n");
    }
    else
      printf("[Thread %d]Successfuly sent dice roll.\n", tdL.idThread);
  }
  //turn check
  else
  {
    nrplayer--;
    nr = -1;
    if (write(tdL.cl, &nr, sizeof(int)) <= 0)
    {
      printf("[Thread %d] ", tdL.idThread);
      perror("[Thread]Eroare la write() catre client.\n");
    }
    else
      printf("[Thread %d]Not this player's turn!.\n", tdL.idThread);
  }
  nrplayer++;
}