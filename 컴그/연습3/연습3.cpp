#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

struct Node
{
	int valid;
	int x;
	int y;
	int z;
	float distance;
	Node() : valid(-1), x(0), y(0), z(0) {}
	Node(int i, int x, int y, int z)
		:valid(i), x(x), y(y), z(z) {}
};

bool compareNodes(const Node& a, const Node& b) {
	return a.distance < b.distance;
}

bool compareNodes1(const Node& a, const Node& b) {
	return a.distance > b.distance;
}

int main() {
	Node Nodes[20];
	Node temp[20];
	string cmd;
	int x, y, z;
	int Index = 0;
	int a = 0;
	int s = 0;

	while (1) {
		cout << "명령을 입력하세요.\n";
		cin >> cmd;

		if (cmd == "+") {
			cin >> x >> y >> z;
			if (Index <= 19) {
				if (Nodes[Index].valid == -1) {
					Nodes[Index] = Node(1, x, y, z);
					Nodes[Index].distance = sqrt(Nodes[Index].x * Nodes[Index].x + Nodes[Index].y * Nodes[Index].y + Nodes[Index].z * Nodes[Index].z);
					++Index;
				}
			}
			else if (Nodes[0].valid == -1) {
				Index = 0;
				Nodes[Index] = Node(1, x, y, z);
				++Index;
			}
			else {
				cout << "\n자리 부족!\n";
			}
		}
		else if (cmd == "-") {
			for (int i = 19; i >= 0; --i) {
				if (Nodes[i].valid == 1) {
					Nodes[i] = Node(-1, 0, 0, 0);
					--Index;
					break;
				}
			}
		}
		else if (cmd == "e") {
			cin >> x >> y >> z;
			for (int i = 18; i >= 0; --i) {
				Nodes[i+1] = Nodes[i];
			}
			Nodes[0] = Node(1, x, y, z);
			Nodes[Index].distance = sqrt(Nodes[Index].x * Nodes[Index].x + Nodes[Index].y * Nodes[Index].y + Nodes[Index].z * Nodes[Index].z);
			++Index;
		}
		else if (cmd == "d") {
			for (int i = 0; i < 20; ++i) {
				if (Nodes[i].valid == 1) {
					Nodes[i] = Node(-1, 0, 0, 0);
					break;
				}
			}
		}
		else if (cmd == "l") {
			int cnt = 0;
			for (int i = 0; i < 20; ++i) {
				if (Nodes[i].valid == 1)
					++cnt;
			}
			cout << "\n점 " << cnt << "개 저장 중\n";
		}
		else if (cmd == "c") {
			for (int i = 0; i < 20; ++i) {
				Nodes[i] = Node(-1, 0, 0, 0);
			}
			Index = 0;
		}
		else if (cmd == "m") {
			float max = 0;
			int maxindex = 0;
			for (int i = 0; i < 20; ++i) {
				if (Nodes[i].valid = 1) {
					if (Nodes[i].distance > max) {
						maxindex = i;
						max = Nodes[i].distance;
					}
				}
			}
			cout << "( "<< Nodes[maxindex].x << ", " << Nodes[maxindex].y << ", " << Nodes[maxindex].z << " )\n";
		}
		else if (cmd == "n") {
			float min = 100;
			int minindex = 0;
			for (int i = 0; i < 20; ++i) {
				if (Nodes[i].valid = 1) {
					if (Nodes[i].distance > min) {
						minindex = i;
						min = Nodes[i].distance;
					}
				}
			}
			cout << "( " << Nodes[minindex].x << ", " << Nodes[minindex].y << ", " << Nodes[minindex].z << " )\n";

		}
		else if (cmd == "a") {
			if (a % 2 == 0) {
				int num = 0;
				for (int i = 0; i < 20; ++i) {
					temp[i] = Nodes[i];
				}
				vector<Node> validNodes;

				for (int i = 0; i < 20; ++i) {
					if (Nodes[i].valid == 1) {
						validNodes.push_back(Nodes[i]);
					}
				}

				sort(validNodes.begin(), validNodes.end(), compareNodes);

				int idx = 0;
				for (int i = 0; i < validNodes.size(); ++i) {
					Nodes[idx++] = validNodes[i];
				}

				for (int i = idx; i < 20; ++i) {
					Nodes[i] = Node(-1, 0, 0, 0);
				}
				++a;
			}
			else {
				for (int i = 0; i < 20; ++i) {
					Nodes[i] = temp[i];
				}
				++a;
			}
		}
		else if (cmd == "s") {
			if (s % 2 == 0) {
				for (int i = 0; i < 20; ++i) {
					temp[i] = Nodes[i];
				}
				sort(Nodes, Nodes + 20, compareNodes1);
				++s;
			}
			else {
				for (int i = 0; i < 20; ++i) {
					Nodes[i] = temp[i];
				}
				++s;
			}
		}
		else if (cmd == "q") {
			return 0;
		}

		for (int i = 19; i >= 0; --i) {
			cout << i + 1 << " - ";
			if (Nodes[i].valid == 1)
				cout << Nodes[i].x << Nodes[i].y << Nodes[i].z;
			cout << '\n';
		}
	}

}