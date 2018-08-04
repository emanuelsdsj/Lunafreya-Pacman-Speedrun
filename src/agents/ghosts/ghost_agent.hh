#ifndef GHOST_AGENT_HH
#define GHOST_AGENT_HH

#include "../../state/direction.hh"
#include "../agent.hh"
#include "../../utils.hh"
#include "../../state/position.hh"
#include "../../pathfinding/pathfinding.hh"
#include <cmath>

class Ghost_Agent: public Agent {

public:

    virtual ~Ghost_Agent() {}
    //lunafreya
    int intelligenceGhost;
    Ghost_Agent(int intelligence) {
        intelligenceGhost = intelligence;
    }
    //lunafreya
    inline Direction take_action(const State& s, uint ghost_id) {
        const Ghost_State& ghost = s.ghosts[ghost_id];
        //int randomValue = rand() % 100 + 1;
        if (ghost.pos != ghost.prev) {
            if (s.is_scared(ghost)) {
                return PathMagic::try_to_avoid(ghost.pos, PathMagic::from_to(ghost.pos, s.pacman.pos));
            }
            else {
                Direction d;
                //cout << s.pacman.pos.i << " - " << s.pacman.pos.j << endl;
                switch(ghost.behaviour) {
                    case SCATTER:
                        d = PathMagic::from_to(ghost.pos, ghost.scatter_pos); break;
                    case CHASE:
                        switch(ghost.typeGhost) {
                            case 1: d = PathMagic::from_to(ghost.pos, s.pacman.pos); break; // Blinky - only chase
                            case 2: d = PathMagic::from_to(ghost.pos, pinkyAction(s)); break; // 4 pos in front of pacman
                            case 3: d = PathMagic::from_to(ghost.pos, inkyAction(s)); break; // (Pacman pos - Blinky pos) + blinky pos = nearest pos
                            case 4: if(clydeAction(s)) {// run from pacman when close
                                        d = PathMagic::try_to_avoid(ghost.pos, PathMagic::from_to(ghost.pos, s.pacman.pos)); break;
                                    }
                                    d = PathMagic::from_to(ghost.pos, s.pacman.pos); break;
                            default: ensure(false, "Invalid ghost behaviour enum");
                        }
                    
                }

                //ensure(pr.found, "Path not found!");
                if (d == Direction::STAY) _debug("No movement for ghost #%d!", ghost_id);
                return d;
            }
        }
        else return ghost.dir;
    }

    inline Position pinkyAction(const State& s) {
        Position pacmanPosMatrix;
        Position finalPos = Position(s.pacman.pos.i, s.pacman.pos.j);
        int lastMov = 0;
        bool posMov = false;
        // 1 = RIGHT
        // 2 = UP
        // 3 = DOWN
        // 4 = LEFT
       if(s.pacman.dir == Direction::RIGHT) {
            for(int k = 0; k < 4; k++) {
                posMov = false;
                pacmanPosMatrix = Position(finalPos.i, finalPos.j + 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 2 && posMov == false) {
                    lastMov = 1;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i - 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 4 && posMov == false) {
                    lastMov = 3;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i + 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 3 && posMov == false) {
                    lastMov = 4;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j - 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 1 && posMov == false) {
                    lastMov = 2;
                    posMov = true;
                }
                if(lastMov == 1)
                    finalPos = Position(finalPos.i, finalPos.j + 1);
                else if(lastMov == 2)
                    finalPos = Position(finalPos.i, finalPos.j - 1);
                else if(lastMov == 3)
                    finalPos = Position(finalPos.i - 1, finalPos.j);
                else
                    finalPos = Position(finalPos.i + 1, finalPos.j);
            }
            if(finalPos.i == s.ghosts[1].pos.i && finalPos.j == s.ghosts[1].pos.j) {
                finalPos = Position(s.pacman.pos.i, s.pacman.pos.j);
            }
            return finalPos;
        }
        if(s.pacman.dir == Direction::UP) {
            for(int k = 0; k < 4; k++) {
                posMov = false;
                pacmanPosMatrix = Position(finalPos.i - 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 4 && posMov == false) {
                    lastMov = 3;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j - 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 1 && posMov == false) {
                    lastMov = 2;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j + 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 2 && posMov == false) {
                    lastMov = 1;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i + 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 3 && posMov == false) {
                    lastMov = 4;
                    posMov = true;
                }
                if(lastMov == 1)
                    finalPos = Position(finalPos.i, finalPos.j + 1);
                else if(lastMov == 2)
                    finalPos = Position(finalPos.i, finalPos.j - 1);
                else if(lastMov == 3)
                    finalPos = Position(finalPos.i - 1, finalPos.j);
                else
                    finalPos = Position(finalPos.i + 1, finalPos.j);
            }
            if(finalPos.i == s.ghosts[1].pos.i && finalPos.j == s.ghosts[1].pos.j) {
                finalPos = Position(s.pacman.pos.i, s.pacman.pos.j);
            }
            return finalPos;
        }
        if(s.pacman.dir == Direction::DOWN) {
            for(int k = 0; k < 4; k++) {
                posMov = false;
                pacmanPosMatrix = Position(finalPos.i + 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 3 && posMov == false) {
                    lastMov = 4;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j + 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 2 && posMov == false) {
                    lastMov = 1;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j - 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 1 && posMov == false) {
                    lastMov = 2;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i - 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 4 && posMov == false) {
                    lastMov = 3;
                    posMov = true;
                }
                if(lastMov == 1)
                    finalPos = Position(finalPos.i, finalPos.j + 1);
                else if(lastMov == 2)
                    finalPos = Position(finalPos.i, finalPos.j - 1);
                else if(lastMov == 3)
                    finalPos = Position(finalPos.i - 1, finalPos.j);
                else
                    finalPos = Position(finalPos.i + 1, finalPos.j);
            }
            if(finalPos.i == s.ghosts[1].pos.i && finalPos.j == s.ghosts[1].pos.j) {
                finalPos = Position(s.pacman.pos.i, s.pacman.pos.j);
            }
            return finalPos;
        }
        if(s.pacman.dir == Direction::LEFT) {
            for(int k = 0; k < 4; k++) {
                posMov = false;
                pacmanPosMatrix = Position(finalPos.i, finalPos.j - 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 1 && posMov == false) {
                    lastMov = 2;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i + 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 3 && posMov == false) {
                    lastMov = 4;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i - 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 4 && posMov == false) {
                    lastMov = 3;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j + 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 2 && posMov == false) {
                    lastMov = 1;
                    posMov = true;
                }
                if(lastMov == 1)
                    finalPos = Position(finalPos.i, finalPos.j + 1);
                else if(lastMov == 2)
                    finalPos = Position(finalPos.i, finalPos.j - 1);
                else if(lastMov == 3)
                    finalPos = Position(finalPos.i - 1, finalPos.j);
                else
                    finalPos = Position(finalPos.i + 1, finalPos.j);
            }
            if(finalPos.i == s.ghosts[1].pos.i && finalPos.j == s.ghosts[1].pos.j) {
                finalPos = Position(s.pacman.pos.i, s.pacman.pos.j);
            }       
            return finalPos;
        }
        return finalPos;
    }
        

    inline Position inkyAction(const State& s) {
        Position nextPos;
        float min = 9999999;
        int minIndexj = 0;
        int minIndexi = 0;
        // ghosts[0] = blinky
        int pos1 = s.pacman.pos.i - (s.pacman.pos.i - s.ghosts[0].pos.i);
        int pos2 = s.pacman.pos.j - (s.pacman.pos.j - s.ghosts[0].pos.j);
        nextPos = Position(pos1, pos2);
        
        if(!(nextPos.i > 0 && nextPos.j > 0 && nextPos.i < 29 && nextPos.j < 27 && s.maze[nextPos.i][nextPos.j] != State::WALL && s.maze[nextPos.i][nextPos.j] != State::SPAWN_AREA)) {
            for (int i = 0; i < 29; i++) {
                for (int j = 0; j < 27; j++) {
                    if (s.maze[i][j] != State::WALL && s.maze[i][j] != State::SPAWN_AREA) {
                        if(sqrt(pow((nextPos.i - i), 2) + pow((nextPos.j - j), 2)) < min) {
                            min = sqrt(pow((nextPos.i - i), 2) + pow((nextPos.j - j), 2));
                            minIndexj = j;
                            minIndexi = i;
                        }
                    }
                }
            }
            nextPos = Position(minIndexi, minIndexj);
        }
        if(nextPos.i == s.ghosts[2].pos.i && nextPos.j == s.ghosts[2].pos.j) {
            nextPos = Position(s.pacman.pos.i, s.pacman.pos.j);
        }
        //cout << s.maze[nextPos.i][nextPos.j] << " AQUI " << s.maze[nextPos.i][nextPos.j] << endl;
        return nextPos;
    }

    inline bool clydeAction(const State& s) {
        int i, j;
        (s.pacman.pos.i > s.ghosts[3].pos.i) ? i = s.pacman.pos.i - s.ghosts[3].pos.i : i = s.ghosts[3].pos.i - s.pacman.pos.i;
        if(i <= 2)
            return true;
        (s.pacman.pos.j > s.ghosts[3].pos.j) ? j = s.pacman.pos.j - s.ghosts[3].pos.j : j = s.ghosts[3].pos.j - s.pacman.pos.j;
        if(j <= 2)
            return true;
        return false;
    }
};

#endif
