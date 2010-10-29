#include <iostream>
#include "PlanetWars.h"

// The DoTurn function is where your code goes. The PlanetWars object contains
// the state of the game, including information about all planets and fleets
// that currently exist. Inside this function, you issue orders using the
// pw.IssueOrder() function. For example, to send 10 ships from planet 3 to
// planet 8, you would say pw.IssueOrder(3, 8, 10).
//
// There is already a basic strategy in place here. You can use it as a
// starting point, or you can throw it out entirely and replace it with your
// own. Check out the tutorials and articles on the contest website at
// http://www.ai-contest.com/resources.
void MidPoint(std::vector<Planet> my_planets, int &x, int &y) {
  x = 0;
  y = 0;
  for (int i = 0; i < my_planets.size(); ++i) {
    const Planet& p = my_planets[i];
    x += p.X();
    y += p.Y();
  }

  x /= my_planets.size();
  y /= my_planets.size();
}

int square(int a) {
  return a*a;
}

int Distance(int src_x, int src_y, int dest_x, int dest_y) {
  return square(dest_y - src_y) + square(dest_x - src_x);
}

void SendAttackers(std:vector<Planet> attacking_planets, const PlanetWars& pw)
{
	int x = 0; // x coord of source
	int y = 0; //y coord of source
	int weakest = -1; //Weakest planet
	int source = -1; //Current planet
	double weak_score = -999999.0; //Weakest planet score
	
	std::vector<Planet> not_my_planets = pw.NotMyPlanets(); //Put the planets that aren't ours into a vector
	
	
	for(int i = 0;i<attacking_planets.size();++i)
	{
		//iterate through the all the attacking planets
		const Planet& myPlanet = attacking_planets[i];
		 for (int j = 0; j < not_my_planets.size(); ++j)
		 {
			const Planet& p = not_my_planets[j] //find the weakest planet relative to the ith attacking planet
			x = myPlanet.X;
			y = myPlanet.Y;
			double cur_score = (double)p.GrowthRate()/((double)p.NumShips()*(double)Distance(x, y, p.X(), p.Y())); //calculate score of weakest planet
			if (cur_score > weak_score) //check if current score is greater than the previous score
			{
				//if yes, set jth planet to the weakest planet.
				weak_score = cur_score;
				weakest = p.PlanetID();
			}
		}
		source = myPlanet.PlanetID(); //set the source to the ith planet
		source_num_ships = myPlanet.NumShips(); //set number of ships to the ith planet
		
		//check if it is a valid planet, send half the fleet
		if (source >= 0 && weakest >= 0) 
			{
				int num_ships = source_num_ships / 2;
				pw.IssueOrder(source, weakest, num_ships);
			}	
	source = -1;
	weakest = -1;
	}
}
void DoTurn(const PlanetWars& pw) {
  // (1) If we currently have a fleet in flight, just do nothing.
  if (pw.MyFleets().size() >= 50) {
    return;
  }
	
}

void 

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
  std::string current_line;
  std::string map_data;
  while (true) {
    int c = std::cin.get();
    current_line += (char)c;
    if (c == '\n') {
      if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
        PlanetWars pw(map_data);
        map_data = "";
        DoTurn(pw);
  pw.FinishTurn();
      } else {
        map_data += current_line;
      }
      current_line = "";
    }
  }
  return 0;
}
