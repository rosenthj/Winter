Winter is a chess engine that relies on the UCI protocol to communicate with a GUI.

In order to run it on Linux, just compile it via "make" in the root directory and then run it from the root directory. Systems which do not have the newest generation of processors may lack access to the BMI2 instruction set, in this case compile with "make no_bmi" instead.

When running it the first time call "uci" to get the author and program names and then "perft_test" in order to confirm nothing is wrong with the move generation. Assuming this passes you can call "go infinite" in order to see what it thinks in the starting position and "stop" or "quit" to stop the search.
