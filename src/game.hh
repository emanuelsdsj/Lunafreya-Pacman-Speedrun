#ifndef GAME_HH
#define GAME_HH

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <random>
#include <algorithm>
#include <vector>

#include "agents/agent.hh"
#include "arguments.hh"
#include "utils.hh"
#include "state/position.hh"
#include "agents/ghosts/ghost_agent.hh"
#include "state/state.hh"
#include "pathfinding/pathfinding.hh"
#include "agents/pacman/rl/rl_pacman_agent_inputs.hh"
#include "stopwatch.hh"

struct GameResult {
    GameResult() {}

    bool won;
    double completion;
    State state;
};

class Game {
private:

    vector<Agent*> ghosts;

    float ghost_speed(int ghost_id) {
        return state.is_scared(state.ghosts[ghost_id]) ?
               Arguments::ghost_speed*Arguments::ghost_afraid_speed_fraction :
               Arguments::ghost_speed;
    }

    // TODO: Take movement step in account?
    bool game_over_on_collision(Ghost_State& ghost) {
        if (state.is_scared(ghost)) {
            pacman->notify_killed_ghost();
            if(state.combo >= 200) {
                state.combo = state.combo * 2;
                state.ghost_eaten_total_points = state.ghost_eaten_total_points + state.combo;
            }
            else {
                state.combo = 200;
                state.ghost_eaten_total_points = state.ghost_eaten_total_points + state.combo;
            }
            ghost.maybe_scared = false;
            ghost.n_rounds_revive = Arguments::n_rounds_ghost_revive;
            ghost.pos = Position(-1,-1);
        }
        else game_over = true;

        return game_over;
    }

    void update_ghost_states() {
        for (Ghost_State& ghost : state.ghosts) {
            if (ghost.n_rounds_left_behaviour-- == 0) {
                normal_distribution<float> distribution;

                switch (ghost.behaviour) {
                    case SCATTER:
                        distribution = normal_distribution<float>(ghost.chase_cycle_rounds, Arguments::cycle_rounds_stdev);
                        ghost.behaviour = Ghost_Behaviour::CHASE;
                        ghost.chase_cycle_rounds = min(1000000, int(ghost.chase_cycle_rounds*Arguments::chase_cycle_factor));
                        break;
                    case CHASE:
                        if (ghost.scatter_cycle_rounds >= 1) {
                            distribution = normal_distribution<float>(ghost.scatter_cycle_rounds, Arguments::cycle_rounds_stdev);
                            ghost.behaviour = Ghost_Behaviour::SCATTER;
                            ghost.scatter_cycle_rounds /= Arguments::scatter_cycle_factor;
                            ghost.scatter_pos = State::random_valid_pos();
                        }
                        break;
                    default: ensure(false, "Invalid ghost behaviour");
                }

                ghost.n_rounds_left_behaviour = max(1, (int) distribution(Arguments::random_generator));
            }

            if (ghost.behaviour == SCATTER) {
                while (ghost.scatter_pos == ghost.pos) ghost.scatter_pos = State::random_valid_pos();
            }
        }
    }

public:
    Agent* pacman;

    GameResult result;

    State initialState;
    State state;

    bool loaded_maze;

    bool game_over;

    Game() : pacman(NULL), loaded_maze(false), game_over(false) {}

    void set_ai(Agent* pacman) {
        this->pacman = pacman;
    }

    void load_maze() {
        loaded_maze = true;

        ifstream maze_file;
        maze_file.open(Arguments::maze_path);

        if (not maze_file.is_open())
            error("Invalid maze file path '" + Arguments::maze_path + "'");

        int rows, cols;
        maze_file >> rows >> cols;
        state.maze = Matrix<char>(rows, cols);
        uint valid_index = 0;
        PathMagic::index_from_pos = vector<vector<int>>(rows, vector<int>(cols, -1));
        State::valid_positions.clear();
        State::valid_positions_no_spawn.clear();

        maze_file >> std::noskipws;
        for (int i = 0; i < rows; ++i) {
            char dummy; maze_file >> dummy;
            for (int j = 0; j < cols; ++j) {
                char cell; maze_file >> cell;
                state.maze[i][j] = cell != State::PACMAN and cell != State::BLINKY and cell != State::PINKY and cell != State::INKY and cell != State::CLYDE ? cell : State::FREE;

                state.n_normal_pills_left += cell == State::PILL;
                state.n_powerpills_left += cell == State::POWER_PILL;
                state.ghost_eaten_total_points = 0;
                state.pills_eaten = 0;
                state.powerpills_eaten = 0;
                state.total_points = 0;
                state.combo = 0;

                if (cell == State::POWER_PILL) {
                    state.powerpills.insert(state.valid_positions.size());
                    state.pills.insert(state.valid_positions.size());
                }
                else if (cell == State::PILL) {
                    state.pills.insert(state.valid_positions.size());
                }

                if (cell != State::WALL) {
                    state.valid_positions.push_back(Position(i, j));
                    if (cell != State::SPAWN_AREA and cell != State::BLINKY and cell != State::PINKY and cell != State::INKY and cell != State::CLYDE) state.valid_positions_no_spawn.push_back(Position(i, j));
                    PathMagic::index_from_pos[i][j] = valid_index++;
                }

                if (cell == State::PACMAN) state.pacman = Agent_State(Position(i, j), Direction::UP);
                else if (cell == State::BLINKY) {
                    state.maze[i][j] = State::SPAWN_AREA;
                    state.ghosts.push_back(Ghost_State(Position(i, j), Direction::LEFT, 1));
                    ghosts.push_back((Agent*)new Ghost_Agent(90));
                }
                else if (cell == State::PINKY) {
                    state.maze[i][j] = State::SPAWN_AREA;
                    state.ghosts.push_back(Ghost_State(Position(i, j), Direction::UP, 2));
                    ghosts.push_back((Agent*)new Ghost_Agent(60));
                }
                else if (cell == State::INKY) {
                    state.maze[i][j] = State::SPAWN_AREA;
                    state.ghosts.push_back(Ghost_State(Position(i, j), Direction::UP, 3));
                    ghosts.push_back((Agent*)new Ghost_Agent(30));
                }
                else if (cell == State::CLYDE) {
                    state.maze[i][j] = State::SPAWN_AREA;
                    state.ghosts.push_back(Ghost_State(Position(i, j), Direction::UP, 4));
                    ghosts.push_back((Agent*)new Ghost_Agent(0));
                }
            }
        }

        state.total_pills = state.n_normal_pills_left + state.n_powerpills_left;
        state.total_powerpills = state.n_powerpills_left;
        state.total_normal_pills = state.n_normal_pills_left;
        state.distribution_valid_pos_no_spawn = uniform_int_distribution<>(0, state.valid_positions_no_spawn.size() - 1);
        state.n_ghosts = state.ghosts.size();

        SeenMatrix::init(rows, cols);
        PathMagic::compute(state);
        RL_Pacman_Agent_Inputs::precompute();

        initialState = state;
    }

    void reset() {
        ensure(loaded_maze, "Try to reset without a loaded maze");
        state = initialState;
        game_over = false;
    }

    GameResult& play() {
        ensure(loaded_maze, "Try to play without a loaded maze");
        ensure(pacman != NULL, "Try to play without an agent");

        if (Arguments::plays == 1) cout << state << endl;
        Stopwatch<> sw;
        //Lunafreya Changes
        while (/*state.n_powerpills_left + state.n_normal_pills_left > 0 ||*/ state.total_points < 5000) { // break if game_over
            //if(duration_cast<double>(sw.elapsed()) > 180){
                //game_over = true;
                //sw.reset();
                //break;
            //}
            if(state.n_powerpills_left + state.n_normal_pills_left <= 0 && state.total_points < 5000){
                game_over = true;
                sw.reset();
                break;
            }
            if (Arguments::plays == 1) {
                cout.flush();
                usleep(Arguments::visualization_speed);
            }

            update_ghost_states();

            ++state.round;

            state.n_rounds_powerpill = max(0, state.n_rounds_powerpill - 1);

            Direction pacman_direction = pacman->take_action(state, 0);

            Position pacman_previous_pos = state.pacman.pos;
            vector<Position> ghost_previous_pos(state.n_ghosts);
            for (int i = 0; i < state.n_ghosts; ++i) ghost_previous_pos[i] = state.ghosts[i].pos;

            // TODO: Maybe process movements stochastically to guarantee equality?

            if (state.pacman.dir == pacman_direction) {
                state.pacman.step += Arguments::pacman_speed;

                if (state.pacman.step >= 1) {
                    --state.pacman.step;

                    Position next_pos = state.pacman.pos.move_destination(pacman_direction);
                    char cell_content = state.maze[next_pos.i][next_pos.j];


                    if (cell_content == State::WALL) {
                        _debug("Pacman crashes with wall");
                    }
                    else if (cell_content == State::SPAWN_AREA) {
                        _debug("Pacman crashes with spawn area");
                        assert(false);
                    }
                    else {
                        state.pacman.pos.move(pacman_direction);

                        vector<int> ghosts_pos = state.ghosts_in_position(next_pos);

                        for (int ghost_id : ghosts_pos)
                            if (game_over_on_collision(state.ghosts[ghost_id]))
                                goto end;

                        switch(cell_content) {
                            case State::PILL:
                                state.maze[next_pos.i][next_pos.j] = State::FREE;
                                --state.n_normal_pills_left;
                                ++state.pills_eaten;
                                state.pills.erase(PathMagic::id(next_pos));
                                pacman->notify_eaten_pill();
                                break;
                            case State::POWER_PILL:
                                state.maze[next_pos.i][next_pos.j] = State::FREE;
                                --state.n_powerpills_left;
                                ++state.powerpills_eaten;
                                state.pills.erase(PathMagic::id(next_pos));
                                state.powerpills.erase(PathMagic::id(next_pos));
                                pacman->notify_eaten_powerpill();

                                for (Ghost_State& ghost : state.ghosts) ghost.maybe_scared = true;

                                state.n_rounds_powerpill = Arguments::n_rounds_powerpill;
                                break;
                            case State::FREE: break;
                            default: ensure(false, "Invalid cell_content character '%c'", cell_content);
                        }
                    }
                }
            }
            else {
                pacman->notify_reverse_direction();
                state.pacman.step = (1 - state.pacman.step)*Arguments::pacman_speed;
                state.pacman.dir = pacman_direction;
            }

            for (int i = 0; i < min(1 + state.round/Arguments::n_rounds_between_ghosts_start, (int) ghosts.size()); ++i) {
                Ghost_State& ghost = state.ghosts[i];

                if (ghost.is_alive()) {
                    Direction ghost_direction = ghosts[i]->take_action(state, i);

                    if (ghost_direction == ghost.dir) {
                        ghost.step += ghost_speed(i);

                        if (ghost.step >= 1) {
                            --ghost.step;

                            Position next_pos = ghost.pos.move_destination(ghost_direction);
                            char cell_content = state.maze[next_pos.i][next_pos.j];

                            if (cell_content == State::WALL) {
                                _debug("Ghost #%d at [%d,%d] crashes with wall", i, ghost.pos.i, ghost.pos.j);
                            }
                            else {
                                ghost.pos.move(ghost_direction);

                                if (state.pacman.pos == next_pos and game_over_on_collision(ghost))
                                    goto end;
                            }
                        }
                    }
                    else {
                        ghost.step = (1 - ghost.step)*ghost_speed(i);
                        ghost.dir = ghost_direction;
                    }
                }
                else if (ghost.n_rounds_revive == 1) {
                    ghost.pos = ghost.initial_pos;
                    ghost.n_rounds_revive = 0;
                }
                else {
                    --ghost.n_rounds_revive;
                }

                state.ghosts[i].prev = ghost_previous_pos[i];
            }
            if(state.n_rounds_powerpill <= 0)
                state.combo = 0;
            
            state.pacman.prev = pacman_previous_pos;

            if (Arguments::plays == 1) cout << state << endl;
            //Lunafreya Changes
            //std::cout << "Elapsed: " << duration_cast<double>(sw.elapsed()) << " " << state.total_points << '\n';
            state.total_points = state.ghost_eaten_total_points + (state.powerpills_eaten * 50) + (state.pills_eaten * 10);
        }

        end:
        if (Arguments::plays == 1) {
            cout << state << endl;
            cout << (game_over ? "LOST" : "WON") << endl;
        }
        this->result.won = not game_over;
        this->result.completion = 1 - (state.n_normal_pills_left + state.n_powerpills_left)/double(state.total_pills);
        //this->result.completion = 1 - state.total_points/2000;
        pacman->notify_game_result(this->result.won);
        return this->result;
    }
};

#endif
