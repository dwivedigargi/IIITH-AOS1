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
#include<cstddef>
#include<fstream>
using namespace std;

vector<int> isFile;
vector<string> currentDir;
stack<string> back, Forward;

void normal_mode(string);
void command_mode();

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
        string nameWithPath = dirname + entity->d_name;
        currentDir.push_back(nameWithPath);
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
    string s1, s2;
    int pos = s.find_last_of('/');
    s2 = s.substr(pos + 1);
    if(s2.length()>20)
        s1=s2.substr(0, 17)+'.'+'.'+'.';
    else
        s1=s2;
    char f;
    if(isFile==1)
        f='-';
    else
        f='d';
    cout<<f<<permissions(s)<<"\t"<<ownerName(s)<<" "<<groupName(s);
    cout<<"\t"<<setw(7)<<fixed<<setprecision(2)<<filesize(s)<<"K\t"<<getFileCreationTime(s)<<"\t"<<s1;
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
                return 1;   //UP
            else if(c==67)
                return 2;   //RIGHT
            else if(c==66)
                return 3;   //DOWN
            else if(c==68)
                return 4;   //LEFT
        }
    }
    else if(c==107)
        return 5;   //k
    else if(c==108)
        return 6;   //l
    else if(c==104)
        return 7;   //h
    else if(c==127)
        return 8;   //bsp
    else if(c==10)
        return 9;   //enter
    else if(c==58)
        return 10;  //colon
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
        else if(x==9) {
            if(!isFile[cursorTracker]) {
                if(currentDir[cursorTracker]==back.top()+"/..") {
                    if(back.top()=="")
                        continue;

                    int pos = back.top().find_last_of("/");
                    string sub = back.top().substr(0 , pos);

                    back.push(sub); 
                    normal_mode(back.top()+"/");
                    continue; 
                } 
                else if(currentDir[cursorTracker]==back.top()+"/.") 
                    continue;
                back.push(currentDir[cursorTracker]);   
                normal_mode(back.top()+"/");
            }
            else {
                int n=currentDir[cursorTracker].length();
                char filename[n+1];
                strcpy(filename, currentDir[cursorTracker].c_str());
         
                int pid = fork();
                if (pid == 0) {
                    execl("/usr/bin/xdg-open", "xdg-open", filename, (char *)0);
                    exit(1);
                }
            }
        }
        else if(x==2) {
            if(!Forward.empty()) {
                back.push(Forward.top());
                Forward.pop();
                normal_mode(back.top()+"/");
            }
        }
        else if(x==4) {
            if(back.size()>1) {
                Forward.push(back.top());
                back.pop();
                normal_mode(back.top()+"/");
            }
        }
        else if(x==8) {

            if(back.top()=="")
                continue;
            int pos = back.top().find_last_of("/");
            string sub = back.top().substr(0 , pos);

            while(!back.empty())
                back.pop();
            while(!Forward.empty())
                Forward.pop();
            back.push(sub);
            normal_mode(sub+"/");
        }
        else if(x==7) {
            back.push("/home");
            normal_mode(back.top()+"/");
        }
        else if(x==10) {
            command_mode();
        }
    }
}

void normal_mode(string s) {
    listFiles(s);

    int start=0, end;
    end=(currentDir.size()-1 > 19 ? 19:currentDir.size()-1);
    printKFilesWithMetadata(start, end);
    enableScrolling(0, currentDir.size()-1, start, end);
}

void create_dir(string dest, string name) {
    if (dest[0]=='~') {
            dest = "/home" + dest.substr(1);
    }
    string sdir=dest+'/'+name;
    int n=sdir.length();
    char dirname[n+1];
    strcpy(dirname, sdir.c_str());
    mkdir(dirname, 0777);
}

void create_file(string dest, string name) {
    // if dest last char = '/'
    string sfile=dest+'/'+name;
    if (sfile[0]=='~')
        {
            sfile = "/home" + sfile.substr(1);
        }
    int n=sfile.length();
    char filename[n+1];
    strcpy(filename, sfile.c_str());
    ofstream file(filename);
    if(file)
        ;
    else{
        cout << "Unable to create file at given location" << sfile;
        exit(0);
    } 
    chmod(filename, S_IRUSR|S_IWUSR);
    file.close();
}

void Goto(string path) {
    int n=path.length();

    if (path[0]=='~')
        {
            path = "/home" + path.substr(1);
        }

    char pathc[n+1];
    strcpy(pathc, path.c_str());   
    chdir(pathc);
}

void moveOneFile (string file, string destination) {
    int n = file.length();
    char filename[n+1];
    strcpy(filename, file.c_str()); //char * of file

    string destprime = destination + "/" + file;
    int nprime = destprime.length();
    char dest[nprime+1];
    strcpy(dest, destprime.c_str());

    rename(filename, dest);
}

void copyOneFile (string file, string destination) { //helloTest/aaya.txt, aaya.txt
    int k = file.find_last_of("/");// k = 9
    string file_name; 
    if (k != string::npos)
        file_name = file.substr(k+1); // file = aaya.txt
    else
        file_name = file;

    int n = file.length();
    char filename[n+1];
    strcpy(filename, file.c_str()); //char * of file

    // int pos = file.find_last_of("/");
    // if(pos!=string::npos) {
    //     file = file.substr(pos+1);
    // }

    string destprime = destination + "/" + file_name; //full path of (not yet) copied file
    int nprime = destprime.length();
    char dest[nprime+1];
    strcpy(dest, destprime.c_str()); //char * of full path of (not yet) copied file

    ifstream infile(filename);
    ofstream outfile(dest);

    string line;
    if (infile && outfile) {
        while (getline(infile, line)) {
            outfile<<line<<endl;
        }
    }
    else if (!infile){
        cout << "!infile" <<endl;
        cout << filename << endl;
    }
    else{
        cout << "!outfile " << dest << endl;
    }
    infile.close();
    outfile.close();
}

void copyOneDir(string source, string dest) {  
    cout<<source<<endl<<dest<<endl;
    int k = source.find_last_of("/");
    string curr_file_name; 
    if(k!=string::npos)
        curr_file_name = source.substr(k+1); // insideHelloTest
    else
        curr_file_name = source;

    create_dir(dest, curr_file_name);
    string inSource = "./"+source; // ->  ./helloTest

    int n=inSource.length();
    char dirnamec[n+1];
    strcpy(dirnamec, inSource.c_str());
    DIR* dir = opendir(dirnamec);   // inside helloTest
    if (dir==NULL) {
        return; 
    }
    char cwd[256];
    getcwd(cwd, 256);
    char dot[] = ".", dd[]= "..";
    struct dirent* entity;
    entity = readdir(dir); // helloTest ke andar ki files read krega
    while (entity!=NULL) {
        // kuch mila -> 1. agar file hai -> copyonefile(new_source, new_dest) else
        //2. dir mili -> copyonedir(new_source, new_dest)
        if(entity->d_type==4) {
            if(strcmp(entity->d_name,dot) == 0 || strcmp(entity->d_name, dd) == 0) {
                cout << entity->d_name << endl;
                entity = readdir(dir);
                continue;
            }
            copyOneDir(source + "/" + toString(entity->d_name), dest+'/'+curr_file_name);
        }
        else {
            copyOneFile(source+ "/"+ toString(entity->d_name), dest+'/'+curr_file_name);
        }
        entity = readdir(dir);
    }
}

void Copy(string command, int pos) {
    string left = command.substr(pos+1);
    int pos1 = left.find_last_of(" ");    
    string dest = left.substr(pos1+1);

    if (dest[0]=='~')
        {
            dest = "/home" + dest.substr(1);
        }

    string sources = left.substr(0, pos1);
    while(!sources.empty()) {

        int posSor = sources.find_first_of(" ");
        string source = sources.substr(0, posSor); 
        sources = sources.substr(posSor+1);
        
        string par = ".";
        DIR* dir = opendir(par.c_str());
        struct dirent* entity;
        entity = readdir(dir);
        while(entity!=NULL && entity->d_name!=source)
            entity = readdir(dir);
        if(entity==NULL) {
            cerr << "Source "<< source <<" not found" << endl;
            continue;
        }
        
        if (entity->d_type==4){
            copyOneDir(source, dest);
        }
        else {
            copyOneFile(source, dest);
        }
        struct stat stt;
        stat(source.c_str(), &stt);
        chmod((dest+'/'+source).c_str(), stt.st_mode);
        closedir(dir);
        if(source == sources)
            break;
    }
}

void Rename(string command, int pos) {
    string left = command.substr(pos+1);
    int seperator = left.find_first_of(" ");
        
    string originalName = left.substr(0, seperator);
    string newName = left.substr(seperator+1);

    int n1=originalName.length();
    int n2=newName.length();

    char oNameC[n1+1], nNameC[n2+1];
    strcpy(oNameC, originalName.c_str());
    strcpy(nNameC, newName.c_str());

    rename(oNameC, nNameC);    
}

int remove_directory(const char *path) {
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d) {
      struct dirent *p;

      r = 0;
      while (!r && (p=readdir(d))) {
          int r2 = -1;
          char *buf;
          size_t len;

          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
             continue;

          len = path_len + strlen(p->d_name) + 2; 
          buf = (char*)malloc(len);

          if (buf) {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode))
                   r2 = remove_directory(buf);
                else
                   r2 = unlink(buf);
             }
             free(buf);
          }
          r = r2;
      }
      closedir(d);
   }

   if (!r)
      r = rmdir(path);

   return r;
}

 void delete_file(string path)
{
    char Path[100];
        strcpy(Path, path.c_str());
    int fd=remove(Path);
        if(fd==-1){
            cout<<"error in deleting file "<<endl;
            exit(0);
        }
}

bool found = false;

void search_directory (string directory, string entity) {
    char path[256];
    strcpy(path, directory.c_str());
    DIR *d = opendir(path);
    size_t path_len = strlen(path);

    if (d) {
        struct dirent *p;

        p = readdir(d);
        while (p!=NULL) {
//            cout << p->d_name << endl;
            char *buf;
            size_t len;

            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
                p = readdir(d);
                continue;
            }

            len = path_len + strlen(p->d_name) + 2; 
            buf = (char*)malloc(len);

            if (buf) {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {

                    if (S_ISDIR(statbuf.st_mode)) {
                        string temp = toString(buf);
                        int pos = temp.find_last_of("/");
                        string tempname = temp.substr(pos+1);

                        if(tempname == entity) {
                            cout << temp <<endl;
                            found = true;
                        }

                        search_directory(buf, entity);
                    }
                    else {
                        string temp = toString(buf);
                        int pos = temp.find_last_of("/");
                        string tempname = temp.substr(pos+1);

                        if(tempname == entity) {

                            cout << temp << endl;
                            found = true;
                        }

                    }
                }
                free(buf);
            }
            p = readdir(d);
        }
        closedir(d);
    }
}

void Move(string source, string dest){ // src = helloTest dest = ..
    struct stat s;
    if(stat(source.c_str(), &s)==0) {
        if(s.st_mode & S_IFREG) {
            cout << "Moving " << source << " to " << dest  << "\n";
            rename(source.c_str(), (dest+'/'+source).c_str());
        }
        else if(s.st_mode & S_IFDIR) {
            create_dir(dest, source);
            DIR* d=opendir(source.c_str());
            struct dirent *entity;
            entity = readdir(d);
            while(entity!=NULL) {
                if(strcmp(entity->d_name, "..")==0 || strcmp(entity->d_name, ".")==0)
                    {
                        entity = readdir(d);
                        continue;
                    }
                string new_source = source+'/'+entity->d_name;
                Move(new_source, dest);

                entity = readdir(d);
            }
            closedir(d);
            int x = remove_directory(source.c_str());
        }
        else
            cout << "small Error" <<endl;
    }
    else {
        cout << source << endl;
        cout << "BIG Error" <<endl;
    }
}

void moveHelper(string command){
    int pos1 = command.find_last_of(" ");    
    string dest = command.substr(pos1+1);

    if (dest[0]=='~') {
        dest = "/home" + dest.substr(1);
    }

    string sources = command.substr(0, pos1);
    while(!sources.empty()) {
        int posSor = sources.find_first_of(" ");
        string source = sources.substr(0, posSor); 
        sources = sources.substr(posSor+1);
        Move(source, dest);
        if(source == sources)
            break;
    }
}

void identifyCall(string command) {

    int pos = command.find_first_of(" ");
    string call = command.substr(0, pos);
    if (call=="create_file") {
        string s = command.substr(pos+1);
        int k = s.find_first_of(" ");
        
        create_file(s.substr(k+1), s.substr(0, k));
    }
    else if(call == "create_dir") {
        string s = command.substr(pos+1);
        int k = s.find_first_of(" ");

        create_dir(s.substr(k+1), s.substr(0,k));
    }
    else if(call == "goto") {
        Goto(command.substr(pos+1));
    }
    else if(call == "copy") {
        Copy(command, pos);
    }
    else if(call == "rename") {
        Rename(command, pos);   
    }
    else if(call == "move") {
        moveHelper(command.substr(pos+1));
    }
    else if(call == "delete_file") {
        delete_file(command.substr(pos+1));
    }
    else if(call == "delete_dir") {
        int a = remove_directory((command.substr(pos+1)).c_str());
    }
    else if(call == "search") {
        cout<<"\033[0;0H";
        found = false;
        search_directory(".", command.substr(pos+1));
        if(!found) 
            cout << "Not Found! ";
        cout << "Press any key..";
        fflush(stdout);
        char t = getch();
    }
        
}

void command_mode() {
    cout << "\033[2J\033[9999;1H";  //place cursor at bottom of window
    cout << "\033[7;32m" <<"copy     delete_file      delete_dir      goto";
    cout << "      create_file      create_dir      move      rename      search  ";
    cout<<"\033[1A\r";
    cout<<"\033[0m";
    cout<<":";
    string command="";
    char c;    

    bool flag = 0;

    do {
        fflush(stdout);
        c=getch();
        if (!flag && c=='q'){
            cout << "\033[2J\033[9999;1H";
            exit(0);
        }
        flag = 1;
        if ( c==127 )
            {
                command = command.substr(0, command.length()-1);
                cout << "\033[2J\033[9999;1H";
                cout << "\033[7;32m" <<"copy     delete_file      delete_dir      goto";
                cout << "      create_file      create_dir      move      rename      search  ";  
                cout<<"\033[1A\r";
                cout<<"\033[0m";
                cout<<":";
                cout << command;
            }
        else {
            cout<<c;
            command += c;
        }
    } while(c!=10 && c!=27);

    if(c==27)
        normal_mode(back.top()+"/");

    command = command.substr(0, command.length()-1);  //remove endl
    identifyCall(command);
    // command = "goto ./helloTest"; identifyCall(command);
    command_mode();
}

int main () {
    char cwd[256];
    getcwd(cwd, 256);
    back.push(toString(cwd));
    normal_mode(back.top()+"/");
    return 0;
}