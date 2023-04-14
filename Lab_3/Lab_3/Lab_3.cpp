#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

ifstream infile;
ofstream  outfile;

enum class TypeErrors //Error codes
{
    SYNTAX_ERROR,
    UNDEF_ID,
    ID_MISS,
    MISSING_SYMBOL,
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
};

class Analizer
{
private:
    vector<Var> varList; 
    int c = EOF;
    int last = -1; 
public:
    inline void GetSymbol(); 
    bool GetVarValue(string& str); 
    bool& GetVarAdress(string name); 

    void MethodS();
    bool MethodE();
    bool MethodT();
    bool MethodM();
    bool MethodI(string& str);
    bool& MethodL();
    bool_str MethodC(string& str);

    void Print(); 
    void PrintAll(); 
    void Run(); 
};

static void PrintError(TypeErrors typeer, std::string param = "")
{
    switch (typeer)
    {
    case TypeErrors::SYNTAX_ERROR:
        std::cout << "Error: Syntax error. " << std::endl;
        break;
    case TypeErrors::ID_MISS:
        std::cout << "Error: Invalid character in identifier. " << param << std::endl;
        break;
    case TypeErrors::UNDEF_ID:
        std::cout << "Error: Undefined identifier: " << param << std::endl;
        break;
    case TypeErrors::MISSING_SYMBOL:
        std::cout << "Error: " << param << "missing." << std::endl;
        break;
    }
    infile.close();
    exit(1);
}


int main()
{
    setlocale(LC_ALL, "ru");

    infile.open("text.txt");
    if (!infile.is_open()) {
        cout << "Файл не удалось открыть!";
        return 1;
    }
    
    Analizer start;
    start.Run();

    infile.close();

    system("pause");
	return 0;
}

inline void Analizer::GetSymbol()
{
    c = infile.get();
}

bool Analizer::GetVarValue(string& str)
{
    if (str.empty() || str.size() == 0)
        PrintError(TypeErrors::SYNTAX_ERROR);

    for (int i = 0; i < varList.size(); i++)
        if (varList[i].IdName == str)
            return varList[i].value;

    PrintError(TypeErrors::UNDEF_ID, str);
    return false;
}

bool& Analizer::GetVarAdress(string name)
{
    if (name.empty() || name.size() == 0)
        PrintError(TypeErrors::SYNTAX_ERROR);

    for (int i = 0; i < varList.size(); i++)
        if (varList[i].IdName == name)
            return varList[last = i].value;

    varList.push_back(Var{ name, true });
    last = varList.size() - 1;
    return varList[last].value;
}

void Analizer::MethodS()
{
    if (c != '(')
        PrintError(TypeErrors::MISSING_SYMBOL, "\'=\'");
    GetSymbol();
    bool& p = MethodL();
    if (c != ',')
        PrintError(TypeErrors::MISSING_SYMBOL, "\'=\'");
    GetSymbol();
    p = MethodE();
    while (isspace(c))
    {
        GetSymbol();
    }
    if (c != ')' )
        PrintError(TypeErrors::MISSING_SYMBOL, "\'=\'");
    GetSymbol();
}

bool Analizer::MethodE()
{
    bool x = MethodT();
    while (c == '|')
    {
        GetSymbol();
        x |= MethodT();
    }
    return x;
}

bool Analizer::MethodT()
{
    bool x = MethodM();
    while (c == '&')
    {
        GetSymbol();
        x &= MethodM();
    }
    return x;
}

bool Analizer::MethodM()
{
    string temp_bool_str = "";
    bool x;
    if (c == '(')
    {
        GetSymbol();
        x = MethodE();
        if (c != ')')
            PrintError(TypeErrors::MISSING_SYMBOL, "\')\'");
        GetSymbol();
    }
    else
    {
        if (c == '~')
        {
            GetSymbol();
            x = !MethodM();
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
                    if (c >= 'a' && c <= 'z')
                        x = MethodI(temp_bool_str);
                    else
                        PrintError(TypeErrors::SYNTAX_ERROR);
                }
            else
                if (c >= 'a' && c <= 'z')
                    x = MethodI(temp_bool_str);
                else
                    PrintError(TypeErrors::SYNTAX_ERROR);
    }

    return x;
}

bool Analizer::MethodI(string& str)
{
    bool flag = false;
    while ((c >= 'a' && c <= 'z'))
    {
        str.push_back(c);
        GetSymbol();
        /*if (!(c >= 'a' && c <= 'z') && c != '=')
            flag = false;
        else*/
            flag = true;
    }
    if (!flag)
        PrintError(TypeErrors::ID_MISS, string(1, (char)c));
    return GetVarValue(str);
}

bool& Analizer::MethodL()
{
    string str;
    bool flag = false;
    while ((c >= 'a' && c <= 'z'))
    {
        str.push_back(c);
        GetSymbol();
        /*if (!(c >= 'a' && c <= 'z') && c != ',')
            flag = false;
        else*/
            flag = true;
    }
    if (!flag)
        PrintError(TypeErrors::ID_MISS, string(1, (char)c));
    return GetVarAdress(str);
}

bool_str Analizer::MethodC(string& str)
{
    str += c;
    string temp_true = "true ";
    string temp_false = "false ";
    for (int i = 0;; GetSymbol(), str += c, i++)
    {
        if (str[0] == 't')
        {
            if (c == temp_true[i] && i <= 3)
            {
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

void Analizer::Print()
{
    string txt;
    if (varList.size() == 0)
        cout << "No varibales defined yet.\n";
    else
        txt = varList[last].value ? "true" : "false";
        cout << varList[last].IdName << " = " << txt << endl;
}

void Analizer::PrintAll()
{
    string txt;
    if (!varList.size())
        printf("No variables defined yet.\n");
    else
        for (int i = 0; i < varList.size(); i++)
        {
            txt = varList[last].value ? "true" : "false";
            cout << varList[i].IdName << " = " << txt << endl;
        }
}

void Analizer::Run()
{
    int i = 0;
    GetSymbol();
    for (i; ; i++)
    {
        while (c >= 0 && c <= ' ')
            GetSymbol();
        if (c == EOF)
            break;
        cout << "Оператор: " << i + 1 << endl;
        MethodS();
        Print();
    }
    cout << "\nРезультат: \n\tКоличество переменных:" << varList.size() << "\n\tКоличество операторов:" << i << "\n\nКонечные значения:\n";
    PrintAll();
}

Var::Var(const std::string& src, bool value) :IdName{ src }, value{ value } {}