#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

struct Rectangle1 {
	int startX;
	int startY;
	int endX;
	int endY;
};

void SettingBoard(vector<Rectangle1>& rectangles, vector<Rectangle1>& rectangless) {
	if (rectangles.size() > 0) {
		rectangles.clear();
	}
	if (rectangless.size() > 0) {
		rectangless.clear();
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
	struct Rectangle1 temp = { x1,y1,x2,y2 };
	rectangles.push_back(temp);
	temp = { x1 + 30,y1,x2 + 30,y2 };
	rectangles.push_back(temp);
	temp = { x1 - 30,y1,x2 - 30,y2 };
	rectangles.push_back(temp);
	temp = { x1,y1 + 30,x2,y2 + 30 };
	rectangles.push_back(temp);
	temp = { x1,y1 - 30,x2,y2 - 30 };
	rectangles.push_back(temp);

	int x1s, y1s, x2s, y2s;
	cout << "\n좌표값 두 개를 또 입력하세요";
	cin >> x1s >> y1s >> x2s >> y2s;
	temp = { x1s,y1s,x2s,y2s };
	rectangless.push_back(temp);
	temp = { x1s + 30,y1s,x2s + 30,y2s };
	rectangless.push_back(temp);
	temp = { x1s - 30,y1s,x2s - 30,y2s };
	rectangless.push_back(temp);
	temp = { x1s,y1s + 30,x2s,y2s + 30 };
	rectangless.push_back(temp);
	temp = { x1s,y1s - 30,x2s,y2s - 30 };
	rectangless.push_back(temp);
}

void PrintBoard(const vector<Rectangle1>& rectangles, const vector<Rectangle1>& rectangless) {
	const WORD highlightColor1 = FOREGROUND_RED | FOREGROUND_INTENSITY;
	const WORD defaultColor = 7;

	system("cls");
	for (int i = 0; i < 30; ++i) {
		for (int j = 0; j < 30; ++j) {
			bool rectPrinted = false;
			bool rectanglessPrinted = false;
			bool overlapPrinted = false;

			for (const auto& rect : rectangles) {
				if (rect.startX <= j && rect.endX >= j && rect.startY <= i && rect.endY >= i) {
					rectPrinted = true;
					break;
				}
			}

			for (const auto& rectang : rectangless) {
				if (rectang.startX <= j && rectang.endX >= j && rectang.startY <= i && rectang.endY >= i) {
					rectanglessPrinted = true;
					if (rectPrinted) {
						overlapPrinted = true;
						break;
					}
				}
			}

			if (overlapPrinted) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), highlightColor1);
				cout << " #";
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), defaultColor);
			}
			else if (rectPrinted) {
				cout << " 0";
			}
			else if (rectanglessPrinted) {
				cout << " X";
			}
			else {
				cout << " .";
			}
		}
		cout << '\n';
	}
}

int main() {
	vector<Rectangle1> Rectangles;
	vector<Rectangle1> Rectangless;

	SettingBoard(Rectangles, Rectangless);
	PrintBoard(Rectangles, Rectangless);

	char cmd;
	int X = 0;
	int Y = 0;

	while (1) {
		cout << "명령을 입력하세요\n";
		cin >> cmd;

		switch (cmd) {
		case 'x':
			for (int i = 0; i < Rectangles.size(); ++i) {
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
		case 'w':
			for (int i = 0; i < Rectangless.size(); ++i) {
				--Rectangless[i].endX;
				--Rectangless[i].startX;
			}
			break;
		case 'W':
			for (int i = 0; i < Rectangless.size(); ++i) {
				++Rectangless[i].endX;
				++Rectangless[i].startX;
			}
			break;
		case 'a':
			for (int i = 0; i < Rectangless.size(); ++i) {
				--Rectangless[i].endY;
				--Rectangless[i].startY;
			}
			break;
		case 'A':
			for (int i = 0; i < Rectangless.size(); ++i) {
				++Rectangless[i].endY;
				++Rectangless[i].startY;
			}
			break;
		case 'd':
			if (Rectangless[0].endX + 1 != Rectangless[0].startX && Rectangless[0].endY + 1 != Rectangles[0].startY) {
				for (int i = 0; i < Rectangless.size(); ++i) {
					++Rectangless[i].endX;
					--Rectangless[i].startX;
					++Rectangless[i].endY;
					--Rectangless[i].startY;
				}
			}
			break;
		case 'D':
			if (Rectangless[0].endX != 29 && Rectangless[0].startX != 0 && Rectangless[0].endY != 29 && Rectangless[0].startY != 0) {
				for (int i = 0; i < Rectangless.size(); ++i) {
					--Rectangless[i].endX;
					++Rectangless[i].startX;
					--Rectangless[i].endY;
					++Rectangless[i].startY;
				}
			}
			break;
		case 'r':
			SettingBoard(Rectangles, Rectangless);
			break;
		case 'q':
			return 0;
		}

		if (cmd != 'm' && cmd != 'n')
			PrintBoard(Rectangles, Rectangless);

	}
}