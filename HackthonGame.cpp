#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

using namespace std;

void swap(int &a, int &b){
  int t=a;
  a = b;
  b = t;
}
int getrand(int min, int max)
{
  return(min+rand()%(max-min+1));
}

WINDOW *create_newwin(int height,int width,int starty,int startx);
void destory_win(WINDOW *local_win);

int game_win_height=30;
int game_win_width=45;

int hint_win_height=10;
int hint_win_width=20;
WINDOW * game_win, *hint_win ,*score_win;
int key;

class Piece
{
public:
  int score;
  int shape;
  int next_shape;

  int head_x;
  int head_y;

  int size_h;
  int size_w;

  int next_size_h;
  int next_size_w;

  int h;
  int w;
  int currentId;
  int nextId;

  int box_shape[4][4];
  int next_box_shape[4][4];

  int box_map[30][45];

  bool game_over;
  bool hit;
public:
  void initial();
  void set_shape(int &cshape, int box_shape[][4],int &size_w, int & size_h);

  void score_next();
  void judge();
  void move();
  void rotate();
  bool isaggin();
  bool exsqr(int row);
  void moveLeft();
  void moveRight();
  void set_Monster();


};

int main()
{

  initscr();
  //raw();                                                                                                                                                   
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr,TRUE);

  refresh();

  game_win = create_newwin(game_win_height, game_win_width, 0,0);
  wborder(game_win, '*', '*', '*', '*', '*', '*', '*', '*');
  wrefresh(game_win);

  hint_win = create_newwin(hint_win_height, hint_win_width, 0, game_win_width+10);
  mvprintw(0, game_win_width+10+2,"%s","Next");
  refresh();

  score_win = create_newwin(hint_win_height, hint_win_width, 20, game_win_width+10);
  mvprintw(20, game_win_width+10+2,"%s","Score");
  refresh();



  Piece* pp = new Piece;
  pp->initial();


  while(1)
    {
      pp->move();
      if(pp->game_over)
	break;
    }

  destory_win(game_win);
  destory_win(hint_win);
  destory_win(score_win);
  delete pp;
  system("clear");
  int row,col;
  getmaxyx(stdscr,row,col);
  mvprintw(row/2,col/2 ,"%s","GAMER OVER ! \n ");
  mvprintw(row/2+2,col/2-2 ,"%s","Wait 5s to return tthe erminal ! \n ");
  refresh();

  sleep(5);
  endwin();
  return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
  WINDOW *local_win;
  local_win = newwin(height, width, starty, startx);
  box(local_win,0,0);
  wrefresh(local_win);
  return local_win;
}

void destory_win(WINDOW *local_win)
{
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(local_win);
  delwin(local_win);
}


void Piece::initial()
{
  score=100;
  game_over=false;
  for(int i =0;i<game_win_height;i++)
    for(int j=0;j<game_win_width;j++){
      if(i==0 || i==game_win_height-1 || j==0 || j==game_win_width-1){
	box_map[i][j]=1;
      }
      else
	box_map[i][j]=0;
    }
  set_Monster();
  hit = false;
  srand((unsigned)time(0));
  shape=getrand(0,1);
  currentId = shape;
  set_shape(shape,box_shape,size_w,size_h);

  next_shape=getrand(0,1);
  nextId = next_shape;
  set_shape(next_shape,next_box_shape,next_size_w,next_size_h);

  for(int i =0;i<4;i++)
    for(int j=0;j<4;j++)
      if(next_box_shape[i][j]==1){
	mvwaddch(hint_win,(hint_win_height-size_h)/2+i,(hint_win_width-size_w)/2+j,'#');

	wrefresh(hint_win);
      }


  mvwprintw(score_win, hint_win_height/2,hint_win_width/2-2,"%d",score);
  wrefresh(score_win);
}

void Piece::set_Monster(){
  h = game_win_height-2;
  w = game_win_width/2;
  mvwaddch(game_win,h,w+1,'#');
  mvwaddch(game_win,h,w,'#');
  mvwaddch(game_win,h-1,w,'#');
  mvwaddch(game_win,h-1,w+1,'#');
  box_map[h][w+1] = 1;
  box_map[h][w] = 1;
  box_map[h-1][w] = 1;
  box_map[h-1][w+1] = 1;
}

void Piece::set_shape(int &cshape, int shape[][4],int &size_w,int &size_h)
{
  int i,j;
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      shape[i][j]=0;
  switch(cshape)
    {

    case 0:
      size_h=2;
      size_w=2;
      shape[0][0]=1;
      shape[0][1]=1;
      shape[1][0]=1;
      shape[1][1]=1;
      break;

    case 1:
      size_h=2;
      size_w=3;
      shape[0][1]=1;
      shape[1][0]=1;
      shape[1][1]=1;
      shape[1][2]=1;
      break;
    }

  head_x=game_win_width/2 + getrand(-19,19);
  head_y=1;

  if(isaggin())    /* GAME OVER ! */
    game_over=true;

}

void Piece::move(){
  fd_set set;
  FD_ZERO(&set);
  FD_SET(0, &set);

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec= 200000;

  if (select(1, &set, NULL, NULL, &timeout) == 0){
    head_y++;
    if(isaggin()){
      head_y--;
      for(int i=0;i<size_h;i++)
	for(int j=0;j<size_w;j++)
	  if(box_shape[i][j]==1)
	    mvwaddch(game_win,head_y+i,head_x+j,' ');

      if(head_y + 1 == game_win_height-3){
	hit = true;
      }
      else{
	hit = false;
      }
      score_next();
    }
    else{
      for(int i=size_h-1; i>=0;i--)
	for(int j=0;j<size_w;j++){
	  if(this->box_shape[i][j]==1){
	    mvwaddch(game_win,head_y-1+i,head_x+j,' ');
	    mvwaddch(game_win,head_y+i,head_x+j,'#');

	  }
	}
      wrefresh(game_win);
    }

  }

  if (FD_ISSET(0, &set)) {
    while ((key = getch()) == -1) ;

    if(key==KEY_LEFT){

      moveLeft();

    }

    if(key==KEY_RIGHT){

      moveRight();
    }
  }
}

void Piece::moveLeft(){

  mvwaddch(game_win,h,w+1,' ');
  mvwaddch(game_win,h,w,' ');
  mvwaddch(game_win,h-1,w,' ');
  mvwaddch(game_win,h-1,w+1,' ');
  box_map[h][w+1] = 0;
  box_map[h][w] = 0;
  box_map[h-1][w] = 0;
  box_map[h-1][w+1] = 0;
  w=w-1;
  mvwaddch(game_win,h,w+1,'#');
  mvwaddch(game_win,h,w,'#');
  mvwaddch(game_win,h-1,w,'#');
  mvwaddch(game_win,h-1,w+1,'#');
  box_map[h][w+1] = 1;
  box_map[h][w] = 1;
  box_map[h-1][w] = 1;
  box_map[h-1][w+1] = 1;
}

void Piece::moveRight(){
  mvwaddch(game_win,h,w+1,' ');
  mvwaddch(game_win,h,w,' ');
  mvwaddch(game_win,h-1,w,' ');
  mvwaddch(game_win,h-1,w+1,' ');
  box_map[h][w+1] = 0;
  box_map[h][w] = 0;
  box_map[h-1][w] = 0;
  box_map[h-1][w+1] = 0;
  w=w+1;
  mvwaddch(game_win,h,w+1,'#');
  mvwaddch(game_win,h,w,'#');
  mvwaddch(game_win,h-1,w,'#');
  mvwaddch(game_win,h-1,w+1,'#');
  box_map[h][w+1] = 1;
  box_map[h][w] = 1;
  box_map[h-1][w] = 1;
  box_map[h-1][w+1] = 1;
}

bool Piece::isaggin(){
  for(int i=0;i<size_h;i++)
    for(int j=0;j<size_w;j++){
      if(box_shape[i][j]==1){
	if(head_y+i > game_win_height-2)
	  return true;
	if(head_x+j > game_win_width-2 || head_x+i-1<0)
	  return true;
	if(box_map[head_y+i][head_x+j]==1)
	  return true ;
      }
    }
  return false;
}

bool Piece::exsqr(int row){
  for(int j=1;j<game_win_width-1;j++)
    if(box_map[row][j]==1)
      return true;
  return false;
}

void Piece::judge(){
  switch(currentId)
    {
    case 0:
      score = score + 500;
      mvwprintw(score_win, hint_win_height/2,hint_win_width/2-2,"%d",score);
      wrefresh(score_win);
      break;
    case 1:
      score = -500;
      mvwprintw(score_win, hint_win_height/2,hint_win_width/2-2,"%d",score);
      wrefresh(score_win);
      break;

    }

}

void Piece::score_next(){
  if(hit == true){
    judge();
    hit = false;
  }
  else{
    score = score - 10;
    mvwprintw(score_win, hint_win_height/2,hint_win_width/2-2,"%d",score);
    wrefresh(score_win);
  }

  if(score < 0)
    game_over=true;


  currentId = nextId;
  set_shape(currentId,box_shape,size_w,size_h);

  nextId=getrand(0,1);
  set_shape(nextId,next_box_shape,next_size_w,next_size_h);


  for(int i =1;i<hint_win_height-1;i++)
    for(int j=1;j<hint_win_width-1;j++){
      mvwaddch(hint_win, i, j,' ');
      wrefresh(hint_win);
    }
  for(int i =0;i<4;i++)
    for(int j=0;j<4;j++)
      if(next_box_shape[i][j]==1){
	mvwaddch(hint_win,(hint_win_height-size_h)/2+i,(hint_win_width-size_w)/2+j,'#');
	wrefresh(hint_win);
      }
}
