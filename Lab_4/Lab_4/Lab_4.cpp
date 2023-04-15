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
public:
    string IdName;
    bool value;

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
    bool GetVarValue(string& str);
    long GetVarAdress(string name);

    void MethodS();
    bool MethodE();
    bool MethodT();
    bool MethodM();
    long MethodL();
    bool MethodI(string& str);
    bool_str MethodC(string& str);

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
        while(c >= 0 || c <= ' ')
            GetSymbolSkipSpace();
        if (c == EOF)
            break;
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

bool Analyzer::GetVarValue(string& str)
{
    if (str.empty() || str.size() == 0)
        PrintError(SYNTAX_ERROR);

    for (int i = 0; i < varList.size(); i++)
        if (varList[i].IdName == str)
            return varList[i].value;

    PrintError(UNDEF_ID, str);
    return false;
}

long Analyzer::GetVarAdress(string name)
{
    if (name.empty() || name.size() == 0)
        PrintError(SYNTAX_ERROR);

    for (int i = 0; i < varList.size(); i++)
        if (varList[i].GetIdName() == name)
        {
            IdTriad++;
            outfile << IdTriad << " : " << "V(" << name << ", @)\n";
            return IdTriad;
        }

    varList.push_back(Var{ name, 0 });
    IdTriad++;
    outfile << IdTriad << " : " << "V(" << name << ", @)\n";
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
    GetSymbol;
    if (!isspace(c))
        PrintError(SYNTAX_ERROR);

    IdTriad++;
    outfile << IdTriad << " : " << "=(^" << p1 << ", ^" << p2 << ")\n";
    GetSymbolSkipSpace();
}

bool Analyzer::MethodE()
{
    long x1 = MethodT();
    while (c == '|')
    {
        long x2;
        GetSymbolSkipSpace();
        x2 |= MethodT();
        IdTriad++;
        outfile << IdTriad << " : " << "|(^" << x1 << ", " << "^" << x2 << ")\n";
        x1 = IdTriad;
    }
    return x1;
}

bool Analyzer::MethodT()
{
    long x1 = MethodM();
    while (c == '&')
    {
        long x2;
        GetSymbolSkipSpace();
        x2 &= MethodM();
        IdTriad++;
        outfile << IdTriad << " : " << "&(^" << x1 << ", " << "^" << x2 << ")\n";
        x1 = IdTriad;
    }
    return x1;
}

bool Analyzer::MethodM()
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
            x = !MethodM();

            if (c == ' ')
                GetSymbolSkipSpace();
        }
        else
            if (c == 't' || c == 'f')
                switch (MethodC(temp_bool_str))
                {
                case TRUE:
                    x = true;
                    break;
                case FALSE:
                    x = false;
                    break;
                case VAR:
                    if ((c >= 'a' && c <= 'z') || !temp_bool_str.empty())
                        x = MethodI(temp_bool_str);
                    else
                        PrintError(SYNTAX_ERROR);
                }
            else
                if (c >= 'a' && c <= 'z')
                    x = MethodI(temp_bool_str);
                else
                    PrintError(SYNTAX_ERROR);
    }
    if (c == ' ')
        GetSymbolSkipSpace();
    return x;
}

bool_str Analyzer::MethodC(string& str)
{
    str += c;
    string temp_true = "true ";
    string temp_false = "false ";
    GetSymbol();
    for (int i = 1;; GetSymbol(), i++)
    {
        if (str[0] == 't')
        {
            if (c == temp_true[i] && i <= 3)
            {
                str += c;
                continue;
            }
            else if (!(c >= 'a' && c <= 'z') && i == 4)
            {
                return TRUE;
            }
            else
            {
                break;
            }
        }
        else
        {
            if (c == temp_false[i] && i <= 4)
            {
                str += c;
                continue;
            }
            else if (!(c >= 'a' && c <= 'z') && i == 5)
            {
                return FALSE;
            }
            else
            {
                break;
            }
        }
    }
    return VAR;
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

bool Analyzer::MethodI(string& str)
{

    bool flag = false;

    if (!str.empty())
    {
        flag = true;
    }
    while ((c >= 'a' && c <= 'z'))
    {
        str.push_back(c);
        GetSymbol();
        if (c == ' ')
        {
            GetSymbolSkipSpace();
            if (c != '|' && c != '&' && c != ')')
            {
                flag = false;
                break;
            }
        }
        else if (!(c >= 'a' && c <= 'z') && c != '|' && c != '&' && c != ')')
            flag = false;
        else
            flag = true;
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
