#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

ifstream infile;
ofstream outfile;
int IdTriad = 0;


enum TypeErrors
{
    SYNTAX_ERROR,       //Синтаксическая ошибка
    UNDEF_ID,           //Неопределенный идентификатор
    ID_MISS,            //Недопустимый символ в идентификаторе
    MISSING_SYMBOL,     //Отсутствующий символ
};

enum bool_str
{
    FALSE,
    TRUE,
    VAR
};

class Var
{
private:
    string IdName;
    bool value;
public:
    Var(const string& src, bool value);
    string GetIdName();
};

class Analyzer
{
private:
    vector<Var> varList;
    int c = EOF;
public:
    inline void GetSymbol();
    inline void GetSymbolSkipSpace();
    long GetVarValue(string& str);
    long GetVarAdress(string name);

    void MethodS();
    long MethodE();
    long MethodT();
    long MethodM();
    long MethodL();
    long MethodI(string& str);
    long MethodC(string& str);

    void Run();
};

static void PrintError(TypeErrors typeer, string param = "")
{
    switch (typeer)
    {
    case SYNTAX_ERROR:
        cout << "Error: Syntax error. " << "\n" << endl;
        break;
    case ID_MISS:
        cout << "Error: Invalid character in identifier. " << param << "\n" << endl;
        break;
    case UNDEF_ID:
        cout << "Error: Undefined identifier: " << param << "\n" << endl;
        break;
    case MISSING_SYMBOL:
        cout << "Error: " << param << "missing.\n" << endl;
        break;
    }
    infile.close();
    outfile.close();
    exit(2);
}


int main()
{
    setlocale(LC_ALL, "ru");

    infile.open("text.txt");
    if (!infile.is_open()) {
        cout << "The file could not be opened!";
        return 1;
    }
    outfile.open("outfile.txt");
    if (!outfile.is_open()) {
        infile.close();
        cout << "The file could not be opened!";
        return 1;
    }

    Analyzer start;
    start.Run();

    infile.close();
    outfile.close();
    system("pause");
    return 0;
}

void Analyzer::Run()
{
    int i = 0;
    GetSymbol();
    for (i; ; i++)
    {
        while(c >= 0 && c <= ' ')
            GetSymbolSkipSpace();
        if (c == EOF)
            break;
        outfile << i+1 << ")";
        MethodS();
    }
}

inline void Analyzer::GetSymbol()
{
    c = infile.get();
}

inline void Analyzer::GetSymbolSkipSpace()
{
    c = infile.get();
    while (c >= 0 && c <= ' ')
        GetSymbolSkipSpace();
}

long Analyzer::GetVarValue(string& str)
{
    if (str.empty() || str.size() == 0)
        PrintError(SYNTAX_ERROR);

    for (int i = 0; i < varList.size(); i++)
        if (varList[i].GetIdName() == str)
        {
            IdTriad++;
            outfile << "\t" << IdTriad << ":  " << "V(" << str << ", @)\n";
            return IdTriad;
        }

    PrintError(UNDEF_ID, str);
    return 0;
}

long Analyzer::GetVarAdress(string name)
{
    if (name.empty() || name.size() == 0)
        PrintError(SYNTAX_ERROR);

    for (int i = 0; i < varList.size(); i++)
        if (varList[i].GetIdName() == name)
        {
            IdTriad++;
            outfile << "\t" << IdTriad << ":  " << "V(" << name << ", @)\n";
            return IdTriad;
        }

    varList.push_back(Var{ name, 0 });
    IdTriad++;
    outfile << "\t" << IdTriad << ":  " << "V(" << name << ", @)\n";
    return IdTriad;
}

void Analyzer::MethodS()
{
    if (c != '(')
        PrintError(MISSING_SYMBOL, "\'(\'");
    GetSymbolSkipSpace();
    long p1 = MethodL();
    if (c != ',')
        PrintError(MISSING_SYMBOL, "\',\'");
    GetSymbolSkipSpace();
    long p2 = MethodE();
    if (c != ')')
        PrintError(MISSING_SYMBOL, "\')\'");
    GetSymbol();
    if (!isspace(c) && c != EOF)
        PrintError(SYNTAX_ERROR);

    IdTriad++;
    outfile << "\t" << IdTriad << ":  " << "=(^" << p1 << ", ^" << p2 << ")\n";
    GetSymbolSkipSpace();
}

long Analyzer::MethodL()
{
    string str;
    bool flag = false;
    while ((c >= 'a' && c <= 'z'))
    {
        str.push_back(c);
        GetSymbol();
        if (c == ' ')
        {
            GetSymbolSkipSpace();
            if (c != ',')
            {
                flag = false;
                break;
            }
        }
        else if (!(c >= 'a' && c <= 'z') && c != ',')
            flag = false;
        else
            flag = true;
    }
    if (!flag)
        PrintError(ID_MISS, string(1, char(c)));
    if (str == "true" || str == "false")
        PrintError(SYNTAX_ERROR);
    return GetVarAdress(str);
}

long Analyzer::MethodE()
{
    long x1 = MethodT();
    while (c == '|')
    {
        long x2;
        GetSymbolSkipSpace();
        x2 |= MethodT();
        IdTriad++;
        outfile << "\t" << IdTriad << ":  " << "|(^" << x1 << ", " << "^" << x2 << ")\n";
        x1 = IdTriad;
    }
    return x1;
}

long Analyzer::MethodT()
{
    long x1 = MethodM();
    while (c == '&')
    {
        long x2;
        GetSymbolSkipSpace();
        x2 &= MethodM();
        IdTriad++;
        outfile << "\t" << IdTriad << ":  " << "&(^" << x1 << ", " << "^" << x2 << ")\n";
        x1 = IdTriad;
    }
    return x1;
}

long Analyzer::MethodM()
{
    string temp_bool_str = "";
    long x;
    if (c == '(')
    {
        GetSymbolSkipSpace();
        x = MethodE();
        if (c == ' ')
            GetSymbolSkipSpace();
        if (c != ')')
            PrintError(MISSING_SYMBOL, "\')\'");
        GetSymbolSkipSpace();
    }
    else
    {
        if (c == '~')
        {
            GetSymbolSkipSpace();
            x = MethodM();
            IdTriad++;
            outfile << "\t" << IdTriad << ":  " << "~(^" << x << ", @)\n";
            x = IdTriad;
            if (c == ' ')
                GetSymbolSkipSpace();
        }
        else
            for (string temp_var = ""; ;GetSymbol())
            {                                                                 
                if (c >= 'a' && c <= 'z') {
                    temp_var += c;
                }
                else if (temp_var == "true" || temp_var == "false") {
                    x = MethodC(temp_var);
                    break;
                }
                else if (!temp_var.empty()) {
                    x = MethodI(temp_var);
                    break;
                }
                else {
                    PrintError(SYNTAX_ERROR);
                }
            }
    }
    if (c == ' ')
        GetSymbolSkipSpace();
    return x;
}

long Analyzer::MethodC(string& str)
{
    bool x;
    if (str[0] == 't') 
    {
        x = true;
    }
    else
    {
        x = false;
    }
    IdTriad++;
    outfile << "\t" << IdTriad << ":  " << "C(" << x << ", @)\n";
    return IdTriad;
}

long Analyzer::MethodI(string& str)
{

    bool flag = false;

    if (c == ' ') 
    {
        GetSymbolSkipSpace();
    }
    if (!str.empty())
    {
        flag = true;
    }
    if (c != '|' && c != '&' && c != ')')
    {
        flag = false;
    }
    if (!flag)
        PrintError(ID_MISS, string(1, char(c)));
    return GetVarValue(str);
}


Var::Var(const string& src, bool value) : IdName(src), value(value) {}

string Var::GetIdName()
{
    return this->IdName;
}
