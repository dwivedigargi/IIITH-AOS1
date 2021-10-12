#include<bits/stdc++.h>
#include<dirent.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<pwd.h>
#include<grp.h>
#include<filesystem>
#include<termios.h>
using namespace std;

vector<int> isFile;
vector<string> currentDir;

void listFiles(string dirname){
    isFile.clear();
    currentDir.clear();
    
    int n=dirname.length();
    char dirnamec[n+1];
    strcpy(dirnamec, dirname.c_str());
    DIR* dir = opendir(dirnamec);
    if (dir==NULL) {
        return;   
    }

    struct dirent* entity;
    entity = readdir(dir);
    while (entity!=NULL) {
        isFile.push_back((entity->d_type)/4 - 1);
        currentDir.push_back(entity->d_name);
        entity = readdir(dir);
    }

    closedir(dir);
}

string toString(char* c) {
    string s="";
    int i=0;
    while(c[i]!='\0')
        s = s+c[i++];
    return s;
}

string permissions(string sfile){
    int n=sfile.length();
    char file[n+1];
    strcpy(file, sfile.c_str());
    struct stat st;
    char *modeval = (char*)malloc(sizeof(char) * 9 + 1);
    if(stat(file, &st) == 0){
        mode_t perm = st.st_mode;
        modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
        modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
        modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
        modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
        modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
        modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
        modeval[6] = (perm & S_IROTH) ? 'r' : '-';
        modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
        modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
        modeval[9] = '\0';
        return toString(modeval);     
    }
    else{
        return toString(strerror(errno));
    }   
}

string ownerName(string sfile) {
    int n=sfile.length();
    char filename[n+1];
    strcpy(filename, sfile.c_str());
    struct stat info;
    stat(filename, &info);  // Error check omitted
    struct passwd *pw = getpwuid(info.st_uid);
    if(pw != 0)
        return toString(pw->pw_name);
    else 
        return " ";
}

string groupName(string sfile) {
    int n=sfile.length();
    char filename[n+1];
    strcpy(filename, sfile.c_str());
    struct stat info;
    stat(filename, &info);   
    struct group  *gr = getgrgid(info.st_gid);
    if(gr != 0)
        return toString(gr->gr_name);
    else 
        return " ";
}

float filesize(string sfile) {
    int n=sfile.length();
    char filename[n+1];
    strcpy(filename, sfile.c_str());
    struct stat st;
    stat(filename, &st);
    return st.st_size*1.0/1024;
}

string getFileCreationTime(string sfile)
{
    int n=sfile.length();
    char filename[n+1];
    strcpy(filename, sfile.c_str());
    struct stat attrib;
    stat(filename, &attrib);
    char date[50];
    strftime(date, 20, "%a %b %d %H:%M %y", localtime(&(attrib.st_ctime)));
    return toString(date);
}

void printdetails(string s, int isFile) {
    string s1;
    if(s.length()>20)
        s1=s.substr(0, 17)+'.'+'.'+'.';
    else
        s1=s;
    char f;
    if(isFile==1)
        f='-';
    else
        f='d';
    cout<<s1<<"\t"<<f<<permissions(s)<<"\t"<<ownerName(s)<<" "<<groupName(s);
    cout<<"\t"<<fixed<<setprecision(2)<<filesize(s)<<"K\t"<<getFileCreationTime(s);
}

void printKFilesWithMetadata(int start, int end) {
    cout << "\033[2J\033[0;0H"; //clears screen, places cursor at 0;0

    for(int i=start; i<=end; i++) {
        printdetails(currentDir[i], isFile[i]);
        if(i<end)
            cout<<endl;
        else
            cout<<"\r";
    }
}

char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

int keypress () {
    fflush(stdout);
    int c=getch();
    
    if(c==27) {
        fflush(stdout);
        c=getch();
        
        if(c==91) {
            fflush(stdout);
            c=getch();
            
            if(c==65)
                return 1;
            else if(c==67)
                return 2;
            else if(c==66)
                return 3;
            else if(c==68)
                return 4;
        }
    }
    else if(c==107)
        return 5;
    else if(c==108)
        return 6;
    else if(c==105)
        return 7;
    else if(c==68)
        return 8;
    else if(c==10)
        return 9;
    return 0;
}

void enableScrolling(int list_start, int list_end, int start, int end) {
    cout<<"\033[0;0H"; //set cursor
 //   cout<<"\033[?25h"; 
 
    int cursorTracker = 0;
    int x;

    while(true) {
        x=keypress();
        if(x==1 && cursorTracker!=start) {
            cursorTracker--;
            cout<<"\033[1A";  
        }
       else if(x==3 && cursorTracker!=end) {
            cursorTracker++;
            cout<<"\033[1B";
        }
        else if(x==5 && cursorTracker==start && start!=list_start) {
            start--;
            end--;
            cursorTracker--;
            printKFilesWithMetadata(start, end); 
            cout<<"\033[0;0H";   
        }
        else if(x==6 && cursorTracker==end && end!=list_end) {
            start++;
            end++;
            cursorTracker++;
            printKFilesWithMetadata(start, end);
        }
        else if(x==9)
            break;
    }
}

void normal_mode(string s) {

    listFiles(s);

    int start=0, end=19;
    printKFilesWithMetadata(start, end);
    enableScrolling(0, currentDir.size(), start, end);
}

int main (int argc, char* argv[]) {
    stack<string> back, forward;

    char cwd[256];
    getcwd(cwd, 256);
    back.push(toString(cwd));
    
    normal_mode(back.top());
    return 0;
}