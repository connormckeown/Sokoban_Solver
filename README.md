# Sokoban Solver 

## Overview

A solver written in C++ for the popular japanese puzzle game [Sokoban](https://en.wikipedia.org/wiki/Sokoban). The solver always produces an optimal solution (lowest number of moves) as it uses IDA* search.

## Input Format

A valid map follows the following the format.

| Level element  | Character |
| ------------- |:-------------:|
| Wall	     | #     |
| Player      | @     |
| Player on goal square	      | +     |
| Box	     | 	$     |
| Box on goal square		     | *     |
| Goal square		     | .     |
| Wall	     | #     |
| Floor		     | (Space)     |

## Output

The solution of a level is represented by the moves of a player. The letters u, d, l, r, correspond to the moves up, down, left, right. The letters are capitalized when the move also involves a box push.

## How to Run
The program builds with a Makefile so simply type ```make``` to get the executable. Then, to run the program type . . .

```
$ ./solver path/to/map.txt
```

## Example

Input microban/6.txt

```
   ####
####  #
#@.*  #
#  #  ###
####  $ #
   #  # #
   ##   #
    #####
```

Output
```
Solving microban2/6.txt
Solution found... 958 nodes explored
Time: 2.64457 seconds
Solution: rRRdrdddrruuLLuluurDDDrrddllUlUUruLLrddRddrruuLLdlUUruL

```
