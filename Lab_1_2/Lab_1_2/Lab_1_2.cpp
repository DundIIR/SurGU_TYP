#include <iostream>
#include "fstream"

using namespace std;

typedef enum States { Normal, Slash, Comment, Asterisk, SlashComment, Symbol, Str, ConstSymbol, ConstStr } States;

int main(int argc, char** argv)
{
	system("chcp 1251 > 0");
	/*Привет*/
	States State = Normal; 
	char c; 

	ifstream infile("text1.txt");
	if (!infile.is_open()) {
		cout << "Файл не удалось открыть!";
		return 1;
	}
	ofstream outfile("text2.txt");
	if (!outfile.is_open()) {
		infile.close();
		cout << "Ошибка в /*создании  временного файла";
		return 2;
	}
	
	while (infile.get(c) && !infile.eof())
	{
		switch (State)
		{
		case Normal:
			if (c == '/')
				State = Slash;
			else if (c == '\"') {
				outfile << '\"';
				State = Str;
			}
			else if (c == '\'') {
				outfile << '\'';
				State = Symbol;
			}
			else
				outfile << c;
			break;
		case Slash:
			if (c == '*')
				State = Comment;
			else if (c == '/')
				State = SlashComment;
			else {
				State = Normal;
				outfile << '/' << c;
			}
			break;
		case Comment:
			if (c == '*')
				State = Asterisk;
			break;
		case Asterisk:
			if (c == '*')
				break;
			else if (c == '/') {
				State = Normal;
				outfile << ' ';
			}
			else
				State = Comment;
			break;
		case SlashComment:
			if (c == '\n') {
				State = Normal;
				outfile << '\n';
			}
			else if (c == '\r') {
				State = Normal;
				outfile << '\r';
			}
			break;
		case Str:
			if (c == '\"') {
				outfile << '\"';
				State = Normal;
			}
			else if (c == '\\') {
				outfile << '\\';
				State = ConstStr;
			}
			else
				outfile << c;
			break;
		case ConstStr:
			outfile << c;
			State = Str;
			break;
		case Symbol:
			if (c == '\'') {
				outfile << '\'';
				State = Normal;
			}
			else if (c == '\\') {
				outfile << '\\';
				State = ConstSymbol;
			}
			else
				outfile << c;
			break;
		case ConstSymbol:
			outfile << c;
			State = Symbol;
			break;

		}
	}

	infile.close();
	outfile.close();
	std::remove("text1.txt");
	std::rename("text2.txt", "text1.txt");
	return 0;
}