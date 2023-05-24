#include <iostream>
#include <vector>
#include <string>
#include <fstream>

constexpr short int  MATRIX_SIZE = 15;
constexpr short int  RULES_SIZE = 12;

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
    UNKOWN_SYMBOL       //Неизвестный символ
};

/*
  Исходная грамматика
      S → (I,E)
      E → E "|" T | T
      T → T & M | M
      M → ~M | (E) | I | C
      I → AI | A
      C → D
      A → [a-z]
      D → true | false

  Приводим грамматику к виду слабого предшествования:

      S'→ ꞱLꞱ
      L → LS | S
      S → (I,E)
      E → E "|" T | T
      T → T & M | M
      M → ~M | (E) | I | C
*/


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

    Triad(char operation, Base* leftOperand, Base* rightOperand);
    void OutTriad();
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
    case UNKOWN_SYMBOL:
        std::cout << "Error: Unkown symbol " << param << std::endl;
        break;   
    }
    infile.close();
    outfile.close();
    exit(2);
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
    outfile << operation << '(' << leftOperand->GetOperand() << ", " << rightOperand->GetOperand() << ')' << endl;
} 


char Matrix[MATRIX_SIZE][MATRIX_SIZE] =
{
    {' ', 'L', 'S', 'E', 'T', 'M', ',', '|', '&', '~', '(', ')', 'I', 'C', '#'},
    {'L', ' ', '=', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ', ' ', ' ', '='},
    {'S', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', ' ', ' ', ' ', '>'},
    {'E', ' ', ' ', ' ', ' ', ' ', ' ', '=', ' ', ' ', ' ', '=', ' ', ' ', ' '},
    {'T', ' ', ' ', ' ', ' ', ' ', ' ', '>', '=', ' ', ' ', '>', ' ', ' ', ' '},
    {'M', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', ' ', ' ', '>', ' ', ' ', ' '},
    {',', ' ', ' ', '$', '<', '<', ' ', ' ', ' ', '<', '<', ' ', '<', '<', ' '},
    {'|', ' ', ' ', ' ', '$', '<', ' ', ' ', ' ', '<', '<', ' ', '<', '<', ' '},
    {'&', ' ', ' ', ' ', ' ', '=', ' ', ' ', ' ', '<', '<', ' ', '<', '<', ' '},
    {'~', ' ', ' ', ' ', ' ', '=', ' ', ' ', ' ', '<', '<', ' ', '<', '<', ' '},
    {'(', ' ', ' ', '$', '<', '<', ' ', ' ', ' ', '<', '<', ' ', '$', '<', ' '},
    {')', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', ' ', '>', '>', ' ', ' ', '>'},
    {'I', ' ', ' ', ' ', ' ', ' ', '=', '>', '>', ' ', ' ', '>', ' ', ' ', ' '},
    {'C', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', ' ', ' ', '>', ' ', ' ', ' '},
    {'#', '$', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ', ' ', ' ', ' '},
};


class Rule
{
public:
    char left;
    string right;
    Rule(char cLeft, string cRight);
};

Rule::Rule(char cLeft, string cRight) : left(cLeft), right(cRight) {}

Rule Rules[RULES_SIZE] =
{
    {'R',"#L#"},
    {'L',"LS"}, {'L',"S"},
    {'S',"(I,E)"},
    {'E',"E|T"}, {'E',"T"},
    {'T',"T&M"}, {'T',"M"},
    {'M',"~M"}, {'M',"(E)"}, {'M',"I"}, {'M',"C"},
};


class Relation
{
public:
    char symbol;
    char symbolRelation;
    int valueConstant;
    int idTriad;
    string varName;

    Relation(char symbolRelation, char symbol);
};

Relation::Relation(char symbolRelation, char symbol) : symbolRelation(symbolRelation), symbol(symbol) {}


class Analyzer
{
private:
    int token = EOF;
    int currentSymbol;
    string name;
    bool value;

    const string alp = "(,)|&~";

    vector<Var> varList;
    vector<Triad> triadList;
    vector<Relation> stack;
    
public:
    inline void GetSymbol();
    inline void GetSymbolSkipSpace();


    char FindRelationMatrix(char Y);
    char FindRules(string rightRule);
    char FindRelation(char symbol);


    void GetToken();
    void SemantixAnalyzer();
    char Convolution();
    void CreatTriads(string base, int indexLeft);


    bool GetVarValue(string& str);
    bool GetVarAdress(string name);


    void Run();
};


inline void Analyzer::GetSymbol()
{
    currentSymbol = infile.get();
}

inline void Analyzer::GetSymbolSkipSpace()
{
    while (isspace(currentSymbol)) GetSymbol();
}


char Analyzer::FindRelationMatrix(char Y)
{
    char X = stack.back().symbol;
    int i = 0, j = 0;
    while (Matrix[i][0] != X)
        i++;
    while (Matrix[0][j] != Y)
        j++;
    if (j > MATRIX_SIZE)
        PrintError(UNKOWN_SYMBOL, "No matrix element found");
    return Matrix[i][j];
}

char Analyzer::FindRules(string rightRule)
{
    for (int i = 0; i <= RULES_SIZE; i++)
        if (Rules[i].right == rightRule)
            return Rules[i].left;
    return NULL;
}

char Analyzer::FindRelation(char symbol)
{
    for (int i = stack.size() - 1; i > 1; i--)
        if (stack[i].symbolRelation == symbol)
            return i;
}


void Analyzer::GetToken()
{
    GetSymbolSkipSpace();

    if (isalpha(currentSymbol)) {
        for (string temp_var = ""; ; GetSymbol())
        {
            if (currentSymbol >= 'a' && currentSymbol <= 'z') {
                temp_var += currentSymbol;
            }
            else if (temp_var == "true" || temp_var == "false") {
                value = (temp_var == "true");
                token = 'C';
                break;
            }
            else if (!temp_var.empty()) {
                name = temp_var;
                token = 'I';
                break;
            }
            else {
                PrintError(SYNTAX_ERROR);
            }
        }
    }
    else if (strchr(alp.c_str(), currentSymbol))
    {
        token = currentSymbol;
        GetSymbol();
    }
    else if (currentSymbol == EOF)
        token = '#';
    else
        PrintError(SYNTAX_ERROR, string((char)currentSymbol, 1));
}

void Analyzer::SemantixAnalyzer()
{
    char relation = FindRelationMatrix(token);
    if (relation == ' ')
        PrintError(UNKOWN_SYMBOL, "No relation for lexeme " + relation);
    else if (relation != '>')
    {
        Relation rel(relation, token);
        if (token == 'C')
        {
            rel.valueConstant = value;
            Triad triad('C', new Constant(value), new None);
            triadList.push_back(triad);
            IdTriad++;
            rel.idTriad = IdTriad;
        }
        else if (token == 'I')
        {
            rel.varName = name;
            Triad triad('V', new Var(name), new None);
            triadList.push_back(triad);
            IdTriad++;
            rel.idTriad = IdTriad;
        }
        stack.push_back(rel);
    }
    else
    {
        while (relation == '>')
        {
            char result = Convolution();
            Relation rel(FindRelationMatrix(result), result);
            rel.idTriad = IdTriad;
            stack.push_back(rel);
            relation = FindRelationMatrix(token);
            for (auto it = stack.begin(); it != stack.end(); ++it)
                cout << it->symbolRelation << it->symbol << " ";
            cout << relation << char(token) << endl;
        }
        Relation rel(relation, token);
        if (token == 'I')
        {
            rel.varName = name;
            Triad triad('V', new Var(name), new None());
            triadList.push_back(triad);
            IdTriad++;
            rel.idTriad = IdTriad;
        }
        for (auto it = stack.begin(); it != stack.end(); ++it)
            cout << it->symbolRelation << it->symbol << " ";
        cout << endl;
        stack.push_back(rel);
    }
}

char Analyzer::Convolution()
{
    string base;
    int indexLeft = 0, temp = 0;
    char leftRule;
    bool flag = false;

    indexLeft = FindRelation('<');
    if (indexLeft == 0)
        indexLeft = FindRelation('$');

    while (true)
    {
        for (temp = indexLeft; temp < stack.size(); temp++)
            base += stack[temp].symbol;

        CreatTriads(base, indexLeft);

        if (leftRule = FindRules(base))
        {
            for (; indexLeft < stack.size(); )
                stack.pop_back();
            return leftRule;
        }
        else
        {
            base.clear();
            for (; indexLeft < stack.size(); indexLeft++)
                if (stack[indexLeft].symbolRelation == '$' && !flag)
                {
                    flag = true;
                    break;
                }
                else
                    flag = false;

            if (indexLeft == stack.size())
                PrintError(SYNTAX_ERROR, "Couldn't find symbols with the relation < and <= .");
        }
    }
}

void Analyzer::CreatTriads(string base, int indexLeft)
{
    if (base == "(I,E)")
    {
        GetVarAdress(stack[indexLeft + 1].varName);
        Triad triad('=', new Link(stack[indexLeft + 1].idTriad), new Link(stack[indexLeft + 3].idTriad));
        triadList.push_back(triad);
        IdTriad++;
    }
    else if (base == "E|T")
    {
        Triad triad('|', new Link(stack[indexLeft].idTriad), new Link(stack[indexLeft + 2].idTriad));
        triadList.push_back(triad);
        IdTriad++;
    }
    else if (base == "T&M")
    {
        Triad triad('&', new Link(stack[indexLeft].idTriad), new Link(stack[indexLeft + 2].idTriad));
        triadList.push_back(triad);
        IdTriad++;
    }
    else if (base == "~M")
    {
        Triad triad('~', new Link(stack[indexLeft + 1].idTriad), new None());
        triadList.push_back(triad);
        IdTriad++;
    }
    else if (base == "(E)")
    {
        IdTriad = stack[indexLeft + 1].idTriad;
    }
    else if (base == "I")
    {
        GetVarValue(stack[indexLeft].varName);
    }
}


bool Analyzer::GetVarValue(string& str)
{
    if (str.empty() || str.size() == 0)
        PrintError(SYNTAX_ERROR);

    for (int i = 0; i < varList.size(); i++)
        if (varList[i].GetOperand() == str)
            return true;

    PrintError(UNDEF_ID, str);
}

bool Analyzer::GetVarAdress(string name)
{
    if (name.empty() || name.size() == 0)
        PrintError(SYNTAX_ERROR);

    for (int i = 0; i < varList.size(); i++)
        if (varList[i].GetOperand() == name)
            return true;

    varList.push_back(Var{ name });
    return true;
}


void Analyzer::Run()
{
    stack.push_back(Relation(' ', '#'));
    GetSymbol();
    do
    {
        GetToken();
        SemantixAnalyzer();

        
        for (auto it = stack.begin(); it != stack.end(); ++it)
            cout << it->symbolRelation << it->symbol << " ";
        cout << endl;
        

    } while (token != '#');

    for (int i = 0, j = 1; i < triadList.size(); i++)
    {
        if (j == 1)
            outfile << j++ << ")";

        outfile << "\t" << i + 1 << " : ";
        triadList[i].OutTriad();

        if (triadList[i].operation == '=' && i != triadList.size() - 1)
            outfile << j++ << ")";
    }

}

int main()
{
    setlocale(LC_ALL, "ru");

    infile.open("infile.txt");
    outfile.open("outfile.txt");
    if (!infile.is_open() || !outfile.is_open()) {
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

