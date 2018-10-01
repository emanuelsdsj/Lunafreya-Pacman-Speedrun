## Lunafreya Pacman Speedrun

## Changes
    - Added timer
    - Added scoring
    - Added combos for killing ghosts
    - Changed Win and Lose parameters for training 
    - Scatter and chase mode cycle: 7 seconds Scatter then 20 seconds chase. Repeat.
    - Clone of the original game the time of powerpill when activated.
    - Added the ghosts personalities.
    - Ghosts - Scatter Mode:
        - Random valid position. // Same of albertsgrc. no need of changes.
    - Ghosts - Chase Mode:
        - Blinky: Only chase pacman // Same of albertsgrc. no need of changes.
        - Pinky: 4 positions in front of pacman.
        - Inky: if blinky on chase > (blinky distance of pacman) * 2 in front of pacman, limit of 8 pos else run from pacman.
        - Clyde: Chase pacman when close 4 positions run from pacman
    - Added in train Max Points and rounds number.

- Deep q-learning
    - To be continued...

## Modified Version of

- https://github.com/albertsgrc/q-mspacman.git
