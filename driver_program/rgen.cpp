// g++ -o build-rgen/rgen rgen.cpp -std=c++11
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <cstdlib>
#include "rgen.hpp"

using namespace std;

// Helper Functions
int random_int(int min, int max)
{
  ifstream urandom("/dev/urandom");
  if (urandom.fail())
  {
    cerr << "Error: Cannot Open /dev/urandom" << endl;
    return 1;
  }

  int range = (max - min) + 1;
  int num = 0;
  while (true)
  {
    urandom.read((char *)&num, 1);
    num = min + num % range;

    if (num >= min && num <= max)
    {
      urandom.close();
      break;
    }
  }
  return num;
};

// Graph Functions
void RandomGen::output_streets()
{
  string output;
  int len = this->street_names.size();
  if (len > 0)
  {
    for (int i = 0; i < len; i++)
    {
      output = output + "add \"" + this->street_names[i] + "\"";
      for (int j = 0; j < this->street_points[i].size(); j++)
      {
        pair<int, int> &point = this->street_points[i][j];
        output += " (" + to_string(point.first) + ',' + to_string(point.second) + ')';
      }
      output += "\n";
    }
    output += "gg";
    cout << output << endl;
  }
};

void RandomGen::removing_streets()
{
  string output;
  int len = this->street_names.size();
  for (int i = 0; i < len; i++)
  {
    output = output + "rm \"" + this->street_names[i] + "\"" + "\n";
  }
  cout << output << endl;
};

string RandomGen::generate_street_names(int len)
{
  string alphabet = "abcdefghijklmnopqrstuvwxyz";
  string street_name;
  int position;
  while (street_name.size() != len)
  {
    position = random_int(0, 25);
    street_name += alphabet.substr(position, 1);
  }
  return street_name;
};

void RandomGen::create_street_list(int s_val, int n_val, int c_val)
{
  int num_streets = random_int(2, s_val);
  this->street_names.clear();

  for (int i = 0; i < num_streets; i++)
  {
    // Generate Street Name then push into the streetnames array
    string street_name = this->generate_street_names(7);
    // Check if the street name exists?? Might not need to since the there is a 1 / 26 ^ 7 chance of repeating

    this->street_names.push_back(street_name);
    this->street_points.push_back(this->create_street_points(n_val, c_val));
  }
};

vector<pair<int, int>> RandomGen::create_street_points(int n_val, int c_val)
{
  int line_segments = random_int(1, n_val);
  int coordinate_values = c_val;
  // Counter for attempts to generate
  int counter = 0;

  // Points and lines
  vector<pair<int, int>> line;
  pair<int, int> point;

  for (int i = 0; i <= line_segments; i++)
  {
    point.first = random_int(-coordinate_values, coordinate_values);
    point.second = random_int(-coordinate_values, coordinate_values);
    // Point Validation
    if (valid_line_segment(line, point))
    {
      line.push_back(point);
      counter = 0;
    }
    else
    {
      counter++;
      if (counter > 25)
      {
        cerr << "Error: Failed to Generate a valid input after " << counter - 1 << " attempts" << endl;
        counter = 0;
        exit(1);
      }
    }
  }

  return line;
};

bool RandomGen::valid_line_segment(vector<pair<int, int>> line, pair<int, int> point)
{
  for (int i = 0; i < line.size(); i++)
  {
    // Valid Line segments cannot have any point overlap
    if (line[i].first == point.first && line[i].second == point.second)
    {
      return false;
    }
    // Check for self intersection and for overlapping lines
    for (int j = i + 1; j < line.size(); j++)
    {
      if (i + 1 < line.size() && j + 1 < line.size())
      {
        if (this->line_overlap(line[i].first, line[i].second, line[i + 1].first, line[i + 1].second, line[j].first, line[j].second, line[j + 1].first, line[j + 1].second) == true ||
            this->line_intersect(line[i].first, line[i].second, line[i + 1].first, line[i + 1].second, line[j].first, line[j].second, line[j + 1].first, line[j + 1].second) == true)
        {
          return false;
        };
      }
    }
  }

  return true;
};

bool RandomGen::line_overlap(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
  if ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4) == 0 && (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4) == 0 && (x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2) == 0)
  {
    return true;
  }
  return false;
};

bool RandomGen::line_intersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
  float denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4); // Parrallel of coincident
  if (denominator == 0)
  {
    return false;
  }

  float u_num = (x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3);
  float t_num = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);

  float t = t_num / denominator;
  float u = -(u_num / denominator);

  if (t < 0 || t > 1 || u < 0 || u > 1)
  {
    return false;
  }

  return true;
};

int main(int argc, char **argv)
{
  string s, n, l, c;
  // Initialize the default values
  // S val = # of streets, N val = # of line segmants (range), L val = # of seconds waiting, C val = coordinate range
  int s_val = 10, n_val = 5, l_val = 5, c_val = 20;
  int option;
  opterr = 0;

  while ((option = getopt(argc, argv, "s:n:l:c:?")) != -1)
  {
    switch (option)
    {
    case 's':
      s_val = atoi(optarg);
      if (s_val < 2)
      {
        cerr << "Error: Street Options -s must have a value greater than 2" << endl;
        return 1;
      }
      break;

    case 'n':
      n_val = atoi(optarg);
      if (n_val < 1)
      {
        cerr << "Error: Number of line segments option -n must be greater than 1" << endl;
        return 1;
      }
      break;

    case 'l':
      l_val = atoi(optarg);
      if (l_val < 5)
      {
        cerr << "Error: Wait time option -l must be greater than 5" << endl;
        return 1;
      }
      break;

    case 'c':
      c_val = atoi(optarg);
      if (c_val < 1)
      {
        cerr << "Error: Coordinate value option -c must be greater than 1" << endl;
        return 1;
      }
      break;
    case '?':
      cerr << "Error: Unknown Options provided" << endl;
      return 1;
    default:
      return 0;
    }
  };

  while (true)
  {
    RandomGen generator;
    generator.create_street_list(s_val, n_val, c_val);
    generator.output_streets();

    int lag_time = random_int(5, l_val);
    generator.removing_streets();
    sleep(lag_time);
  }
  return 0;
};