#include<iostream>
#include<fstream>
#include<vector>
#include<deque>
#include<queue>
#include<string>
#include<windows.h>

using namespace std;

int cycle = 0;
int PC=0;//word address
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
    PC = 0;
}

void ReadFile(const string &folder_path)
{
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

enum stage{IF,ID,EX,MEM,WB,Done};

string EnumtoString(enum stage s)
{
    switch (s){
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

struct Instruction
{
    string instruction;
    int rd;
    int rs; 
    int rt; 
    int constant;//也存beq的offset
    stage current_stage;
};

Instruction process_instruction(string input)
{
    Instruction inst;
    for(int i=0;i<input.length();i++)
    {
        if(input[i]==' ')
        {
            inst.instruction = input.substr(0,i);
            input = input.substr(i+1);
            break;
        }
    }
    if(inst.instruction == "lw" || inst.instruction == "sw")
    {
        for(int i=0;i<input.length();i++){
            if(input[i]=='$')
            {
                inst.rt = input[i+1] - '0';
                input = input.substr(i+3);
                break;
            }
        }
        for(int i = 0,j = 0;i<input.length();i++){
            if(input[i] == '(')
            {
                inst.constant = stoi(input.substr(j,i-j));
            }
            if(input[i] == '$')
            {
                inst.rs = input[i+1] - '0';
                break;
            }
        }
        
    }
    else if(inst.instruction == "add" || inst.instruction == "sub")
    {
        for(int i=0;i<input.length();i++){
            if(input[i]=='$')
            {
                inst.rd = input[i+1] - '0';
                input = input.substr(i+3);
                break;
            }
        }
        for(int i=0;i<input.length();i++){
            if(input[i]=='$')
            {
                inst.rs = input[i+1] - '0';
                input = input.substr(i+3);
                break;
            }
        }
        for(int i=0;i<input.length();i++){
            if(input[i]=='$')
            {
                inst.rt = input[i+1] - '0';
                break;
            }
        }
    }
    else
    {
        for(int i=0;i<input.length();i++){
            if(input[i]=='$')
            {
                inst.rs = input[i+1] - '0';
                input = input.substr(i+3);
                break;
            }
        }
        for(int i=0;i<input.length();i++){
            if(input[i]=='$')
            {
                inst.rt = input[i+1] - '0';
                input = input.substr(i+3);
                break;
            }
        }
        inst.constant = stoi(input);
    }
    inst.current_stage = IF;

    return inst;
}

int main(){

    init();
    ReadFile(folder_path);
    int ca=0;
    while(!Inputs.empty()){

        cout<<"Case"<<(++ca)<<endl;

        ofstream output("../outputs/"+filenames.front());
        filenames.pop();
        init();
        
        deque<string> input = Inputs.front();
        deque<Instruction> instructions;

        Inputs.pop();

        instructions.push_back(process_instruction(input[PC]));
        int stall = 0;
        bool taken = false;
        while(!instructions.empty()){
            cout<<PC<<endl;
            //cout<<"Cycle "<<(++cycle)<<endl;
            output<<"Cycle "<<++cycle<<endl;
            PC++;
            for(int i=0;i<instructions.size();i++){
                string instruction = instructions[i].instruction;
                stage current_stage = instructions[i].current_stage;

                //cout<<instructions[i].instruction<<":"<<EnumtoString(current_stage)<<endl;
                output<<instructions[i].instruction<<":"<<EnumtoString(current_stage);

                if(current_stage==IF)
                {
                    output<<endl;
                    if(taken){
                        instructions.pop_back();
                        taken = false;
                    }
                    if(stall) continue;
                    instructions[i].current_stage = ID;
                }
                else if(current_stage==ID)
                {
                    output<<endl;
                    if(stall) continue;
                    if(instruction == "beq"){
                        cout<<Register[instructions[i].rs]<<" "<<Register[instructions[i].rt]<<endl;
                        if(Register[instructions[i].rs] == Register[instructions[i].rt]){
                            taken = true;
                            PC = PC + instructions[i].constant - 1; //PC++ && predict not taken
                        }
                    }
                    instructions[i].current_stage = EX;
                }
                else if(current_stage==EX)
                {
                    if(instructions[i].instruction=="add")
                    {   
                        output<<" RegDst=1 ALUSrc=0 Branch=0 MemRead=0 MemWrite=0 RegWrite=1 MemtoReg=0";
                    }
                    else if(instructions[i].instruction=="sub")
                    {   
                        output<<" RegDst=1 ALUSrc=0 Branch=0 MemRead=0 MemWrite=0 RegWrite=1 MemtoReg=0";
                    }
                    else if(instructions[i].instruction=="lw")
                    {
                        output<<" RegDst=0 ALUSrc=1 Branch=0 MemRead=1 MemWrite=0 RegWrite=1 MemtoReg=1";
                    }
                    else if(instructions[i].instruction=="sw")
                    {
                        output<<" RegDst=X ALUSrc=1 Branch=0 MemRead=0 MemWrite=1 RegWrite=0 MemtoReg=X";
                    }
                    else if(instructions[i].instruction=="beq")
                    {
                        output<<" RegDst=X ALUSrc=0 Branch=1 MemRead=0 MemWrite=0 RegWrite=0 MemtoReg=X";
                    }
                    if (instructions[i].instruction=="lw" && i+1 < instructions.size()){
                        if ((instructions[i].rt == instructions[i+1].rs) || (instructions[i].rt == instructions[i+1].rt)){
                            if (instructions[i+1].instruction == "beq"){
                                stall = 2;
                            }
                            else{  // add, sub, ...
                                stall = 1;
                            }
                        }
                    }
                    if ((instructions[i].instruction=="sub" || instructions[i].instruction=="add") && i+1 < instructions.size()){
                        if(instructions[i+1].instruction == "beq" && ((instructions[i].rd == instructions[i + 1].rs )|| (instructions[i].rd == instructions[i + 1].rt))){
                            stall = 1;
                            cout<<"Hello world"<<endl;
                        }
                    }
                    
                    output<<endl;
                    instructions[i].current_stage = MEM;
                }
                else if(current_stage==MEM)
                {
                    if(instructions[i].instruction=="add")
                    {   
                        Register[instructions[i].rd]=Register[instructions[i].rs]+Register[instructions[i].rt];
                        output<<" Branch=0 MemRead=0 MemWrite=0 RegWrite=1 MemtoReg=0";
                    }
                    else if(instructions[i].instruction=="sub")
                    {   
                        Register[instructions[i].rd]=Register[instructions[i].rs]-Register[instructions[i].rt];
                        output<<" Branch=0 MemRead=0 MemWrite=0 RegWrite=1 MemtoReg=0";
                    }
                    else if(instructions[i].instruction=="lw")
                    {
                        output<<" Branch=0 MemRead=1 MemWrite=0 RegWrite=1 MemtoReg=1";
                    }
                    else if(instructions[i].instruction=="sw")
                    {
                        output<<" Branch=0 MemRead=0 MemWrite=1 RegWrite=0 MemtoReg=X";
                    }
                    else if(instructions[i].instruction=="beq")
                    {
                        output<<" Branch=1 MemRead=0 MemWrite=0 RegWrite=0 MemtoReg=X";
                    }
                    output<<endl;
                    instructions[i].current_stage = WB;
                }
                else if(current_stage==WB)
                {
                    if(instructions[i].instruction=="add")
                    {
                        output<<" RegWrite=1 MemtoReg=0";
                    }
                    else if(instructions[i].instruction=="sub")
                    {
                        output<<" RegWrite=1 MemtoReg=0";
                    }
                    else if(instructions[i].instruction=="lw")
                    {
                        Register[instructions[i].rt] = Memory[instructions[i].constant / 4 + instructions[i].rs];
                        output<<" RegWrite=1 MemtoReg=1";
                    }
                    else if(instructions[i].instruction=="sw")
                    {
                        int offset=instructions[i].constant/4+instructions[i].rs;
                        Memory[offset]=Register[instructions[i].rt];
                        output<<" RegWrite=0 MemtoReg=X";
                    }
                    else if(instructions[i].instruction=="beq")
                    {
                        output<<" RegWrite=0 MemtoReg=X";
                    }
                    output<<endl;
                    instructions[i].current_stage = Done;
                }
            }
            if(instructions[0].current_stage==Done)
            {
                instructions.pop_front();
            }
            if(PC<input.size() && stall == 0)
            {
                instructions.push_back(process_instruction(input[PC]));
            }
            if (stall != 0){
                PC--;
                stall--;
            }
        }
        output<<endl<<"## Final Result:"<<endl;
        output<<"Total Cycles: "<<cycle<<endl<<endl;
        output<<"Final Register Values:"<<endl;
        for(int i=0;i<Register_num;i++)
        {
            output<<Register[i]<<" ";
        }
        output<<endl<<"Final Memory Values:"<<endl;
        for(int i=0;i<Memory_size;i++)
        {
            output<<Memory[i]<<" ";
        }
        output.close();
    }
    cout<<"Finish"<<endl;
    return 0;
}