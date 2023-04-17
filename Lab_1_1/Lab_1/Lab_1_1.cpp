#include <iostream>
#include "fstream"

using namespace std;

typedef enum States { Normal, Slash, Comment, Asterisk } States;

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
		cout << "Ошибка в создании временного файла";
		return 2;
	}
	
	while (infile.get(c) && c != EOF)
	{
		switch (State)
		{
		case Normal:
			if (c == '/')
				State = Slash;
			else
				outfile << c;
			break;
		case Slash:
			if (c == '*')
				State = Comment;
			else if (c == '/')
				outfile << '/';
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
			else if (c == '/')
				State = Normal;
			else
				State = Comment;
			break;
		}
	}

	infile.close();
	outfile.close();
	std::remove("text1.txt");
	std::rename("text2.txt", "text1.txt");
	return 0;
}