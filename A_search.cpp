#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};

//Directional deltas
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

vector<State> ParseLine (string str)
{
  istringstream my_stream(str);
  int i;
  char c;
  vector<State> vec;

  while(my_stream >> i >> c && c == ',')
  {
    switch (i)
    {
      case 0: vec.push_back(State::kEmpty);
        break;
      default: vec.push_back(State::kObstacle);
        break;
    }
  }

  return vec;
}

vector<vector<State>> ReadBoardFile (string path)
{
  ifstream file(path);
  string line;
  vector<vector<State>> board;

  if (file)
  {
    while (getline(file, line))
    {
      vector<State> row = ParseLine(line);
      board.push_back(row);
    }
  }
  return board;
}

bool Compare (vector<int> node1, vector<int> node2)
{
  int f1 = node1[2] + node1[3];
  int f2 = node2[2] + node2[3];

  return f1 > f2;
}

void CellSort (vector<vector<int>> *v)
{
  sort(v->begin(), v->end(), Compare);
}

//Calculates the Manhattan distance
int Heuristic (int x1, int y1, int x2, int y2)
{
  return abs(x2 - x1) + abs(y2 - y1);
}

bool CheckValidCell (int x, int y, vector<vector<State>> &grid)
{
  bool ongrid_x = (x >= 0 && x < grid.size());
  bool ongrid_y = (y >= 0 && y < grid[0].size());

  if (ongrid_x && ongrid_y)
  {
    return grid[x][y] == State::kEmpty;
  }

  return false;
}

void AddToOpen (int x, int y, int g, int h, vector<vector<int>> &openList, vector<vector<State>> &grid)
{
  openList.push_back(vector<int>{x, y, g, h});
  grid[x][y] = State::kClosed;
}

void ExpandNeighbors (const vector<int> &currentNode, vector<vector<int>> &openList, vector<vector<State>> &grid, int goal[2])
{
  int x = currentNode[0];
  int y = currentNode[1];
  int g = currentNode[2];

  for (int i = 0; i < 4; i++)
  {
    int x2 = x + delta[i][0];
    int y2 = y + delta[i][1];

    if (CheckValidCell(x2, y2, grid))
    {
      int g2 = g + 1;
      int h2 = Heuristic(x2, y2, goal[0], goal[1]);
      AddToOpen(x2, y2, g2, h2, openList, grid);
    }
  }
}

//Implements the A* algorithm.
vector<vector<State>> Search (vector<vector<State>> grid, int start[2], int end[2])
{
  vector<vector<int>> open{};

  int x = start[0];
  int y = start[1];
  int g = 0;
  int h = Heuristic(x, y, end[0], end[1]);
  AddToOpen(x, y, g, h, open, grid);

  while(!open.empty())
  {
    CellSort(&open);
    auto currentNode = open.back();
    open.pop_back();
    x = currentNode[0];
    y = currentNode[1];
    grid[x][y] = State::kPath;

    //Checks if we are done.
    if (x == end[0] && y == end[1])
    {
      grid[start[0]][start[1]] = State::kStart;
      grid[end[0]][end[1]] = State::kFinish;
      return grid;
    }

    ExpandNeighbors(currentNode, open, grid, end);

  }

  cout << "No path found!\n";
  return vector<vector<State>>{};
}

string CellString (State value)
{
  switch(value)
  {
    case State::kEmpty: return "  ";
      break;
    case State::kClosed: return "❌  ";
      break;
    case State::kPath: return "🐕‍  ";
      break;
    case State::kStart: return "🚦  ";
      break;
    case State::kFinish: return "🏁  ";
      break;
    default: return "💧  ";
      break;
  }
}

void PrintBoard (vector<vector<State>> v)
{
  cout << "\n";
  for (auto i : v)
  {
    for (auto j : i)
    {
      cout << CellString(j);
    }
    cout << "\n";
  }
}

int main ()
{
  string path = "/Users/michelle.husbands/Documents/Udacity/Workspaces/Language Basics/files/1.board";
  auto board = ReadBoardFile(path);
  int start[2] = {0, 0};
  int end[2] = {4, 5};
  auto solution = Search(board, start, end); //while loop that continues as long as there are free nodes open to explore.

  PrintBoard(solution);
}
