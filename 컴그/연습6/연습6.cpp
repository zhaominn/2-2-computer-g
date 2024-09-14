#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <conio.h> // _getch()
#include <windows.h>

using namespace std;

struct board {
    string valid;  // "1"배경, "0"경로, "X"장애물
    int color;
};

struct point {
    int x, y;
};

void SetBoard(board(&boards)[30][30]) {
    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 30; ++j) {
            boards[i][j] = { ".", 0 };
        }
    }

    random_device rd;
    default_random_engine dre{ rd() };
    uniform_int_distribution<> uid(0, 29);
    int x, y;

    // 장애물
    for (int i = 0; i < 30; ++i) {
        do {
            x = uid(dre);
            y = uid(dre);
        } while (boards[x][y].valid == "X" || (x == 0 && y == 0) || (x == 29 && y == 29));
        boards[x][y].valid = "X";
    }
}

void PrintBoard(const board boards[30][30]) {
    system("cls");
    const WORD highlightColor1 = FOREGROUND_RED | FOREGROUND_INTENSITY;
    const WORD defaultColor = 7;

    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 30; ++j) {
            if (boards[i][j].valid == "*") {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), highlightColor1);
                cout << boards[i][j].valid << " ";
            }
            else {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), defaultColor);
                cout << boards[i][j].valid << " ";
            }
        }
        cout << '\n';
    }
}

// 유효한 이동인지 확인
bool IsValidMove(int x, int y, const board(&boards)[30][30], vector<vector<bool>>& visited) {
    return x >= 0 && x < 30 && y >= 0 && y < 30 && boards[x][y].valid != "X" && !visited[x][y];
}

bool MakeRoute(board(&boards)[30][30], int x, int y, vector<point>& path, vector<vector<bool>>& visited,
    bool& up, bool& right, bool& down, bool& left, int& lastDirection, int& directionCount) {

    // 목적지에 도착했는지 확인
    if (x == 29 && y == 29) {
        path.push_back({ x, y });
        boards[x][y].valid = "0";
        return up && right && down && left;
    }

    visited[x][y] = true;
    path.push_back({ x, y });
    boards[x][y].valid = "0";

    int directions[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
    bool directionFlags[4] = { false, false, false, false };

    for (int i = 0; i < 4; ++i) {
        int newX = x + directions[i][0];
        int newY = y + directions[i][1];

        // 방향 인덱스
        int dirIndex = i;

        // 이동이 유효한지 확인
        if (IsValidMove(newX, newY, boards, visited)) {

            // 같은 방향으로 이동 중인지 확인
            if (dirIndex == lastDirection) {
                directionCount++;
            }
            else {
                directionCount = 1;
                lastDirection = dirIndex;
            }

            // 연속된 셀의 개수가 5 미만인지 확인
            if (directionCount < 5) {
                if (i == 0) up = true;
                if (i == 1) right = true;
                if (i == 2) down = true;
                if (i == 3) left = true;

                if (MakeRoute(boards, newX, newY, path, visited, up, right, down, left, lastDirection, directionCount)) {
                    return true;
                }
            }
        }
    }

    visited[x][y] = false;
    path.pop_back();
    return false;
}

void ClearPath(board(&boards)[30][30], const vector<point>& path) {
    for (const auto& p : path) {
        boards[p.x][p.y].valid = ".";
    }
}

int main() {
    board boards[30][30];
    vector<point> path;
    vector<vector<bool>> visited(30, vector<bool>(30, false));
    point objectPosition = { 0, 0 };
    bool up = false, right = false, down = false, left = false;

    int lastDirection = -1; // 초기 방향
    int directionCount = 0;

    SetBoard(boards);
    PrintBoard(boards);

    while (true) {
        char command = _getch();

        switch (command) {
        case 13:  // Enter
            ClearPath(boards, path);
            path.clear();
            visited = vector<vector<bool>>(30, vector<bool>(30, false));
            SetBoard(boards);  // 보드와 장애물 초기화
            objectPosition = { 0, 0 }; // 객체를 (0, 0)으로 이동
            boards[objectPosition.x][objectPosition.y].valid = "*"; // 객체의 위치에 '*' 표시
            if (MakeRoute(boards, 0, 0, path, visited, up, right, down, left, lastDirection, directionCount)) {
                // cout << "경로가 성공적으로 생성되었습니다!\n";
            }
            else {
                cout << "경로 생성에 실패했습니다.\n";
            }
            PrintBoard(boards);
            break;

        case 'r':
            if (boards[objectPosition.x][objectPosition.y].valid == "0") {
                boards[objectPosition.x][objectPosition.y].valid = "."; // 기존 경로를 지우고
            }
            objectPosition = { 0, 0 }; // 객체를 (0, 0)으로 이동
            boards[objectPosition.x][objectPosition.y].valid = "*"; // 객체의 위치에 '*' 표시
            break;

        case 'w':
            if (objectPosition.x > 0 && boards[objectPosition.x - 1][objectPosition.y].valid == "0") {
                boards[objectPosition.x][objectPosition.y].valid = "0";
                objectPosition.x--;
                boards[objectPosition.x][objectPosition.y].valid = "*";
            }
            break;

        case 's':
            if (objectPosition.x < 29 && boards[objectPosition.x + 1][objectPosition.y].valid == "0") {
                boards[objectPosition.x][objectPosition.y].valid = "0";
                objectPosition.x++;
                boards[objectPosition.x][objectPosition.y].valid = "*";
            }
            break;

        case 'a':
            if (objectPosition.y > 0 && boards[objectPosition.x][objectPosition.y - 1].valid == "0") {
                boards[objectPosition.x][objectPosition.y].valid = "0";
                objectPosition.y--;
                boards[objectPosition.x][objectPosition.y].valid = "*";
            }
            break;

        case 'd':
            if (objectPosition.y < 29 && boards[objectPosition.x][objectPosition.y + 1].valid == "0") {
                boards[objectPosition.x][objectPosition.y].valid = "0";
                objectPosition.y++;
                boards[objectPosition.x][objectPosition.y].valid = "*";
            }
            break;

        case 'Q':
            return 0;

        default:
            cout << "잘못된 명령입니다.\n";
            break;
        }

        PrintBoard(boards);
    }

    return 0;
}
