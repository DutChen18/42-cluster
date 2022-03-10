# 42-cluster

# usage

	make
	./cluster [bot 1] [bot 2]

# commands

	init %d %d %d %f %d
		%d = total number of colors [1]
		%d = chips per color
		%d = grid size
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
		%d = first chip from bag
		%d = second chip from bag

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
		%d = chip to drop

[1] this is 2 times the number of colors per player, so 4 in a standard game.  
[2] except for the first turn of bot 0, bots will receive their opponent's action before drawing chips.  
[3] this is the initial direction.  
[4] 0 is centered around the current direction of gravity, positive values are in the clockwise direction.  

# example

	> init 4 22 6 1.0 1
	< color 5
	> drop 0 1
	> chips 2 3
	< drop 1 2
	> drop 0 1
	> chips 3 3
	< rotate 1
