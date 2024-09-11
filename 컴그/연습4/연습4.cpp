#include <iostream>
#include <string>
#include <random>

#include <sstream>
#include <vector>

#include <thread>
#include <chrono>

#include <fstream>
#include <algorithm>
#include <windows.h>


using namespace std;

struct Node {
	char location;
	string shape;
	char valid;
	Node() : location(), shape(), valid(-1) {}
};

void SettingBoard(Node board[5][5]) {
	// 랜덤 설정
	random_device rd;
	default_random_engine dre{ rd() };
	uniform_int_distribution <> uid(1, 12);
	uniform_int_distribution <> joker(1, 5);

	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			board[i][j].valid = -1;
			board[i][j].shape = 'A' + uid(dre);
		}
	}

	board[joker(dre)][joker(dre)].shape = '@';

}

void PrintBoard(Node board[5][5]) {
	const WORD highlightColor1 = FOREGROUND_RED | FOREGROUND_INTENSITY;  // 하이라이트 색상
	const WORD defaultColor = 7;  // 기본 색상


	cout << "   a  b  c  d  e\n";
	for (int i = 0; i < 5; ++i) {
		cout << i + 1;
		for (int j = 0; j < 5; ++j) {
			if (board[i][j].valid == -1) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), defaultColor);
				cout << "  *";
			}
			else {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), highlightColor1);
				cout << "  " << board[i][j].shape;
			}
		}
		cout << '\n';
	}
}

int main() {
	const WORD highlightColor1 = FOREGROUND_RED | FOREGROUND_INTENSITY;  // 하이라이트 색상
	const WORD highlightColor2 = FOREGROUND_BLUE | FOREGROUND_INTENSITY;  // 하이라이트 색상
	const WORD defaultColor = 7;  // 기본 색상


	Node board[5][5];

	SettingBoard(board);
	PrintBoard(board);

	string word1, word2;
	while (1) {
		cout << "\n좌표 입력 :";
		cin >> word1;

		if (word1 == "r") {
			cout << "\n\n보드를 다시 세팅합니다~\n\n";
			SettingBoard(board);
			PrintBoard(board);
		}
		else {
			cin >> word2;

			if ((board[word1[1] - '1'][word1[0] - 'a'].shape == board[word2[1] - '1'][word2[0] - 'a'].shape)
				|| board[word1[1] - '1'][word1[0] - 'a'].shape == "@" || board[word2[1] - '1'][word2[0] - 'a'].shape == "@") {

				board[word1[1] - '1'][word1[0] - 'a'].valid = 1;
				board[word2[1] - '1'][word2[0] - 'a'].valid = 1;
			}
			else {
				board[word1[1] - '1'][word1[0] - 'a'].valid = 1;
				board[word2[1] - '1'][word2[0] - 'a'].valid = 1;
				

				cout << "   a  b  c  d  e\n";
				for (int i = 0; i < 5; ++i) {
					cout << i + 1;
					for (int j = 0; j < 5; ++j) {
						if (board[i][j].valid == -1) {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), defaultColor);
							cout << "  *";
						}
						else if (i== word1[1] - '1'&&j== word1[0] - 'a') {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), highlightColor2);
							cout << "  " << board[i][j].shape;
						}
						else {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), highlightColor1);
							cout << "  " << board[i][j].shape;
						}
					}
					cout << '\n';
				}


				cout << "\n다른 값이네요~ 다시 해보세요~~~~\n\n";
				this_thread::sleep_for(3s);
				board[word1[1] - '1'][word1[0] - 'a'].valid = -1;
				board[word2[1] - '1'][word2[0] - 'a'].valid = -1;
			}

			PrintBoard(board);
		}
	}

}