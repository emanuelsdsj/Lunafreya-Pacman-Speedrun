#ifndef STATE_HH
#define STATE_HH

#include <unordered_set>

#include "../utils.hh"
#include "position.hh"
#include "agents/ghost_state.hh"
#include "../game.hh"
#include <ctime>
#include <cmath>

struct State {
    static const uchar PILL = '.';
    static const uchar POWER_PILL = 'o';
    static const uchar PACMAN = 'P';
    static const uchar BLINKY = 'B';
    static const uchar PINKY = 'K';
    static const uchar INKY = 'I';
    static const uchar CLYDE = 'C';
    static const uchar FREE = ' ';
    static const uchar WALL = '%';
    static const uchar SPAWN_AREA = 'X';

    // Contains the current state of the maze
    // NOTE THAT PACMAN AND GHOSTS ARE **NOT** HERE
    // Instead, their positions are encoded as a Position member
    // in their state instances
    Matrix<char> maze;

    // Contains a list of all valid positions in the board
    static vector<Position> valid_positions;
    static vector<Position> valid_positions_no_spawn;
    static int total_pills;
    static int max_dist;
    static int total_powerpills;
    static int total_normal_pills;
    static int n_ghosts;

    unordered_set<int> pills;
    unordered_set<int> powerpills;

    static uniform_int_distribution<> distribution_valid_pos_no_spawn;

    int round;

    // Number of power pill rounds left. 0 if no power pill in action
    int n_rounds_powerpill;

    int n_normal_pills_left;

    int n_powerpills_left;
    //Lunafreya Changes
    int pills_eaten;
    int powerpills_eaten;
    double total_points;
    double combo;
    double ghost_eaten_total_points;


    vector<Ghost_State> ghosts;
    Agent_State pacman;

    State() :
        round(0), n_rounds_powerpill(0), n_normal_pills_left(0),
        n_powerpills_left(0) {}

    inline void operator=(const State& o) {
        maze = o.maze;
//Lunafreya Changes
        n_normal_pills_left = o.n_normal_pills_left;
        n_powerpills_left = o.n_powerpills_left;
        ghosts = o.ghosts;
        pacman = o.pacman;
        round = o.round;
        n_rounds_powerpill = o.n_rounds_powerpill;
        pills = o.pills;
        ghost_eaten_total_points = o.ghost_eaten_total_points;
        pills_eaten = o.pills_eaten;
        powerpills_eaten = o.powerpills_eaten;
        total_points = o.total_points;
        combo = o.combo;
    }

    friend ostream& operator<<(ostream& o, const State& s) {
        Matrix<char> maze_aux = s.maze;

        for (int i = 0; i < maze_aux.rows(); ++i)
            for (char& d : maze_aux[i]) if (d == State::SPAWN_AREA) d = State::FREE;

        maze_aux[s.pacman.pos.i][s.pacman.pos.j] = 'P';

        //for (const Ghost_State& ghost : s.ghosts)
            //if (ghost.n_rounds_revive == 0) {
                //if (s.is_scared(ghost)) maze_aux[ghost.pos.i][ghost.pos.j] = 'S';
                //else if (ghost.behaviour == Ghost_Behaviour::SCATTER) maze_aux[ghost.pos.i][ghost.pos.j] = 'R';
                //else maze_aux[ghost.pos.i][ghost.pos.j] = 'C';
            //}
        //Lunafreya
        for (const Ghost_State& ghost : s.ghosts) {
            if (ghost.n_rounds_revive == 0) {
                if (s.is_scared(ghost)) maze_aux[ghost.pos.i][ghost.pos.j] = 'S';
                else {
                    switch(ghost.typeGhost) {
                        case 0: maze_aux[ghost.pos.i][ghost.pos.j] = 'B'; break;
                        case 1: maze_aux[ghost.pos.i][ghost.pos.j] = 'K'; break;
                        case 2: maze_aux[ghost.pos.i][ghost.pos.j] = 'I'; break;
                        case 3: maze_aux[ghost.pos.i][ghost.pos.j] = 'C'; break;
                    }
                }
            }
        }
        o << "Round " << s.round << ":" << endl
          << maze_aux << endl
          << "Pills left: " << s.n_normal_pills_left << endl
          << "Powerpills left: " << s.n_powerpills_left << endl
          << "Rounds powerpill: " << s.n_rounds_powerpill << endl
          << "Combo: " << s.combo << endl
          << "Points: " << s.total_points;

        return o;
    }

    inline vector<int> ghosts_in_position(const Position& pos) const {
        vector<int> ghosts_pos;

        for (uint i = 0; i < ghosts.size(); ++i) {
            if (ghosts[i].pos.i == pos.i and ghosts[i].pos.j == pos.j)
                ghosts_pos.push_back(i);
        }

        return ghosts_pos;
    }


    inline int distance_closest_harmful_ghost(const Position& pos) const {
        int distance = MAXINT;

        for (const Ghost_State& ghost : ghosts) {
            if (ghost.is_alive() and not is_scared(ghost)) {
                distance = min(distance, ghost.pos.manhattan(pos));
            }
        }

        return distance;
    }

    inline bool valid_to_move(const Position& pos) const {
        return maze[pos.i][pos.j] != WALL;
    }

    inline bool has_pill(const Position& pos) const {
        return maze[pos.i][pos.j] == PILL;
    }

    inline bool has_powerpill(const Position& pos) const {
        return maze[pos.i][pos.j] == POWER_PILL;
    }

    inline bool has_any_pill(const Position& pos) const {
        return has_pill(pos) or has_powerpill(pos);
    }

    inline static Position random_valid_pos() {
        return valid_positions_no_spawn[distribution_valid_pos_no_spawn(Arguments::random_generator)];
    }

    inline static bool validPosition(Position pos) {
        for (Position& posValid : valid_positions_no_spawn) {
            if(posValid == pos)
                return true;
        }
        return false;
    }

    inline bool is_scared(const Ghost_State& ghost) const {
        return n_rounds_powerpill > 0 and ghost.maybe_scared;
    }
};

int State::total_pills;
int State::max_dist;
int State::total_normal_pills;
int State::total_powerpills;
int State::n_ghosts;
vector<Position> State::valid_positions;
vector<Position> State::valid_positions_no_spawn;
uniform_int_distribution<> State::distribution_valid_pos_no_spawn;
#endif
