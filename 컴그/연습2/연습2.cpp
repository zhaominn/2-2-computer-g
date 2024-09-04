#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <windows.h>

using namespace std;

vector<std::string> lines;

void FileToArray() {
    string filename;

    cout << "���� �̸��� �Է��ϼ���(.txt����!!): ";
    cin >> filename;
    ifstream file(filename);

    while (!file.is_open()) {
        cerr << "������ �� �� �����ϴ�." << endl;
        cout << "���� �̸��� �Է��ϼ���(.txt����!!): ";
        cin >> filename;
        file.open(filename);
    }

    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }

    file.close();

    for (size_t i = 0; i < lines.size(); ++i) {
        cout << lines[i] << endl;
    }
}

string reverseWordsInString(const std::string& input) {
    istringstream stream(input);
    string word;
    vector<string> words;

    while (stream >> word) {
        reverse(word.begin(), word.end());
        words.push_back(word);
    }

    ostringstream result;
    for (size_t i = 0; i < words.size(); ++i) {
        if (i > 0) result << ' ';
        result << words[i];
    }

    return result.str();
}

bool compareByWordCountAscending(const std::string& a, const std::string& b) {
    istringstream streamA(a), streamB(b);
    int wordCountA = std::distance(std::istream_iterator<std::string>(streamA), std::istream_iterator<std::string>());
    int wordCountB = std::distance(std::istream_iterator<std::string>(streamB), std::istream_iterator<std::string>());
    return wordCountA < wordCountB;
}

bool compareByWordCountDescending(const std::string& a, const std::string& b) {
    istringstream streamA(a), streamB(b);
    int wordCountA = std::distance(std::istream_iterator<std::string>(streamA), std::istream_iterator<std::string>());
    int wordCountB = std::distance(std::istream_iterator<std::string>(streamB), std::istream_iterator<std::string>());
    return wordCountA > wordCountB;
}

void printWithHighlight(const string& line, const string& wordToHighlight, int& totalCount) {
    const WORD highlightColor = FOREGROUND_RED | FOREGROUND_INTENSITY;  // ���̶���Ʈ ����
    const WORD defaultColor = 7;  // �⺻ ����

    // �ҹ��ڷ� ��ȯ
    string lowerLine = line;
    string lowerWordToHighlight = wordToHighlight;
    transform(lowerLine.begin(), lowerLine.end(), lowerLine.begin(), ::tolower);
    transform(lowerWordToHighlight.begin(), lowerWordToHighlight.end(), lowerWordToHighlight.begin(), ::tolower);

    size_t startPos = 0;
    size_t pos;
    int count = 0;

    while ((pos = lowerLine.find(lowerWordToHighlight, startPos)) != string::npos) {
        // ���̶���Ʈ ������ ���
        if (startPos < pos) {
            cout << line.substr(startPos, pos - startPos);
        }

        // ���̶���Ʈ
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), highlightColor);
        cout << line.substr(pos, wordToHighlight.length());
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), defaultColor);

        startPos = pos + wordToHighlight.length();
        count++;
    }

    // ������ ���ڿ� ���
    if (startPos < line.length()) {
        cout << line.substr(startPos);
    }

    cout << endl;
    totalCount += count;
}

int main() {
    FileToArray();
    int replaceCnt = 0;
    int cnt;
    int r = 0;
    string wordToHighlight;
    bool highlightFlag = false;
    bool reverseFlag = false;
    bool insertAtFlag = false;
    bool reverseWordsFlag = false;
    bool replaceFlag = false;

    while (1) {
        char cmd;
        cout << "\n����� �Է��ϼ��� :";
        cin >> cmd;
        cin.ignore();

        switch (cmd) {
        case 'c':
            if (!highlightFlag) {
                cnt = 0;
                cout << "�빮�ڷ� �����ϴ� �ܾ ã���ϴ�." << endl;
                for (size_t i = 0; i < lines.size(); ++i) {
                    if (lines[i][0] >= 'A' && lines[i][0] <= 'Z') {
                        WORD textAttribute = FOREGROUND_RED | FOREGROUND_INTENSITY;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textAttribute);
                        cout << lines[i] << endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                        ++cnt;
                    }
                    else {
                        cout << lines[i] << endl;
                    }
                }
                cout << cnt << " lines\n";
                highlightFlag = true;
            }
            else {
                for (size_t i = 0; i < lines.size(); ++i) {
                    cout << lines[i] << endl;
                }
                highlightFlag = false;
            }
            break;
        case 'd':
            if (!reverseFlag) {
                for (size_t i = 0; i < lines.size(); ++i) {
                    string reversedLine = lines[i];
                    reverse(reversedLine.begin(), reversedLine.end());
                    cout << reversedLine << std::endl;
                }
                reverseFlag = true;
            }
            else {
                for (size_t i = 0; i < lines.size(); ++i) {
                    cout << lines[i] << std::endl;
                }
                reverseFlag = false;
            }
            break;
        case 'e':
            if (!insertAtFlag) {
                for (size_t i = 0; i < lines.size(); ++i) {
                    const std::string& line = lines[i];
                    for (size_t j = 0; j < line.length(); ++j) {
                        if (j % 5 == 3 || j % 5 == 4) {
                            cout << '@';
                        }
                        else {
                            cout << line[j];
                        }
                    }
                    cout << std::endl;
                }
                insertAtFlag = true;
            }
            else {
                for (size_t i = 0; i < lines.size(); ++i) {
                    cout << lines[i] << std::endl;
                }
                insertAtFlag = false;
            }
            break;
        case 'f':
            if (!reverseWordsFlag) {
                for (size_t i = 0; i < lines.size(); ++i) {
                    lines[i] = reverseWordsInString(lines[i]);
                    cout << lines[i] << std::endl;
                }
                reverseWordsFlag = true;
            }
            else {
                for (size_t i = 0; i < lines.size(); ++i) {
                    cout << lines[i] << std::endl;
                }
                reverseWordsFlag = false;
            }
            break;
        case 'g':
            if (!replaceFlag) {
                char a, b;
                cout << "� ���ĺ��� � ���ĺ����� �ٲ��� ������:\n";
                cin >> a >> b;
                cin.ignore(); 
                for (size_t i = 0; i < lines.size(); ++i) {
                    const std::string& line = lines[i];
                    for (size_t j = 0; j < line.length(); ++j) {
                        if (line[j] == a)
                            cout << b;
                        else
                            cout << line[j];
                    }
                    cout << std::endl;
                }
                replaceFlag = true;
            }
            else {
                for (size_t i = 0; i < lines.size(); ++i) {
                    cout << lines[i] << std::endl;
                }
                replaceFlag = false;
            }
            break;
        case 'h':
            for (size_t i = 0; i < lines.size(); ++i) {
                cnt = 1; 
                const std::string& line = lines[i];
                for (size_t j = 0; j < line.length(); ++j) {
                    if (line[j] == ' ') {
                        ++cnt;
                    }
                }
                cout << line << "  " << cnt << " words\n";
            }
            break;
        case 'r':
            if (r % 3 == 0) {
                vector<string> sortedLines = lines;
                sort(sortedLines.begin(), sortedLines.end(), compareByWordCountAscending);
                for (const auto& line : sortedLines) {
                    cout << line << endl;
                }
            }
            else if (r % 3 == 1) {
                vector<string> sortedLines = lines;
                sort(sortedLines.begin(), sortedLines.end(), compareByWordCountDescending);
                for (const auto& line : sortedLines) {
                    cout << line << endl;
                }
            }
            else {
                for (size_t i = 0; i < lines.size(); ++i)
                    cout << lines[i] << std::endl;
            }
            r++;
            break;
        case 's':
        {
            string wordToHighlight;
            cout << "���̶���Ʈ�� �ܾ �Է��ϼ���: ";
            cin.ignore(); 
            getline(cin, wordToHighlight);

            int totalCount = 0;
            for (const auto& line : lines) {
                printWithHighlight(line, wordToHighlight, totalCount);
            }
            cout << "�ܾ� \"" << wordToHighlight << "\"�� �� ���� Ƚ��: " << totalCount << endl;
            break;
        }
        break;
        case 'q':
            cout << "���α׷��� �����մϴ�." << endl;
            return 0;
        }
    }
}
