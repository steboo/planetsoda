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
void MidPoint(std::vector<Planet> my_planets, double &x, double &y) {
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

double square(double a) {
  return a*a;
}

int Distance(double src_x, double src_y, double dest_x, double dest_y) {
  return (int)square(dest_y - src_y) + (int)square(dest_x - src_x);
}

void SendAttackers(std::vector<Planet> attacking_planets, const PlanetWars& pw)
{
	double x = 0; // x coord of source
	double y = 0; //y coord of source
	int weakest = -1; //Weakest planet
	int source = -1; //Current planet
	int source_num_ships = -9999999;
	double weak_score = -999999.0; //Weakest planet score
	
	std::vector<Planet> not_my_planets = pw.NotMyPlanets(); //Put the planets that aren't ours into a vector
	
	
	for(int i = 0;i<attacking_planets.size();++i)
	{
		//iterate through the all the attacking planets
		const Planet& myPlanet = attacking_planets[i];
		 for (int j = 0; j < not_my_planets.size(); ++j)
		 {
			const Planet& p = not_my_planets[j]; //find the weakest planet relative to the ith attacking planet
			x = myPlanet.X();
			y = myPlanet.Y();
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
void AssignRoles(std::vector<Planet> MyPlanets,std::vector<Planet> EnemyPlanets,
		 std::vector<Planet> &Attackers,std::vector<Planet> &Defenders)
{
  std::vector<Planet> attack;
  std::vector<Planet> defend = MyPlanets;

  //Find all attacking planets by finding all of our planets that are closest to enemy planets
  for(int i = 0; i < EnemyPlanets.size(); i++) {
    Planet dest = Planet(-1,0,0,0,0,0);
    double dest_score = 999999.0;
    for (int j = 0; j < MyPlanets.size(); j++) {
      const Planet& _p = MyPlanets[j];
      double score = (double)Distance(_p.X(), _p.Y(), EnemyPlanets[i].X(), EnemyPlanets[i].Y());
      if (score < dest_score) {
        dest_score = score;
        dest = _p;
      }
    }
    bool add = true;
    for (int k = 0; k < attack.size(); k++) {
      if(dest.PlanetID() == attack[k].PlanetID()) add = false;
    }
    if(add) attack.push_back(dest);
  }
  //Determine deffending planets by taking list of our planets and removing the attacking planets
  for (int i = 0; i < defend.size(); i++){
    for (int j = 0; j < attack.size(); j++){
      if(defend[i].PlanetID() == attack[j].PlanetID()) {
        defend.erase(defend.begin()+i);
        i = 0;
      }
    }
  }

  Attackers = attack;
  Defenders = defend;
}
void DoTurn(const PlanetWars& pw) {
  // (1) If we currently have a fleet in flight, just do nothing.
  if (pw.MyFleets().size() >= 50) {
    return;
  }


  double x = 0;
  double y = 0;

  std::vector<Planet> my_planets = pw.MyPlanets();
  MidPoint(my_planets, x,y);
//************Start assigning roles*************************//
  std::vector<Planet> attackers;
  std::vector<Planet> defenders;
  AssignRoles(pw.MyPlanets(),pw.EnemyPlanets(),
		 attackers,defenders);
//************ Start of Send Attackers *********************//
  SendAttackers(attackers, pw);
}


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
