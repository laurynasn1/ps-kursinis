#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_set>
#include <cstring>
#define x first
#define y second

#define ID(x, y) ((x) * N + (y))

using namespace std;

const int MAXN = 8;
int N;
unordered_set<string> closedList;
pair<int, int> d[] = { make_pair(-1, 0), make_pair(0, 1), make_pair(1, 0), make_pair(0, -1) };
char dir[] = {               '^'       ,       '>'      ,       'v'      ,        '<'       };
bool visited[MAXN][MAXN];
int set1[] = { 1, 18, 20, 15, 16, 11, 12, 5 };
int set2[] = { 2, 18, 20, 13, 14, 9, 10, 6 };
int set3[] = { 3, 9, 13, 11, 15, 17, 19, 7 };
int set4[] = { 4, 10, 14, 12, 16, 17, 19, 8 };

bool connectedUp[22][22];
bool connectedDown[22][22];
bool connectedLeft[22][22];
bool connectedRight[22][22];

void calculateConnectedMatrix()
{
    for (int i : set1)
        for (int j : set2)
            connectedUp[i][j] = connectedDown[j][i] = true;
    for (int i : set3)
        for (int j : set4)
            connectedRight[i][j] = connectedLeft[j][i] = true;
}

struct STATE
{
    string BOARD;
    vector<pair<pair<int, int>, char> > moves;
    int hValue;
};

bool boundaries (int x, int y)
{
    return (x >= 0 && x < N && y >= 0 && y < N);
}

bool valid (int x, int y, string &s)
{
    return (boundaries(x, y) && s[ID(x, y)] == 0);
}

int connectedDfs (int x, int y, int &endX, int &endY, string &BOARD)
{
    visited[x][y] = true;

    if (x == endX && y == endY)
        return 1000;

    if (connectedUp[BOARD[ID(x, y)]][BOARD[ID(x - 1, y)]] && boundaries(x - 1, y) && !visited[x - 1][y])
        return connectedDfs(x - 1, y, endX, endY, BOARD) + 1;

    if (connectedDown[BOARD[ID(x, y)]][BOARD[ID(x + 1, y)]] && boundaries(x + 1, y) && !visited[x + 1][y])
        return connectedDfs(x + 1, y, endX, endY, BOARD) + 1;

    if (connectedRight[BOARD[ID(x, y)]][BOARD[ID(x, y + 1)]] && boundaries(x, y + 1) && !visited[x][y + 1])
        return connectedDfs(x, y + 1, endX, endY, BOARD) + 1;

    if (connectedLeft[BOARD[ID(x, y)]][BOARD[ID(x, y - 1)]] && boundaries(x, y - 1) && !visited[x][y - 1])
        return connectedDfs(x, y - 1, endX, endY, BOARD) + 1;

    return 1;
}

int calculateHeuristicValue (int &startX, int &startY, int &endX, int &endY, int &&dist, string &BOARD)
{
    memset(visited, false, sizeof(visited));
    int connectedFromStart = connectedDfs(startX, startY, endX, endY, BOARD);
    if (connectedFromStart >= 1000) return -1;

    memset(visited, false, sizeof(visited));
    int connectedFromEnd = connectedDfs(endX, endY, startX, startY, BOARD);
    //cout << 2 * N * N / (connectedFromStart + connectedFromEnd) + dist << " " << connectedFromStart << " " << connectedFromEnd << " " << 2* N * N / (connectedFromStart + connectedFromEnd) << " " << dist << "\n";
    return (N+1) * N * N / (connectedFromStart + connectedFromEnd) + 5 * dist;
}

void A_Star (STATE s)
{
    auto my_cmp = [](const STATE &s1, const STATE &s2) {
        return s1.hValue > s2.hValue;
    };
    priority_queue <STATE, vector<STATE>, decltype(my_cmp)> Q{ my_cmp };
    Q.push (s);
    closedList.insert (s.BOARD);

    int startX, startY, endX, endY;
    for (int i = 0; i < s.BOARD.size(); i++)
    {
        if (s.BOARD[i] >= 1 && s.BOARD[i] <= 4)
        {
            startX = i / N;
            startY = i % N;
        }
        else if (s.BOARD[i] >= 5 && s.BOARD[i] <= 8)
        {
            endX = i / N;
            endY = i % N;
        }
    }

    while (!Q.empty())
    {
        string BOARD = Q.top().BOARD;
        vector<pair<pair<int, int>, char> > moves = Q.top().moves;
        int hValue = Q.top().hValue;
        Q.pop();

        if (hValue == -1)
        {
            cout << moves.size() << endl;
            for (int i = 0; i < moves.size(); i++)
                cout << moves[i].x.y + 1 << " " << moves[i].x.x + 1 << " " << moves[i].y << endl;
            return;
        }

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if ((BOARD[ID(i, j)] >= 9 && BOARD[ID(i, j)] <= 12) || BOARD[ID(i, j)] == 17 || BOARD[ID(i, j)] == 18 || BOARD[ID(i, j)] == 21)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int new_x = i + d[k].x;
                        int new_y = j + d[k].y;

                        if (valid(new_x, new_y, BOARD))
                        {
                            swap (BOARD[ID(new_x, new_y)], BOARD[ID(i, j)]);
                            if (closedList.count (BOARD) == 0)
                            {
                                closedList.insert (BOARD);
                                vector<pair<pair<int, int>, char> > nextMoves = moves;
                                nextMoves.push_back(make_pair(make_pair(i, j), dir[k]));
                                Q.push({BOARD, nextMoves, calculateHeuristicValue(startX, startY, endX, endY, nextMoves.size(), BOARD)});
                            }
                            swap (BOARD[ID(new_x, new_y)], BOARD[ID(i, j)]);
                        }
                    }
                }
            }
        }
    }
}

int main()
{
    ifstream INPUT ("INPUT.txt");

    INPUT >> N;

    string START (N * N, 0);

    for (int i = 0; i < N; i++)
        for (int j = 0, a; j < N; j++)
        {
            INPUT >> a;
            START[ID(i, j)] = a;
        }

    calculateConnectedMatrix();

    A_Star({START, {}, 0});

    return 0;
}
