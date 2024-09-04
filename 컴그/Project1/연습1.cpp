#include <iostream>
#include <time.h>
#include <string.h>
#include <string>

using namespace std;

int num1[4][4];
int num2[4][4];
int e = 0;

void MakeNewMatrix() {
	srand((unsigned int)time(NULL));
	int two1 = 0;
	int two2 = 0;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			num1[i][j] = rand() % 3;
			if (num1[i][j] == 2) {
				if (two1 == 2) {
					num1[i][j] = rand() % 2;
				}
				else ++two1;
			}
			cout << num1[i][j] << " ";
		}

		cout << "       ";
		for (int j = 0; j < 4; ++j) {
			num2[i][j] = rand() % 3;
			if (num2[i][j] == 2) {
				if (two2 == 2) {
					num2[i][j] = rand() % 2;
				}
				else ++two2;
			}
			cout << num2[i][j] << " ";
		}
		cout << "\n";
	}
}

void determinant(int num[4][4]) {
	int m11, m12, m13, m14;

	m11 = ((num[2][2] * num[3][3] * num[4][4]) + (num[3][2] * num[4][3] * num[2][4]) + (num[4][2] * num[2][3] * num[3][4]))
		- ((num[2][4] * num[3][3] * num[4][2]) + (num[2][3] * num[3][2] * num[4][4]) + (num[2][2] * num[4][3] * num[3][4]));
	m12 = ((num[2][1] * num[3][3] * num[4][4]) + (num[3][1] * num[4][3] * num[2][4]) + (num[4][1] * num[2][3] * num[3][4]))
		- ((num[2][4] * num[3][3] * num[4][1]) + (num[2][3] * num[3][1] * num[4][4]) + (num[2][1] * num[4][3] * num[3][4]));
	m13 = ((num[2][1] * num[3][2] * num[4][4]) + (num[3][1] * num[4][2] * num[2][4]) + (num[4][1] * num[2][2] * num[3][4]))
		- ((num[2][4] * num[3][2] * num[4][1]) + (num[2][2] * num[3][1] * num[4][4]) + (num[2][1] * num[4][2] * num[3][4]));
	m14 = ((num[2][1] * num[3][2] * num[4][3]) + (num[3][1] * num[4][2] * num[2][3]) + (num[4][1] * num[2][2] * num[3][3]))
		- ((num[2][3] * num[3][2] * num[4][1]) + (num[2][2] * num[3][1] * num[4][3]) + (num[2][1] * num[4][2] * num[3][3]));

	cout << "행렬식의 값: " << (num[1][1] * m11) - (num[1][2] * m12) + (num[1][3] * m13) - (num[1][4] * m14) << "\n";
}

void E() {
	++e;
	if (e % 3 == 1) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (num1[i][j] % 2 == 0)
					cout << num1[i][j] << " ";
				else cout << "  ";
			}
			cout << "       ";
			for (int j = 0; j < 4; ++j) {
				if (num2[i][j] % 2 == 0)
					cout << num2[i][j] << " ";
				else cout << "  ";
			}
			cout << "\n";
		}
	}
	else if (e % 3 == 2) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (num1[i][j] % 2 == 1)
					cout << num1[i][j] << " ";
				else cout << "  ";
			}
			cout << "       ";
			for (int j = 0; j < 4; ++j) {
				if (num2[i][j] % 2 == 1)
					cout << num2[i][j] << " ";
				else cout << "  ";
			}
			cout << "\n";
		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
					cout << num1[i][j] << " ";
			}
			cout << "       ";
			for (int j = 0; j < 4; ++j) {
					cout << num2[i][j] << " ";
			}
			cout << "\n";
		}
	}
}

int main() {
	MakeNewMatrix();
	while (1) {
		string cmd;
		cout << "m :곱셈, a :덧셈, d :뺄셈, r :행렬식의 값\n t :전치행렬과 그 행렬식의 값, e :짝수만,홀수만\ns :값 새로 설정, 1~9 :숫자를 곱함, q : 종료\n";
		cin >> cmd;
		if (cmd == "m") {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					cout << (num1[i][1] * num2[1][j]) + (num1[i][2] * num2[2][j])+ (num1[i][3] * num2[3][j])+ (num1[i][4] * num2[4][j]) << " ";
				}
				cout << "\n";
			}
		}
		else if (cmd == "a") {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					cout << num1[i][j] + num2[i][j] << " ";
				}
				cout << "\n";
			}
		}
		else if (cmd == "d") {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					cout << num1[i][j] - num2[i][j] << " ";
				}
				cout << "\n";
			}
		}
		else if (cmd == "r") {
			determinant(num1);
			determinant(num2);
		}
		else if (cmd == "t") {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					cout << num1[j][i] << " ";
				}

				cout << "       ";
				for (int j = 0; j < 4; ++j) {
					cout << num2[j][i] << " ";
				}
				cout << "\n";
			}

			determinant(num1);
			determinant(num2);
		}
		else if (cmd == "e") {
			E();
		}
		else if (cmd == "s") {
			MakeNewMatrix();
		}
		else if (cmd == "q") {
			printf("프로그램을 종료합니다.");
			return 0;
		}
		else {
			if (cmd >= "1" && cmd <= "9") {
				int num = stoi(cmd);
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						cout << num * num1[i][j] << " ";
					}
					cout << "       ";
					for (int j = 0; j < 4; ++j) {
						cout << num * num2[i][j] << " ";
					}
					cout << "\n";
				}
			}
			else
				cout << "잘못된 명령입니다. 다시 입력하세요.\n";
		}
	}
}