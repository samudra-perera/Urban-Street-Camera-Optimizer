#include <vector>
#include <string>
using namespace std;

// Helper functions
// Function to generate a random integer between min and max
int random_int(int min, int max);

// Class for the graph
class RandomGen
{
public:
  // Vector of strings to hold the street names
  vector<string> street_names;
  vector<vector<pair<int, int>>> street_points;

  // Function that uses random_int to generate random street names
  string generate_street_names(int len);

  // Function that uses generate_street_names along with points to populate street_names list
  void create_street_list(int s_val, int n_val, int c_val);
  // Function used to generate the points making the line segments
  vector<pair<int, int>> create_street_points(int n_val, int c_val);
  // Function to confirm that the line segments are indeed valid
  bool valid_line_segment(vector<pair<int, int>> line, pair<int, int> point);

  // Function to check line overlap within streets
  bool line_overlap(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

  // Function to check line intersection within streets
  bool line_intersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

  // Outputting streets
  void output_streets();

  // Removing Streets
  void removing_streets();
};