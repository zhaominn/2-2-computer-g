#include <iostream>
#include <string>
#include <random>

using namespace std;

struct Node {
	string location;
	string shape;
	int valid;
	Node() : valid(-1) {}
};

int main() {
	Node board[5][5];

	// 랜덤 설정
	uniform_int_distribution <> W(1, 12);
	static mt19937 gen(time(NULL));

	// 보드 그리기
	cout << "   a  b  c  d  e\n";
	for (int i = 0; i < 5; ++i) {
		cout << i + 1;
		for (int j = 0; j < 5; ++j) {
			if (board[i][j].valid == -1)
				cout << "  *";
			else
				cout << " " << board[i][j].shape;

			board[i][j].location = { ("a" + i), j };
			board[i][j].shape = W(gen);

			cout<<board
		}
		cout << '\n';
	}


	string word1, word2;
	while (1) {
		cout << "좌표 입력 :";
		cin >> word1 >> word2;

		for (int i = 0; i < 5; ++i) {
			cout << i + 1;
			for (int j = 0; j < 5; ++j) {
				if (board[i][j].valid == -1)
					cout << "  *";
				else
					cout << " " << board[i][j].shape;

				board[i][j].shape = W(gen);
			}
			cout << '\n';
		}

	}


}