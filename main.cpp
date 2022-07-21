#include <bits/stdc++.h>
#include <cstring>
#include "sqlite3.h"
#define QUERY_SIZE 512
#include<iostream>
#include<fstream>
#include<Windows.h>
#include<conio.h>
#include<cstdlib>
#include<iomanip>
#include<string>
#include<malloc.h>/

using namespace std;

COORD exitP={0, 0};
COORD *Empty;//Notes down places for empty place COORD in sudoku
COORD C={0, 0};//Cursor Position
COORD FirstP={0, 0};//First Sudoku Position
COORD LastP={0, 0};//Last Sudoku Position
int c=0;//number of empty places
int inst=0;//gives the number of places filled by user at an instant
int n;
string uname="";

//Functions Declaration
void newUser();//Creating new user
void Sudoku();//The Sudoku
void colorSet(int);//Colored Text
void checkUser();//Checking if user credentials are correct
void MainMenu();//The First Menu
int getUserStatus();//Getting user Status/Level
void fileOpen(int, int, string);//For file
void printValues(int* a, int n);//For actual sudoku Printing
void gameBorders(string, int, char, int);//Game Borders
void showConsole(COORD);
void keyPress(int);
void upKey();
void downKey();
void rightKey();
void leftKey();
void delKey();
COORD getXPosition();
char getCharXY();
int checkifEmpty();
void checkSudoku(int*);
void passFail(int);

class Database{
  private:
    sqlite3 *_db;
  public:
    static int __temp;
    static std::string __temp_string;

 /*   Database(const char *database){
        open(database);
    }

//    ~Database(){
        close();
    }
*/
    int open(const char *database);
    void close(){
        sqlite3_close(_db);
    }

    int checkLoginInfo(const char* username, const char* password);

    int createUser(const char* username, const char* password);
    int changeUsername(const char* old_username, const char* new_username);


    int setPassword(const char* username, const char *password);
    int setLevel(const char* username, const int level);

    std::string getPassword(const char* username);
    int getLevel(const char* username);

    static int checkLoginInfoCallback(void *arg, int no_columns, char **data, char **columns);
    static int getPasswordCallback(void *arg, int no_columns, char **data, char **columns);
    static int getLevelCallback(void *arg, int no_columns, char **data, char **columns);

};

Database db;

int Database::__temp;
std::string Database::__temp_string;

int Database::open(const char *database){
  int res = sqlite3_open(database, &_db);
  if( res ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(_db));
      return(-1);
  }
  else {

      char query[QUERY_SIZE];
      char *error;
      sprintf(query, "CREATE TABLE IF NOT EXISTS USERS(\
                      ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
                      USERNAME VARCHAR(20) NOT NULL UNIQUE,\
                      PASSWORD VARCHAR(20) NOT NULL,\
                      LEVEL INT NOT NULL\
                      );");
      res = sqlite3_exec(_db, query, NULL, NULL, &error);

      if( res != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", error);
        sqlite3_free(error);
        return -1;
      }

    return 0;
  }
}

int Database::createUser(const char* username, const char* password){
  char query[QUERY_SIZE];
  char *error;
  sprintf(query, "INSERT INTO USERS(USERNAME, PASSWORD, LEVEL) VALUES('%s', '%s', %d);", username, password, 0);
  int res = sqlite3_exec(_db, query, NULL, NULL, &error);

  if( res != SQLITE_OK ){
        if(strcmp(error, "UNIQUE constraint failed: USERS.USERNAME")==0){
            cout<<"Username already Exists";
            sqlite3_free(error);
            return 2;
        }
        else
        fprintf(stderr, "SQL error: %s\n", error);
        sqlite3_free(error);
        return -1;
  }

  return 0;
}

int Database::changeUsername(const char* old_username, const char* new_username){
  char query[QUERY_SIZE];
  char *error;
  sprintf(query, "UPDATE USERS SET USERNAME = '%s' WHERE USERNAME = '%s';", new_username, old_username);
  int res = sqlite3_exec(_db, query, NULL, NULL, &error);

  if( res != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", error);
    sqlite3_free(error);
    return -1;
  }

  return 0;
}

int Database::setPassword(const char* username, const char* password){
  char query[QUERY_SIZE];
  char *error;
  sprintf(query, "UPDATE USERS SET PASSWORD = '%s' WHERE USERNAME = '%s';", password, username);
  int res = sqlite3_exec(_db, query, NULL, NULL, &error);

  if( res != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", error);
    sqlite3_free(error);
    return -1;
  }

  return 0;
}

int Database::setLevel(const char* username, const int level){
  char query[QUERY_SIZE];
  char *error;
  sprintf(query, "UPDATE USERS SET LEVEL = %d WHERE USERNAME = '%s';", level, username);
  int res = sqlite3_exec(_db, query, NULL, NULL, &error);

  if( res != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", error);
    sqlite3_free(error);
    return -1;
  }

  return 0;
};

int Database::getPasswordCallback(void *arg, int no_columns, char **data, char **columns){
  __temp_string = data[2];
  return 0;
}

std::string Database::getPassword(const char* username){
  char query[QUERY_SIZE];
  char *error;

  __temp_string = "";

  sprintf(query, "SELECT * FROM USERS WHERE USERNAME = '%s';", username);
  int res = sqlite3_exec(_db, query, getPasswordCallback, NULL, &error);

  if( res != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", error);
    sqlite3_free(error);
  }

  return __temp_string;
}

int Database::getLevelCallback(void *arg, int no_columns, char **data, char **columns){
  __temp = std::atoi(data[3]);
  return 0;
}

int Database::getLevel(const char* username){
  char query[QUERY_SIZE];
  char *error;
  sprintf(query, "SELECT * FROM USERS WHERE USERNAME = '%s';", username);

  __temp = 0;

  int res = sqlite3_exec(_db, query, getLevelCallback, NULL, &error);
  if( res != SQLITE_OK ){
//    fprintf(stderr, "SQL error: %s\n", error);
    sqlite3_free(error);
    return -1;
  }

  return __temp;
}

int Database::checkLoginInfoCallback(void *arg, int no_columns, char **data, char **columns){
  char *password =  (char *) arg;
  __temp = !std::strcmp(password, data[2]);
  return 0;
}

int Database::checkLoginInfo(const char *username, const char *password){
  char query[QUERY_SIZE];
  char *error;
  sprintf(query, "SELECT * FROM USERS WHERE USERNAME = '%s';", username);

  __temp = 0;

  int res = sqlite3_exec(_db, query, checkLoginInfoCallback, (void *)password, &error);
  if( res != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", error);
    sqlite3_free(error);
    return -1;
  }

  return __temp;
}

char getCharXY(int x, int y){
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CHAR_INFO buffer;
	COORD buffer_size ={1, 1};
	COORD buffer_index = {0, 0};
	SMALL_RECT read_rect = {x, y, x, y};
	return ReadConsoleOutput(h, &buffer, buffer_size, buffer_index, &read_rect)?buffer.Char.AsciiChar: '\0';
}

void showConsole(COORD pos){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsole, pos);
}

COORD getXPosition(){
	HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    if (GetConsoleScreenBufferInfo(h, &cbsi))
    {
        return cbsi.dwCursorPosition;
	}
	else
    {
        // The function failed. Call GetLastError() for details.
        COORD invalid = { 0, 0 };
        return invalid;
    }
}

int checkIfEmpty(){
	int u=getXPosition().X, v=getXPosition().Y;
	for(int i=0; i<c; i++){
		int m=Empty[i].X, n=Empty[i].Y;
		if(u==m && n==v){
			return 1;
		}
	}
	return 0;
}

void gameBorders(string s, int n, char x, int p){
	if(s=="outerBorder"){
		colorSet(13);
		cout<<"$";
		for(int i=0; i<n; i++)
			cout<<"$$$$";
		cout<<endl;
	}
	else if(s=="innerBorder"){
		colorSet(13);
		if(n==4){
			cout<<"$---*---$---*---$"<<endl;
		}
		else if(n==6){
			cout<<"$---*---*---$---*---*---$"<<endl;
		}
		else if(n==9){
			cout<<"$---*---*---$---*---*---$---*---*---$"<<endl;
		}
	}
	else if(s=="verticalBorder"){
		colorSet(13);
		 cout<<" ";
		if((x-'1')>=0 && (x-'1')<=8){
			colorSet(2);
			 cout<<x-'0';
		}
		else if(x=='0'){
			Empty[p]=getXPosition();
			if(p==0)
				C=getXPosition();
			colorSet(15);
			 cout<<" ";
		}
		colorSet(13);
		 cout<<" |";
	}
	else if(s=="dollarBorder"){
		colorSet(13);
		cout<<" ";
		if((x-'1')>=0 && (x-'1')<=8){
			colorSet(2);
			 cout<<(x-'0');
		}
		else if(x=='0'){
			Empty[p]=getXPosition();
			if(p==0)
				C=getXPosition();
			colorSet(15);
			 cout<<" ";
		}
		colorSet(13);
		cout<<" $";
	}
}

void newUser(){
    int me=db.open("Sudoku.db");
    if(me==0){
        cout<<"Enter Username: "<<endl;
        cin>>uname;
        cout<<"Enter Password:\n";
        string c="";
        char d=getch();
        while(d!='\r'){
            c=c+d;
            cout<<"*";
            d=getch();
        }
        int l=db.createUser(uname.c_str(), c.c_str());
        if(l==0){
            cout<<"New User Created. Go Back to Main Menu and login again"<<endl;
            cout<<endl<<endl<<endl<<"Press q/Q to quit."<<endl;
            cout<<"Press b/B to go back to Main Menu."<<endl;
            keyPress(9);
        }
        else if(l==2){
            cout<<endl<<endl<<endl<<"Press q/Q to quit."<<endl;
            cout<<"Press b/B to go back to Main Menu."<<endl;
            keyPress(9);
        }
        else{
            cout<<"Database Error"<<endl;
            exit(0);
        }
    }
    else{
        cout<<"Database Error"<<endl;
        exit(0);
    }
}

void writeConsole(COORD pos, char c){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsole, pos);
	WriteConsole(hConsole, &c, 1, NULL, NULL);
}

void upKey(){
	if(C.Y!=FirstP.Y){
		C.Y-=2;
		showConsole(C);
	}
}

void downKey(){
	if(C.Y!=LastP.Y){
		C.Y+=2;
		showConsole(C);
	}
}

void rightKey(){
	if(C.X!=LastP.X){
		C.X+=4;
		showConsole(C);
	}
}

void leftKey(){
	if(C.X!=FirstP.X){
		C.X-=4;
		showConsole(C);
	}
}

void setNumberKey(char s, int n){
	if(n==4){
		int x=s-'4';
		if(x>=-3 &&x<=0){
			if(getCharXY(getXPosition().X, getXPosition().Y)==' ')
			inst++;
			cout<<s;
		}
	}
	else if(n==6){
		int x=s-'6';
		if(x>=-5 && x<=0){
			if(getCharXY(getXPosition().X, getXPosition().Y)==' ')
			inst++;
			cout<<s;
		}
	}
	else if(n==9){
		int x=s-'9';
		if(x>=-8 &&x<=0){
			if(getCharXY(getXPosition().X, getXPosition().Y)==' ')
			inst++;
			cout<<s;
		}
	}
}

string sudokuStatus(){
	if(c==inst)
	return "Done   ";
	else
	return "Playing";
}

void delKey(){
	inst--;
	writeConsole(getXPosition(), ' ');
}

void printValues(int *a, int n){
	FirstP=getXPosition();
	c=0;
	for(int i=0; i<(n*n); i++){
		if(a[i]==0)
		c++;
	}
	Empty=(COORD*) malloc(sizeof(COORD)*c);
	int er=-1;
	for(int i=0; i<n; i++){
		if(n==4 || n==6){
			if(i%2==0)
			gameBorders("outerBorder", n, '0', 0);
			else
			gameBorders("innerBorder", n, '0', 0);
		}
		else if(n==9){
			if(i%3==0)
			gameBorders("outerBorder", n, '0', 0);
			else
			gameBorders("innerBorder", n, '0', 0);
		}

		colorSet(13);
		cout<<"$";
		for(int j=0;j<n;j++){
			char x=(*(a+i*n+j))+'0';
			if(x=='0'){
				er++;
			}
			if(n==4){
				if(j%2==0)
				gameBorders("verticalBorder", n, x, er);
				else
				gameBorders("dollarBorder", n, x, er);
			}
			else if(n==6||n==9){
				if(j%3==2){
					gameBorders("dollarBorder", n, x, er);
				}
				else{
					gameBorders("verticalBorder", n, x, er);
				}
			}
		}
		cout<<endl;
	}
	gameBorders("outerBorder", n, '0', er);
	cout<<endl;
	colorSet(15);
	FirstP.X+=2;
	FirstP.Y+=1;
	LastP.X=FirstP.X+4*(n-1);
	LastP.Y=FirstP.Y+2*(n-1);
	if(getCharXY(LastP.X, LastP.Y)==' '){
		er++;
		Empty[er]=LastP;
	}
	colorSet(3);
	cout<<"Instructions:"<<endl;
	colorSet(6);
	cout<<"Press the arrow keys in the desired direction."<<endl;
	cout<<"Press del or backspace to delete an entered number."<<endl;
	cout<<"Press the number to enter the desired number."<<endl;
	cout<<"Press Q or q to quit at any time."<<endl;
	cout<<"Press b/B to go back to Main Menu."<<endl;
	cout<<"Message if any: ";
	exitP=getXPosition();
	colorSet(15);
	showConsole(C);
	keyPress(n);
}

void fileOpen(int n, int l, string s){
	string mytext;
	int *arr;
	//Opening the file and creating the array
	arr=new int[n*n];
	ifstream MyFile;
	MyFile.open(s.c_str());

	//Moving to correct line
	int i=0, j=0, Line=0;
	if(l!=1){
		int x=l-1;
		while(Line!=((n*x)+x)){
			getline(MyFile, mytext);
			Line++;
		}
	}

	//Assigning value to the Array
	int t=(n*l)+l;
	while(Line!=((n*l)+l-1)){
		getline(MyFile, mytext);
		for(i=0; i<n;i++){
			int c=mytext[i];
			*(arr+j*n+i)=c-48;
		}
		j++;
		Line++;
	}
	MyFile.close();
	gameBorders("sudokuOuterBorder", n, '0', 0);
	printValues(arr, n);
}

void checkSudoku(int *a){
	int incorrect=0;
	COORD P=FirstP;
	int x=FirstP.X, y=FirstP.Y;
	int n=((LastP.X-FirstP.X)/4)+1;
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			char c=getCharXY(x+j*4, y+i*2);
			if(c-'0'!=(*(a+i*n+j))){
				colorSet(12);
				COORD RE={x+j*4, y+i*2};
				 writeConsole(RE, (*(a+i*n+j)+'0'));
				colorSet(15);
				incorrect++;
			}
		}
	}
	showConsole(exitP);
	cout<<"You got "<<incorrect<<" incorrect answers"<<endl;
	cout<<"The incorrect answers are in Peach color."<<endl;
	passFail(incorrect);
}

void passFail(int in){
	cout<<"The maximum incorrect answers allowed were 3."<<endl;
	if(in>3){
		cout<<"As your incorrect answers exceed limit, you are struck on this level"<<endl;
		colorSet(4);
		cout<<"FAIL"<<endl;
		colorSet(15);
		int newv = db.getLevel(uname.c_str());
		if(newv==-1) {
                exit(0);
		}
		else{
            db.setLevel(uname.c_str(), db.getLevel(uname.c_str())+1);
            db.close();
		}
	}
	else{
		cout<<"As your incorrect answers are within limit, you move on to the next level"<<endl;
		colorSet(3);
		cout<<"PASS"<<endl;
		colorSet(15);
		int newv = db.getLevel(uname.c_str());
		if(newv==-1) {
            exit(0);
		}
		else{
            db.setLevel(uname.c_str(), db.getLevel(uname.c_str())+1);
            db.close();
		}
	}
	cout<<"Wait 10 Seconds to go back to Main Menu"<<endl;
	Sleep(10000);
	MainMenu();
}

void fileOpenA(int n, int l, string s){
	string mytext;
	int *Ansarr;
	//Opening the file and creating the array
	Ansarr=new int[n*n];
	ifstream MyFile;
	MyFile.open(s.c_str());

	//Moving to correct line
	int i=0, j=0, Line=0;
	if(l!=1){
		int x=l-1;
		while(Line!=((n*x)+x)){
			getline(MyFile, mytext);
			Line++;
		}
	}

	//Assigning value to the Array
	while(Line!=((n*l)+l-1)){
		getline(MyFile, mytext);
		for(i=0; i<n;i++){
			int c=mytext[i];
			*(Ansarr+j*n+i)=c-48;
		}
		j++;
		Line++;
	}
	MyFile.close();
	checkSudoku(Ansarr);
}

void colorSet(int n){
	HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, n);
}

void keyPress(int n){
	while(1){
		int keypress=0;
		keypress=getch();
		//Arrow keys
		if(keypress==72||keypress==75||keypress==77||keypress==80){
			if(keypress==72){
				upKey();
			}
			else if(keypress==75){
				leftKey();
			}
			else if(keypress==77){
				rightKey();
			}
			else if(keypress==80){
				downKey();
			}
		}
		else if(keypress==8 || keypress==83 || (keypress>=49 && keypress<=57)){
			//Del or backspace
			if(checkIfEmpty()==1){
				if(keypress==83|| keypress==8){
					delKey();
				}
				else{
					if(keypress==49){
						setNumberKey('1', n);
					}
					else if(keypress==50){
						setNumberKey('2', n);
					}
					else if(keypress==51){
						setNumberKey('3', n);
					}
					else if(keypress==52){
						setNumberKey('4', n);
					}
					else if(keypress==53){
						setNumberKey('5', n);
					}
					else if(keypress==54){
						setNumberKey('6', n);
					}
					else if(keypress==55){
						setNumberKey('7', n);
					}
					else if(keypress==56){
						setNumberKey('8', n);
					}
					else if(keypress==57){
						setNumberKey('9', n);
					}
				}
			}
		}
		else if(keypress==113 || keypress==81){//q or Q
			showConsole(exitP);
			exit(0);
		}
		else if(keypress==13){//Enter Key
			if(sudokuStatus()=="Playing"){
				showConsole(exitP);
				cout<<"You are still "<<sudokuStatus()<<endl;
				showConsole(C);
			}
			else{
				showConsole(exitP);
				cout<<"You are "<<sudokuStatus()<<"      "<<endl;
				exitP=getXPosition();
				break;
			}

		}
		else if(keypress==98 || keypress==66){//b/B
			showConsole( exitP );
			MainMenu();
		}
		else{
			continue;
		}
	}
}

void Sudoku(int u){
	if(u>=0 && u<15){
		string s1, s2;
		int l;
		if(u>=0 && u<5){
			s1="Easy.txt";
			s2="EasyAns.txt";
			l=u+1;//
			n=4;//Sudoku Size
			//File_open(n, l, s);
		}
		else if(u>=5 && u<10){
			s1="Medium.txt";
			s2="MediumAns.txt";
			l=u-4;
			n=6;
			//File_open(n, l, s);
		}
		else{
			s1="Hard.txt";
			s2="HardAns.txt";
			l=u-9;
			n=9;
		}
		fileOpen(n, l, s1);
		fileOpenA(n, l, s2);
	}

	else{
		cout<<"CONGRATULATION YOU ARE DONE!!!"<<endl;
		char keypress=0;
		cout<<"Press R/r to Reset Level"<<endl;
		keypress=getch();
		if(keypress==114||keypress==82){
            cout<<(char)keypress<<endl;
            db.setLevel(uname.c_str(), 0);
            Sleep(700);
            MainMenu();
		}

	}
}

int getUserStatus(){
    return db.getLevel(uname.c_str());
}

void checkUser(){
    int me=db.open("Sudoku.db");
    if(me==0){
        string c="";
        colorSet(8);
         cout<<"Username: ";
        colorSet(15);
         cin>>uname;
        colorSet(8);
        cout << "Enter the password: ";
        colorSet(15);
        char d=getch();
        while(d!='\r'){
                c=c+d;
                cout<<"*";
                d=getch();
        }
        cout<<endl;
        if(db.checkLoginInfo(uname.c_str(), c.c_str())==1){
            system("cls");
            cout<<endl;

            int x;
            x=getUserStatus();
            if(x!=15){
                cout<<setw(20)<<"You Got Entry";
                cout<<" in level "<<x<<endl<<endl;
            }
            Sudoku(x);
            db.close();
        }
        else{
            system("cls");
            colorSet(12);
            cout<<"Wrong Username or Password"<<endl<<endl;
            colorSet(15);
            cout<<endl<<endl<<endl<<"Press q/Q to quit."<<endl;
            cout<<"Press b/B to go back to Main Menu."<<endl;
            db.close();
            keyPress(n);
        }
    }
    else{
        cout<<"Database Error"<<endl;
        exit(0);
    }
}

void MainMenu(){
	char c='0';
	int x;
	while(c=='0'){
		system("cls");
		colorSet(13);
		 cout<<setw(12)<<"SUDOKU"<<endl;
		colorSet(15); //WHITE
		 cout<<setw(12)<<"======"<<endl<<endl;
		 cout<<"[1] ";
		colorSet(10); //LIGHT GREEN
		 cout<<"Login"<<endl;
		colorSet(15); //WHITE
		 cout<<"[2] ";
		colorSet(14); //YELLOW
		 cout<<"New User"<<endl;
		colorSet(15); //WHITE
		 cout<<"[3] ";
		colorSet(12); //LIGHT RED
		 cout<<"Play as guest"<<endl;
		colorSet(15); //WHITE
		 cout<<"[4] ";
		colorSet(8);
		 cout<<"Instructions"<<endl;
		colorSet(15); //WHITE
		 cout<<"[5] ";
		colorSet(3);
		 cout<<"Exit"<<endl;
		colorSet(15);
		 cout<<"Enter your choice:"<<endl;
		c=getch();
		cout<<c<<endl;
		x=c-'0';
	}
	if(x==1)
		checkUser();
	else if(x==2){
		system("cls");
		newUser();
	}
	else if(x==3){
		system("cls");
		int x=rand()%16;
//		x=getUserStatus(c);
		cout<<endl;
		cout<<setw(20)<<"This is level "<<x+1<<endl<<endl;
		Sudoku(x);
	}
	else if(x==4){
		system("cls");
		colorSet(12);
		cout<<setw(50)<<"WELCOME TO SUDOKU"<<endl;
		cout<<setw(50)<<"================="<<endl<<endl;
		colorSet(6);
		cout<<setw(15)<<"You have to fill the empty boxes in such a way that no number repeats itself in a row, a column or its own sub-grid."<<endl;
		cout<<setw(15)<<"This game consists of 3 difficulty levels.:"<<endl<<"      Easy with 4*4 grids. Here you have to fill numbers between 1-4."<<endl
		<<"      Medium with 6*6 grids to fill with numbers 1-6."<<endl<<"      Difficulty with 9*9 grids to fill with numbers 1-9."<<endl
		<<"Use arrow keys to move around the box and delete/backspace key to clear the number selected in the puzzle."<<endl<<endl;
		colorSet(12);
		cout<<setw(50)<<"HAPPY PLAY :)"<<endl;
		colorSet(15);
		cout<<"Press Q/q to Quit"<<endl;
	    cout<<"Press b/B to go back to Main Menu."<<endl;
	    int keypress=getch();
	    if(keypress==98 || keypress==66){//b/B
			MainMenu();
		}
		else{
            exit(0);
		}
	}
	else if(x==5){
		exit(0);
	}
	else{
	    cout<<"Enter a Valid choice"<<endl;
        Sleep(700);
        MainMenu();
	}
}

int main(){
    MainMenu();
}
