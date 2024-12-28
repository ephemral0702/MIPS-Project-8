#include<iostream>
#include<fstream>
#include<vector>
#include<deque>
#include<utility>
#include<queue>
#include<string>
#include<windows.h>

using namespace std;

int cycle = 0;
const string folder_path = "../inputs\\*";
const int Register_num = 32;
const int Memory_size = 32;
vector<int> Register(Register_num);
vector<int> Memory(Memory_size);
queue<string> filenames;
queue<deque<string>> Inputs;

void init()
{
    Register[0] = 0;
    for (int i = 1; i < Register_num; i++)
    {
        Register[i] = 1;
    }
    for (int i = 0; i < Memory_size; i++)
    {
        Memory[i] = 1;
    }
    cycle = 0;
}

void ReadFile(const string &folder_path){
    WIN32_FIND_DATA ToFind; //WIN32_FIND_DATA 是一個typedef 用來存放檔案或資料夾的資料 例如檔案名稱、檔案大小、建立時間等等
    HANDLE hFind = FindFirstFile(folder_path.c_str(), &ToFind); //打開資料夾,HANDLE 是一個指標 用來指向一個資料夾或檔案

    if (hFind == INVALID_HANDLE_VALUE) {    //INVALID_HANDLE_VALUE 表示沒有找到資料夾
        cerr << "No folder: " << folder_path << '\n'; 
        return;
    } 

    do {
        if(ToFind.cFileName[0]=='.') continue; //如果是"."或".."就跳過
        string filename = (string)ToFind.cFileName;
        string filepath = "../inputs/" + filename;
        filenames.push(filename);
        deque<string> input;
        ifstream file(filepath);
        if (file.is_open())
        {
            string line;
            while (getline(file, line))
            {
                input.push_back(line);
            }
            file.close();
            Inputs.push(input);
        }     
    } while (FindNextFile(hFind, &ToFind) != 0);    //找下一個檔案

    FindClose(hFind);
}

string GetInstruction(string line)
{
    for(int i=0;i<line.length();i++)
    {
        if(line[i]==' ')
        {
            return line.substr(0,i);
        }
    }
    return "";
}

enum stage{IF,ID,EX,MEM,WB};

string EnumtoString(enum stage s)
{
    switch (s)
    {
    case IF:
        return "IF";
    case ID:
        return "ID";
    case EX:
        return "EX";
    case MEM:
        return "MEM";
    case WB:
        return "WB";
    default:
        return "ERROR";
    }
}

int main() {

    init();
    ReadFile(folder_path);
    while(!Inputs.empty()){

        ofstream output("../outputs/"+filenames.front());
        filenames.pop();
        init();
        deque<string> input = Inputs.front();
        deque<pair<string,stage>> instructions;

        Inputs.pop();

        instructions.push_back({input.front(),IF});
        input.pop_front();

        while(!instructions.empty()){
            //cout<<"Cycle "<<(++cycle)<<endl;
            output<<"Cycle "<<cycle<<endl;
            for(int i=0;i<instructions.size();i++){
                string instruction = instructions[i].first;
                stage current_stage = instructions[i].second;
                string current_instruction = GetInstruction(instruction);
                //cout<<current_instruction<<":"<<EnumtoString(current_stage)<<endl;
                output<<current_instruction<<":"<<EnumtoString(current_stage)<<endl;
                if(current_stage==IF)
                {
                    instructions[i].second = ID;
                }
                else if(current_stage==ID)
                {
                    instructions[i].second = EX;
                }
                else if(current_stage==EX)
                {
                    instructions[i].second = MEM;
                }
                else if(current_stage==MEM)
                {
                    instructions[i].second = WB;
                }
                else if(current_stage==WB)
                {
                    instructions.pop_front();
                }
            }
            if(!input.empty())
            {
                instructions.push_back({input.front(),IF});
                input.pop_front();
            }
        }
        output.close();
    }
    cout<<"Finish"<<endl;
    return 0;
}