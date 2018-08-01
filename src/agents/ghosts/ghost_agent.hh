#ifndef GHOST_AGENT_HH
#define GHOST_AGENT_HH

#include "../../state/direction.hh"
#include "../agent.hh"
#include "../../utils.hh"
#include "../../state/position.hh"
#include "../../pathfinding/pathfinding.hh"

class Ghost_Agent: public Agent {

public:
    
     // intelligence for each ghost
    //lunafreya
    int intelligenceGhost;
    virtual ~Ghost_Agent() {}
    Ghost_Agent(int intelligence) {
        intelligenceGhost = intelligence;
    }
    //static const uint16_t INTELLIGENCE_BLINKY     = 90;    // intelligence for each ghost
    //static const uint16_t INTELLIGENCE_PINKY      = 60;
    //static const uint16_t INTELLIGENCE_INKY       = 30;
    //static const uint16_t INTELLIGENCE_CLYDE      = 0;

    inline Direction take_action(const State& s, uint ghost_id) {
        const Ghost_State& ghost = s.ghosts[ghost_id];
        int nextMov = 1;
        if (ghost.pos != ghost.prev) {
            if (s.is_scared(ghost)) {
                return PathMagic::try_to_avoid(ghost.pos, PathMagic::from_to(ghost.pos, s.pacman.pos));
            }
            else {
                Direction d;
                int randomValue = (rand() % 100 + 1);
                if(randomValue > intelligenceGhost) {
                    nextMov = 2;
                }
                switch(nextMov) {
                    case 1:
                        d = PathMagic::from_to(ghost.pos, ghost.scatter_pos); break;
                    case 2:
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
