# 42-cluster

# description

This is a game of *n* in a row on a hexagonal grid. The goal of the game is to make *n* in a row with the same color. The amount of colors is variable. You can also change the gravity. This all results in a very dynamic game with a lot of possibilties. Can you make the best bot? Down below you can find a guide of commands.

ruleset from The Deckhands:

	There are 4 distinct colors, 2 for each player.
	The amount of tokens is limited. Of each color there is 1/4th of the amount of tiles in the game board rounded down to the first multiple of 4. The tokens will be in a bag that the player can draw from.
	The base game board has an edge of 5 hexagons. The board is always balanced on one corner, it can never stand on a side.
	You need to get a line of 4 tokens in a row to win.
	If you run out of tokens, you loose.

	Players take turns. The starting player is picked randomly.
	On each turn you draw 2 tokens from your bag. 
	You have 2 actions:
	1. play 1 of the 2 tokens, the other token goes back in the bag to be used for later. The token you picked is put in the ""top"" of the board, where it will fall ""down"" till it reaches the bottom or another token.
	2. you discard both tokens and rotate the game board to a different corner. The two discarded tokens will be out of the game. After rotation all tokens will fall at the same time in the new direction of gravity.

	If, for example, after a rotation, both players form a line of 4 tokens at the same time, the game continues until one player has a longer line of minimum 4 tokens.

# usage

	make
	vim config.txt # change config settings
	./cluster # player versus player
	./cluster [bot 1] # player versus bot
	./cluster [bot 1] [bot 2] # bot versus bot

# commands

	init %d %d %d %d %f %d
		%d = total number of colors [1] [6]
		%d = chips per color
		%d = grid size
		%d = minimum chips in a row required to win
		%f = timeout in seconds
		%d = player id [2]

	color %d
		%d = preferred color id
			0 = red
			1 = yellow
			3 = cyan
			4 = blue
			5 = magenta
	
	chips %d %d
		%d = first chip from bag [6]
		%d = second chip from bag [6]

	rotate %d
		%d = direction of gravity
			0 = to top
			1 = to top right
			2 = to bottom right
			3 = to bottom [3]
			4 = to bottom left
			5 = to top left

	drop %d %d
		%d = position to drop at [4]
		%d = chip to drop [6]

	fetch %s
		%s = type of information to obtain
			cells = get list of all cells, using "cell_count" and "cell" commands
			gravity = get current gravity state, using "gravity" command
			walls = get list of walls, using "wall_count" and "wall" commands
			chips = get list of chips, using "cell_count" and "cell" commands

	cell_count %d
		%d = number of "cell" commands that follow this command

	cell %d %d %d %d
		%d = q coordinate [5]
		%d = r coordinate [5]
		%d = s coordinate [5]
		%d = current chip value at this location [6]
	
	gravity %d
		%d = current direction of gravity, same format as "rotate"
	
	wall_count %d
		%d = number of "wall" commands that follow this command

	wall %d %d %d
		%d = q coordinate [5]
		%d = r coordinate [5]
		%d = s coordinate [5]

[1] this is 2 times the number of colors per player, so 4 in a standard game.  
[2] except for the first turn of bot 0, bots will receive their opponent's action before drawing chips.  
[3] this is the initial direction.  
[4] 0 is centered around the current direction of gravity, positive values are in the clockwise direction.  
[5] https://www.redblobgames.com/grids/hexagons/#coordinates-cube  
[6] bot 0 will always play with the first half of the color ids, bot 1 will always play with the second half.  

# example

	> init 4 22 6 1.0 1
	< color 5
	> drop 0 1
	> chips 2 3
	< drop 1 2
	> drop 0 1
	> chips 3 3
	< rotate 1

