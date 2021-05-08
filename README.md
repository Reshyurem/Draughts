# DSA-Mini-Project
A project by group 38, with the topic of '8x8 checkers' for DSA mini project submission

# Prerequisites
## Minimum computer specifications
Graphics - RTX 3090 <br />
RAM - 32 GB<br />
Processor - AMD Ryzen 9 5900X

## Installing SDL library
To install in Ubuntu/Linux, open Terminal and run the following commands:
```
sudo apt-get update
sudo apt-get install libsdl2-2.0
sudo apt-get install libsdl-image1.2-dev
```
# Running the game
To run the game, you need to run the `Makefile` that is included in the repository.<br />
Open the folder containing the `Makefile` and the source code functions in the Terminal, and type the following command in:
```
make
```
This will create an executable named `game` which can then be run.

# Rules of the game
To refer to the rules of the game, refer to the PDF in the link below:
https://ctycms.com/mn-rochester/docs/checkers-instructions.pdf

If you're too lazy to open the link, the rules are given below:
- The opponent with the darker pieces moves first.

- Pieces may only move one diagonal space forward (towards their opponents pieces) in the
  beginning of the game.

- Pieces must stay on the dark squares.

- To capture an opposing piece,"jump" over it by moving two diagonal spaces in the direction of
  the the opposing piece.

- A piece may jump forward over an opponent's pieces in multiple parts of the board to capture
  them.

- Keep in mind, the space on the other side of your opponent’s piece must be empty for you to
  capture it.

- If your piece reaches the last row on your opponent's side, you may re-take one of your captured
  pieces and "crown"the piece that made it to the Kings Row. Thereby making it a "King Piece."

- King pieces may still only move one space at a time during a non-capturing move. However,
  when capturing an opponent's piece(s) it may move diagonally forward or backwards.

- There is no limit to how many king pieces a player may have.

## The game is won when all of your opponent's pieces are captured !!
