#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Rectangle {
	int startX;
	int startY;
	int endX;
	int endY;
};

void SettingBoard(vector<Rectangle>& rectangles) {
	if (rectangles.size() > 0) {
		rectangles.clear();
	}

	for (int i = 0; i < 30; ++i) {
		for (int j = 0; j < 30; ++j) {
			cout << " .";
		}
		cout << '\n';
	}


	int x1, y1, x2, y2;
	cout << "좌표값 두 개를 입력하세요";
	cin >> x1 >> y1 >> x2 >> y2;
	struct Rectangle temp = { x1,y1,x2,y2 };
	rectangles.push_back(temp);
	temp = { x1 + 30,y1,x2 + 30,y2 };
	rectangles.push_back(temp);
	temp = { x1 - 30,y1,x2 - 30,y2 };
	rectangles.push_back(temp);
	temp = { x1,y1 + 30,x2,y2 + 30 };
	rectangles.push_back(temp);
	temp = { x1,y1 - 30,x2,y2 - 30 };
	rectangles.push_back(temp);
}

void PrintBoard(vector<Rectangle>& rectangles) {
	system("cls");
	for (int i = 0; i < 30; ++i) {
		for (int j = 0; j < 30; ++j) {
			if (rectangles[0].startX <= j && rectangles[0].endX >= j && rectangles[0].startY <= i && rectangles[0].endY >= i) {
				cout << " 0";
			}
			else if (rectangles[1].startX <= j && rectangles[1].endX >= j && rectangles[1].startY <= i && rectangles[1].endY >= i) {
				cout << " 0";
			}
			else if (rectangles[2].startX <= j && rectangles[2].endX >= j && rectangles[2].startY <= i && rectangles[2].endY >= i) {
				cout << " 0";
			}
			else if (rectangles[3].startX <= j && rectangles[3].endX >= j && rectangles[3].startY <= i && rectangles[3].endY >= i) {
				cout << " 0";
			}
			else if (rectangles[4].startX <= j && rectangles[4].endX >= j && rectangles[4].startY <= i && rectangles[4].endY >= i) {
				cout << " 0";
			}
			else
				cout << " .";
		}
		cout << '\n';
	}
}

int main() {
	vector<Rectangle> Rectangles;

	SettingBoard(Rectangles);
	PrintBoard(Rectangles);

	char cmd;
	int X = 0;
	int Y = 0;

	while (1) {
		cout << "명령을 입력하세요\n";
		cin >> cmd;

		switch (cmd) {
		case 'x':
			for (int i = 0; i < Rectangles.size();++i) {
			--Rectangles[i].endX;
			--Rectangles[i].startX;
		}
			break;
		case 'X':
			for (int i = 0; i < Rectangles.size(); ++i) {
				++Rectangles[i].endX;
				++Rectangles[i].startX;
			}
			break;
		case 'y':
			for (int i = 0; i < Rectangles.size(); ++i) {
				--Rectangles[i].endY;
				--Rectangles[i].startY;
			}
			break;
		case 'Y':
			for (int i = 0; i < Rectangles.size(); ++i) {
				++Rectangles[i].endY;
				++Rectangles[i].startY;
			}
			break;
		case 's':
			if (Rectangles[0].endX + 1 != Rectangles[0].startX && Rectangles[0].endY + 1 != Rectangles[0].startY) {
				for (int i = 0; i < Rectangles.size(); ++i) {
					++Rectangles[i].endX;
					--Rectangles[i].startX;
					++Rectangles[i].endY;
					--Rectangles[i].startY;
				}
			}
			break;
		case 'S':
			if (Rectangles[0].endX != 29 && Rectangles[0].startX != 0 && Rectangles[0].endY != 29 && Rectangles[0].startY != 0) {
				for (int i = 0; i < Rectangles.size(); ++i) {
					--Rectangles[i].endX;
					++Rectangles[i].startX;
					--Rectangles[i].endY;
					++Rectangles[i].startY;
				}
				}
			break;
		case 'i':
			for (int i = 0; i < Rectangles.size(); ++i) {
				++Rectangles[i].endX;
				--Rectangles[i].startX;
			}
			break;
		case 'I':
			for (int i = 0; i < Rectangles.size(); ++i) {
				--Rectangles[i].endX;
				++Rectangles[i].startX;
			}
			break;
		case 'j':
			for (int i = 0; i < Rectangles.size(); ++i) {
				++Rectangles[i].endY;
				--Rectangles[i].startY;
			}
			break;
		case 'J':
			for (int i = 0; i < Rectangles.size(); ++i) {
				--Rectangles[i].endY;
				++Rectangles[i].startY;
			}
			break;
		case 'a':
			for (int i = 0; i < Rectangles.size(); ++i) {
				++Rectangles[i].endX;
				--Rectangles[i].startX;
				--Rectangles[i].endY;
				++Rectangles[i].startY;
			}
			break;
		case 'A':
			for (int i = 0; i < Rectangles.size(); ++i) {
				--Rectangles[i].endX;
				++Rectangles[i].startX;
				++Rectangles[i].endY;
				--Rectangles[i].startY;
			}
			break;
		case 'm':
			X = Rectangles[0].endX - Rectangles[0].startX + 1;
			Y = Rectangles[0].endY - Rectangles[0].startY + 1;
			cout << "사각형의 면적 :" << X << " x " << Y << " = " << X * Y << '\n';
			break;
		case 'n':
			X = Rectangles[0].endX - Rectangles[0].startX + 1;
			Y = Rectangles[0].endY - Rectangles[0].startY + 1;
			cout << "전체 면적에 대한 사각형 면적 비율: " << (X * Y * 100) / (30 * 30) << "% \n";
			break;
		case 'r':
			SettingBoard(Rectangles);
			break;
		case 'q':
			return 0;
		}

		if (cmd != 'm' && cmd != 'n')
			PrintBoard(Rectangles);

	}
}