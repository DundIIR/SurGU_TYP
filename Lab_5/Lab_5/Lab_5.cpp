#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

ifstream infile;
ofstream outfile1;
ofstream outfile2;
int IdTriad = 0;

enum TypeErrors
{
    SYNTAX_ERROR,       //Синтаксическая ошибка
    UNDEF_ID,           //Неопределенный идентификатор
    ID_MISS,            //Недопустимый символ в идентификаторе
    MISSING_SYMBOL,     //Отсутствующий символ
};

class Base
{
public:
    virtual string GetOperand() = 0;
    virtual ~Base();
};

class Var : public Base
{
private:
    string IdName;
public:
    Var(const string& src);
    string GetOperand() override;
};

class Link : public Base
{
public:
    long linkTriad;

    Link(const long linkTriad);
    string GetOperand() override;
};

class Constant : public Base
{
public:
    bool value;

    Constant(const bool value);
    string GetOperand() override;
};

class None : public Base
{
    string GetOperand() override;
};



class Triad
{
public:
    char operation;
    Base* leftOperand;
    Base* rightOperand;
    bool isDelete = false;

    Triad(char operation, Base* leftOperand, Base* rightOperand);
    void OutTriad();
};


class Analyzer
{
private:
    vector<Var> varList;
    vector<Triad> triadList;
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

    void Optimization();
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
    outfile1.close();
    outfile2.close();
    exit(2);
}



int main()
{
    setlocale(LC_ALL, "ru");

    infile.open("infile.txt");
    outfile1.open("outfile1.txt");
    outfile2.open("outfile2.txt");
    if (!infile.is_open() || !outfile1.is_open() || !outfile2.is_open()) {
        cout << "The file could not be opened!";
        return 1;
    }

    Analyzer start;
    start.Run();

    infile.close();
    outfile1.close();
    outfile2.close();
    system("pause");
    return 0;
}

void Analyzer::Run()
{
    int i = 0;
    GetSymbol();
    for (i; ; i++)
    {
        while (c >= 0 && c <= ' ')
            GetSymbolSkipSpace();
        if (c == EOF)
            break;
        MethodS();
    }

    for (int i = 0, j = 1; i < triadList.size(); i++)
    {
        if(j == 1)
            outfile1 << j++ << ")";

        outfile1 << "\t" << i + 1 << " : ";
        triadList[i].OutTriad();

        if (triadList[i].operation == '=' && i != triadList.size()-1)
            outfile1 << j++ << ")";
    }

    Optimization();

    for (int i = 0, j = 1; i < triadList.size(); i++)
    {
        if (j == 1)
            outfile2 << j++ << ")";

        outfile2 << "\t" <<  i + 1 << " : ";
        if (!triadList[i].isDelete)
            outfile2 << triadList[i].operation << '(' << triadList[i].leftOperand->GetOperand() << ", " << triadList[i].rightOperand->GetOperand() << ')' << endl;
        else
            outfile2 << endl;

        if (triadList[i].operation == '=' && i != triadList.size() - 1)
            outfile2 << j++ << ")";
    }
}

void Analyzer::Optimization()
{
    int left_id = 0, right_id = 0;

    for (int i = 0; i < triadList.size(); i++)
    {
        try {
            left_id = stoi(triadList[i].leftOperand->GetOperand().substr(1)) - 1;
        }
        catch (const std::exception& e) {
            left_id = 0;
        }
        try {
            right_id = stoi(triadList[i].rightOperand->GetOperand().substr(1)) - 1;
        }
        catch (const std::exception& e) {
            right_id = 0;
        }
        try {
            switch (triadList[i].operation)
            {
            case '|': 
            case '&':
                if (triadList[right_id].operation == 'C')
                {
                    triadList[right_id].isDelete = true;
                    triadList[i].rightOperand = new Constant{ stoi(triadList[right_id].leftOperand->GetOperand()) != 0 };
                    if (triadList[left_id].operation == 'C')
                    {
                        triadList[left_id].isDelete = true;
                        triadList[i].leftOperand = new Constant{ stoi(triadList[left_id].leftOperand->GetOperand()) != 0 };

                        if (triadList[i].operation == '|')
                        
                            triadList[i].leftOperand = new Constant{ (stoi(triadList[left_id].leftOperand->GetOperand()) +
                                                                      stoi(triadList[right_id].leftOperand->GetOperand())) != 0 };
                        
                        else
                        
                            triadList[i].leftOperand = new Constant{ (stoi(triadList[left_id].leftOperand->GetOperand()) *
                                                                      stoi(triadList[right_id].leftOperand->GetOperand())) != 0 };
                        
                        triadList[i].operation = 'C';
                        triadList[i].rightOperand = new None{};
                    }
                }
                break;
            case '~':
                if (triadList[left_id].operation == 'C')
                {
                    triadList[left_id].isDelete = true;
                    triadList[i].leftOperand = new Constant{ !((stoi(triadList[left_id].leftOperand->GetOperand())) != 0) };
                    triadList[i].operation = 'C';
                }
                break;
            case '=':
                if (triadList[left_id].operation == 'V')
                {
                    triadList[left_id].isDelete = true;
                    triadList[i].leftOperand = new Var{ triadList[left_id].leftOperand->GetOperand() };
                }
            }
        }
        catch (const std::exception& e) {
            left_id = 0;
            right_id = 0;
        }
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
        if (varList[i].GetOperand() == str)
        {
            Triad T('V', new Var{ str }, new None{});
            triadList.push_back(T);
            IdTriad++;
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
        if (varList[i].GetOperand() == name)
        {
            Triad T('V', new Var{ name }, new None{});
            triadList.push_back(T);
            IdTriad++;
            return IdTriad;
        }

    varList.push_back(Var{ name });
    Triad T('V', new Var{ name }, new None{});
    triadList.push_back(T);
    IdTriad++;
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

    Triad T('=', new Link{ p1 }, new Link{ p2 });
    triadList.push_back(T);
    IdTriad++;
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
        long x2 = 0;
        GetSymbolSkipSpace();
        x2 = MethodT();
        Triad T('|', new Link{ x1 }, new Link{ x2 });
        triadList.push_back(T);
        IdTriad++;
        x1 = IdTriad;
    }
    return x1;
}

long Analyzer::MethodT()
{
    long x1 = MethodM();
    while (c == '&')
    {
        long x2 = 0;
        GetSymbolSkipSpace();
        x2 = MethodM();
        Triad T('&', new Link{ x1 }, new Link{ x2 });
        triadList.push_back(T);
        IdTriad++;
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
            Triad T('~', new Link{ x }, new None{});
            triadList.push_back(T);
            IdTriad++;
            x = IdTriad;
            if (c == ' ')
                GetSymbolSkipSpace();
        }
        else
            for (string temp_var = ""; ; GetSymbol())
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
    Triad T('C', new Constant{ x }, new None{});
    triadList.push_back(T);
    IdTriad++;
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




Triad::Triad(char operation, Base* leftOperand, Base* rightOperand) : operation(operation), leftOperand(leftOperand), rightOperand(rightOperand) {}

Var::Var(const string& src) : IdName(src) {}

Link::Link(const long linkTriad) : linkTriad(linkTriad) {}

Constant::Constant(const bool value) : value(value) {}

Base::~Base() {}

string Var::GetOperand()
{
    return this->IdName;
}

string Link::GetOperand()
{
    return "^" + to_string(this->linkTriad);
}

string Constant::GetOperand()
{
    return to_string(this->value);
}

string None::GetOperand()
{
    return "@";
}


void Triad::OutTriad()
{
    outfile1 << operation << '(' << leftOperand->GetOperand() << ", " << rightOperand->GetOperand() << ')' << endl;
}