#include <iostream>
#include "PlanetWars.h"
#include <algorithm>


std::vector<Commitment> make_commitments(const PlanetWars pw,
                        const std::vector<Planet> defenders,
                        const std::vector<Request> requests) {
  std::vector<Commitment> commits;

  // TODO: May want to sort requests.

  // TODO: Consider incoming friendlies.

  for (int i = 0; i < requests.size(); ++i) {
    // Defenders should defend themselves first, if possible.
    const Request& r = requests[i];
    const Planet& p = pw.GetPlanet(r.TargetPlanet());

    int max_self = p.GrowthRate()*r.TurnsRemaining() + p.NumShips();

    bool send_self = true;
    for (int k = 0; k < commits.size(); ++k) {
      const Commitment& c2 = commits[k];
      if (c2.SourcePlanet() == r.TargetPlanet()) {
        send_self = false;
        break;
      }
    }

    int amount_to_send = 0;
    int still_need = r.NumShips();

    if (send_self) {
      if (r.NumShips() <= max_self)
        amount_to_send = r.NumShips();
      else
        amount_to_send = max_self;

      Commitment c(r.TargetPlanet(),
                  r.TargetPlanet(),
                  amount_to_send,
                  r.TurnsRemaining());
      commits.push_back(c);

      still_need -= amount_to_send;
    }
    
    if (!send_self || still_need > 0) {
      for (int j = 0; j < defenders.size(); ++j) {
        const Planet& d = defenders[j];

        if (d.PlanetID() == r.TargetPlanet())
          continue; // We've already handled this.

        // Prep time: number of turns before the fleet must leave to assist
        int prep_time = pw.Distance(d.PlanetID(), r.TargetPlanet())
                          - r.TurnsRemaining();

        if (prep_time < 0)
          continue; // Too far away to help.

        // Don't defend from a planet if the planet has a previous commitment.
        bool skip = false;
        for (int k = 0; k < commits.size(); ++k) {
          const Commitment& c2 = commits[k];
          if (c2.SourcePlanet() == d.PlanetID()) {
            skip = true;
            break;
          }
        }

        if (skip)
          continue;

        max_self = p.GrowthRate()*prep_time+p.NumShips();
        amount_to_send = 0;

        if (max_self > still_need)
          amount_to_send = still_need;
        else
          amount_to_send = max_self;

        Commitment c(d.PlanetID(),
                    r.TargetPlanet(),
                    amount_to_send, 
                    prep_time);
        commits.push_back(c);

        if (still_need <= 0)
          break;
      }

    }
  }

  return commits;
}

std::vector<Request> make_requests(const PlanetWars pw) {
  std::vector<Fleet> enemy_fleets = pw.EnemyFleets();
  std::vector<Request> requests;

  for (int i = 0; i < enemy_fleets.size(); ++i) {
    const Fleet& f = enemy_fleets[i];
    int pid = f.DestinationPlanet();

    if (f.Owner() != 1 && pw.GetPlanet(pid).Owner() == 1) {
      // An enemy fleet is headed for us!
      Request r(pid, f.NumShips(), f.TurnsRemaining());
      requests.push_back(r);
    }
  }
  
  return requests;
}

Request::Request(int target_planet,
        int num_ships,
        int turns_remaining) {
  target_planet_ = target_planet;
  num_ships_ = num_ships;
  turns_remaining_ = turns_remaining;
}

int Request::TargetPlanet() const {
  return target_planet_;
}

int Request::NumShips() const {
  return num_ships_;
}

int Request::TurnsRemaining() const {
  return turns_remaining_;
}

Commitment::Commitment(int source_planet,
        int target_planet,
        int num_ships,
        int turns_remaining) {
  target_planet_ = target_planet;
  source_planet_ = source_planet;
  num_ships_ = num_ships;
  turns_remaining_ = turns_remaining;
}

int Commitment::TargetPlanet() const {
  return target_planet_;
}

int Commitment::SourcePlanet() const {
  return source_planet_;
}

int Commitment::NumShips() const {
  return num_ships_;
}

int Commitment::TurnsRemaining() const {
  return turns_remaining_;
}

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
        if (num_ships > 0)
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

bool WeakestSort(const Planet& p1, const Planet& p2)
{
  return p1.NumShips() < p2.NumShips();
}

void DoTurn(const PlanetWars& pw) {

  static int turnNum = 0; 
  turnNum++;

  // (0) initial attack sequence... Attacks as many weak neutral planets as possible. 
  if (turnNum == 1){
    std::vector<Planet> my_planet = pw.MyPlanets();
    std::vector<Planet> Neutral = pw.NeutralPlanets();
    std::sort(Neutral.begin(), Neutral.end(), WeakestSort);

    int my_ships = my_planet[0].NumShips();
    for(int i = 0; i < Neutral.size(); i++){
      int num_ships = Neutral[i].NumShips()+1;
      if(my_ships < num_ships+1) return;
      pw.IssueOrder(my_planet[0].PlanetID(), Neutral[i].PlanetID(), num_ships);
      my_ships -= num_ships;
    }

    return;
  }
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
  // BEGIN step 2 of stage 2.
  // Defend planets.
  std::vector<Request> requests = make_requests(pw);
  std::vector<Commitment> commits = make_commitments(pw,
                                                    defenders,
                                                    requests);


  for (int i = 0; i < commits.size(); ++i) {
    const Commitment& c = commits[i];

    if (c.TurnsRemaining() == 0 && c.SourcePlanet() != c.TargetPlanet()) {
      pw.IssueOrder(c.SourcePlanet(), c.TargetPlanet(), c.NumShips());

      Planet p = pw.GetPlanet(c.SourcePlanet());
      //p.NumShips(p.NumShips() - c.NumShips());
    }
  }

  // Stream remaining ships to attack planets.
  for (int i = 0; i < defenders.size(); ++i) {
    const Planet& d = defenders[i];

    int saved_ships = 0;

    // TODO: improve me
    bool skip = false;
    for (int j = 0; j < commits.size(); ++j) {
      const Commitment& c = commits[j];
      if (c.SourcePlanet() == d.PlanetID()) {
        int buffer = d.GrowthRate();
        int can_make = d.GrowthRate()*c.TurnsRemaining()+d.NumShips();
        int needed = c.NumShips();
        if (can_make <= buffer + needed) {
          skip = true;
          break;
        }
      }
    }
    if (skip) continue;

    int closest_attacker = -1;
    int closest_dist = 29999;

    for (int j = 0; j < attackers.size(); ++j) {
      const Planet& a = attackers[j];

      int dist = pw.Distance(d.PlanetID(), a.PlanetID());
      if (dist < closest_dist) {
        closest_dist = dist;
        closest_attacker = a.PlanetID();
      }
    }

    int ships_to_send = d.NumShips() - 1;

    if (closest_attacker != -1 && ships_to_send > 0) {
      pw.IssueOrder(d.PlanetID(), closest_attacker, ships_to_send);
      Planet p = pw.GetPlanet(d.PlanetID());
      //p.NumShips(p.NumShips() - ships_to_send);
    }
  }
  // END step 2 of stage 2.
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
