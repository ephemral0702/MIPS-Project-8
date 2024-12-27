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

vector<string> ReadInput(string fliepath)
{
    vector<string> input;
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

int main() {
    vector<string> input = ReadInput("../inputs/test3.txt");
    for (string line : input)
    {
        cout << line << endl;
        cout << GetInstruction(line) << endl <<endl;
    }
    return 0;
}