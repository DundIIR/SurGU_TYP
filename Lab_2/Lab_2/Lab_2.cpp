#include <iostream>
#include "fstream"

using namespace std;

typedef enum States {
	Null, Normal, Slash, BigComment, Asterisk, SmallComment, Symbol, String, ConstSymbol, ConstString,
	Number, Exhibitor, StartPoint, PlusMinus, NumberExhibitor, NumberPoint, Long, Float, Error
} States;


bool comment(char c, States* State) {
	if (c == '/') {
		*State = Slash;
		return 1;
	}
	else if (c == '\"') {
		*State = String;
		return 1;
	}
	else if (c == '\'') {
		*State = Symbol;
		return 1;
	}
	else {
		return 0;
	}

}

int main(int argc, char** argv)
{
	system("chcp 1251 > 0");

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
				State = String;
			}
			else if (c == '\'') {
				State = Symbol;
			}
			else if (c == '.') {
				outfile << c;
				State = StartPoint;
			}
			else if (isdigit(c)) {
				outfile << c;
				State = Number;
			}
			else if (isalpha(c) || c == '_') {
				State = Error;
			}
			break;

		case Number:
			if (isdigit(c)) {
				outfile << c;
			}
			else if (c == '.') {
				outfile << c;
				State = NumberPoint;
			}
			else if (tolower(c) == 'e') {
				outfile << c;
				State = Exhibitor;
			}
			else {
				outfile << c << " error\n";
				if (comment(c, &State)) {
					break;
				}
				State = Error;
			}
			break;

		case StartPoint:
			if (comment(c, &State)) {
				break;
			}
			if (isdigit(c)) {
				outfile << c;
				State = NumberPoint;
			}
			else {
				State = Error;
			}
			break;

		case Exhibitor:
			if (isdigit(c)) {
				outfile << c;
				State = NumberExhibitor;
			}
			else if (c == '+' || c == '-') {
				outfile << c;
				State = PlusMinus;
			}
			else if (isalpha(c) || c == '.' || c == '_') {
				outfile << c << " error\n";
				State = Error;
			}
			else {
				outfile << c << " error\n";
				if (comment(c, &State)) {
					break;
				}
				State = Normal;
			}
			break;
		case NumberPoint:
			if (isdigit(c)) {
				outfile << c;
			}
			else if (tolower(c) == 'e') {
				outfile << c;
				State = Exhibitor;
			}
			else if (tolower(c) == 'f') {
				outfile << c;
				State = Float;
			}
			else if (tolower(c) == 'l') {
				outfile << c;
				State = Long;
			}
			else if (isalpha(c) || c == '.' || c == '_') {
				outfile << c << " error\n";
				State = Error;
			}
			else {
				outfile << " double\n";
				if (comment(c, &State)) {
					break;
				}
				State = Normal;
			}
			break;

		case PlusMinus:

			if (isdigit(c)) {
				outfile << c;
				State = NumberExhibitor;
			}
			else if (isalpha(c) || c == '.' || c == '_') {
				outfile << c << " error\n";
				State = Error;
			}
			else {
				outfile << c << " error\n";
				if (comment(c, &State)) {
					break;
				}
				State = Normal;
			}
			break;

		case NumberExhibitor:
			if (isdigit(c)) {
				outfile << c;
			}
			else if (tolower(c) == 'f') {
				outfile << c;
				State = Float;
			}
			else if (tolower(c) == 'l') {
				outfile << c;
				State = Long;
			}
			else if (isalpha(c) || c == '.' || c == '_') {
				outfile << c << " error\n";
				State = Error;
			}
			else {
				outfile << " double\n";
				if (comment(c, &State)) {
					break;
				}
				State = Normal;
			}
			break;

		case Long:
			if (isalpha(c) || c == '.' || c == '_' || isdigit(c)) {
				outfile << c << " error\n";
				State = Error;
			}
			else {
				outfile << " long\n";
				if (comment(c, &State)) {
					break;
				}
				State = Normal;
			}
			break;

		case Float:
			if (isalpha(c) || c == '.' || c == '_' || isdigit(c)) {
				outfile << c << " error\n";
				State = Error;
			}
			else {
				outfile << " float\n";
				if (comment(c, &State)) {
					break;
				}
				State = Normal;
			}
			break;

		case Slash:
			if (c == '*')
				State = BigComment;
			else if (c == '/')
				State = SmallComment;
			else if (isdigit(c)) {
				outfile << c;
				State = Number;
			}
			else if (isalpha(c) || c == '.' || c == '_') {
				outfile << c << " error\n";
				State = Error;
			}
			else {
				State = Normal;
			}
			break;

		case BigComment:
			if (c == '*')
				State = Asterisk;
			break;

		case Asterisk:
			if (c == '*')
				break;
			else if (c == '/') {
				State = Normal;
			}
			else
				State = BigComment;
			break;

		case SmallComment:
			if (c == '\n' || c == '\r') {
				State = Normal;
			}
			break;

		case String:
			if (c == '\"') {
				State = Normal;
			}
			else if (c == '\\') {
				State = ConstString;
			}
			break;

		case ConstString:
			State = String;
			break;

		case Symbol:
			if (c == '\'') {
				State = Normal;
			}
			else if (c == '\\') {
				State = ConstSymbol;
			}
			break;

		case ConstSymbol:
			State = Symbol;
			break;

		case Error:
			if (isalpha(c) || c == '.' || c == '_' || isdigit(c)) {}
			else {
				if (comment(c, &State)) {
					break;
				}
				State = Normal;
			}
			break;
		}
	}

	infile.close();
	outfile.close();
	std::remove("text1.txt");
	std::rename("text2.txt", "text1.txt");
	return 0;
}