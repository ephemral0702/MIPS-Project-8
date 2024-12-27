#include<bits/stdc++.h>
using namespace std;

const int Register_num = 32;
const int Memory_size = 32;
vector<int> Register(Register_num);
vector<int> Memory(Memory_size);

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
}

deque<string> ReadInput(string fliepath)
{
    deque<string> input;
    ifstream file(fliepath);
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            input.push_back(line);
        }
        file.close();
    }
    return input;
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

enum stage
{
    IF,
    ID,
    EX,
    MEM,
    WB
};

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
    deque<string> input = ReadInput("../inputs/test3.txt");
    init();
    int cycle = 0;
    deque<pair<string,stage>> instructions;
    instructions.push_back({input.front(),IF});
    input.pop_front();
    while(!instructions.empty())
    {
        cout<<"Cycle "<<(++cycle)<<endl;
        for(int i=0;i<instructions.size();i++)
        {
            string instruction = instructions[i].first;
            stage current_stage = instructions[i].second;
            string current_instruction = GetInstruction(instruction);
            cout<<current_instruction<<":"<<EnumtoString(current_stage)<<endl;
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

    return 0;
}