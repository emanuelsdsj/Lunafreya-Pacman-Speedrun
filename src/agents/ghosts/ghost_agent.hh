#ifndef GHOST_AGENT_HH
#define GHOST_AGENT_HH

#include "../../state/direction.hh"
#include "../agent.hh"
#include "../../utils.hh"
#include "../../state/position.hh"
#include "../../pathfinding/pathfinding.hh"

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
        int randomValue = 0;
        if (ghost.pos != ghost.prev) {
            if (s.is_scared(ghost)) {
                return PathMagic::try_to_avoid(ghost.pos, PathMagic::from_to(ghost.pos, s.pacman.pos));
            }
            else {
                Direction d;
                randomValue = rand() % 100 + 1;
                switch(ghost.behaviour) {
                    case SCATTER:
                            if(randomValue > intelligenceGhost) {
                            d = PathMagic::from_to(ghost.pos, ghost.scatter_pos); break;
                        }
                        d = PathMagic::from_to(ghost.pos, s.pacman.pos); break;
                    case CHASE:
                        if(randomValue > intelligenceGhost) {
                            d = PathMagic::from_to(ghost.pos, s.pacman.prev); break;
                        }
                        d = PathMagic::from_to(ghost.pos, s.pacman.pos); break;
                    default:
                    ensure(false, "Invalid ghost behaviour enum");
                }

                //ensure(pr.found, "Path not found!");
                if (d == Direction::STAY) _debug("No movement for ghost #%d!", ghost_id);
                return d;
            }
        }
        else return ghost.dir;
    }
};

#endif
