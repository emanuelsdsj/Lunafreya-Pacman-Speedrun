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
                //if (d == Direction::STAY) _debug("No movement for ghost #%d!", ghost_id);
                return d;
            }
        }
        else return ghost.dir;
    }

    inline Position pinkyAction(const State& s) {
        int lookAhead = 2;
        int secondlookAhead = 0;
        Position pacmanPosMatrix;
        //Up
        if(s.pacman.dir == Direction::UP) {
            pacmanPosMatrix = Position(s.pacman.pos.i, s.pacman.pos.j + lookAhead);
            while(!(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA)) {
                //Up and Right Second
                if(lookAhead == 0) {
                    pacmanPosMatrix = Position(s.pacman.pos.i + secondlookAhead, s.pacman.pos.j + lookAhead);
                    while(!(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA)) {
                        lookAhead += 1;
                        secondlookAhead -= 1;
                        pacmanPosMatrix = Position(s.pacman.pos.i + secondlookAhead, s.pacman.pos.j + lookAhead);
                    }
                    return pacmanPosMatrix;
                }
                //Up and Left First
                secondlookAhead += 1;
                lookAhead -= 1;
                pacmanPosMatrix = Position(s.pacman.pos.i - secondlookAhead, s.pacman.pos.j + lookAhead);
            }
            return pacmanPosMatrix;
        }
        //Left
        if(s.pacman.dir == Direction::LEFT) {
            pacmanPosMatrix = Position(s.pacman.pos.i - lookAhead, s.pacman.pos.j);
            while(!(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA)) {
                //Left and Up Second
                if(lookAhead == 0) {
                    pacmanPosMatrix = Position(s.pacman.pos.i - lookAhead, s.pacman.pos.j + secondlookAhead);
                    while(!(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA)) {
                        lookAhead += 1;
                        secondlookAhead -= 1;
                        pacmanPosMatrix = Position(s.pacman.pos.i - lookAhead, s.pacman.pos.j + secondlookAhead);
                    }
                    return pacmanPosMatrix;
                }
                //Left and Down First
                secondlookAhead += 1;
                lookAhead -= 1;

                pacmanPosMatrix = Position(s.pacman.pos.i - lookAhead, s.pacman.pos.j - secondlookAhead);
            }
            return pacmanPosMatrix;
        }
        //Down
        if(s.pacman.dir == Direction::DOWN) {
            pacmanPosMatrix = Position(s.pacman.pos.i, s.pacman.pos.j - lookAhead);
            while(!(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA)) {
                //Down and Left Second
                if(lookAhead == 0) {
                    pacmanPosMatrix = Position(s.pacman.pos.i - secondlookAhead, s.pacman.pos.j - lookAhead);
                    while(!(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA)) {
                        lookAhead += 1;
                        secondlookAhead -= 1;
                        pacmanPosMatrix = Position(s.pacman.pos.i - secondlookAhead, s.pacman.pos.j - lookAhead);
                    }
                    return pacmanPosMatrix;
                }
                //Down and Right First
                secondlookAhead += 1;
                lookAhead -= 1;

                pacmanPosMatrix = Position(s.pacman.pos.i + secondlookAhead, s.pacman.pos.j - lookAhead);
            }
            return pacmanPosMatrix;
        }
        //Right
        if(s.pacman.dir == Direction::RIGHT) {
            pacmanPosMatrix = Position(s.pacman.pos.i + lookAhead, s.pacman.pos.j);
            while(!(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA)) {
                //Right and Down Second
                if(lookAhead == 0) {
                    pacmanPosMatrix = Position(s.pacman.pos.i + lookAhead, s.pacman.pos.j - secondlookAhead);
                    while(!(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA)) {
                        lookAhead += 1;
                        secondlookAhead -= 1;
                        pacmanPosMatrix = Position(s.pacman.pos.i + lookAhead, s.pacman.pos.j - secondlookAhead);
                    }
                    return pacmanPosMatrix;
                }
                //Right and Up First
                secondlookAhead += 1;
                lookAhead -= 1;

                pacmanPosMatrix = Position(s.pacman.pos.i + lookAhead, s.pacman.pos.j + secondlookAhead);
            }
            return pacmanPosMatrix;
        }
        if(s.pacman.dir == Direction::STAY) {
            return s.pacman.pos;
        }
        return s.pacman.pos;
    }

     inline Position inkyAction(const State& s) {
        Position nextPos;
        float min = 1000;
        int minIndexj = 0;
        int minIndexi = 0;
        // ghosts[0] = blinky
        nextPos = Position(s.pacman.pos.i - (s.pacman.pos.i - s.ghosts[0].pos.i), s.pacman.pos.j - (s.pacman.pos.j - s.ghosts[0].pos.j));

        if(!(nextPos.i > 0 && nextPos.j > 0 && nextPos.i < 29 && nextPos.j < 27 && s.maze[nextPos.i][nextPos.j] != State::WALL && s.maze[nextPos.i][nextPos.j] != State::SPAWN_AREA)) {
            for (int i = 0; i< 28; i++) {//for each tile
                for (int j = 0; j< 31; j++) {
                    if (s.maze[nextPos.i][nextPos.j] != State::WALL && s.maze[nextPos.i][nextPos.j] != State::SPAWN_AREA) {
                        if(sqrt(pow((nextPos.i - i), 2) + pow((nextPos.j + j), 2)) < min) {
                            min = sqrt(pow((nextPos.i - i), 2) + pow((nextPos.j + j), 2));
                            minIndexj = j;
                            minIndexi = i;
                        }
                    }
                }
            }
            nextPos = Position(minIndexi, minIndexj);
        }
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
