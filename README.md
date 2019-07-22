
# Winter
Winter is a UCI chess engine.

It is the top rated chess engine from Switzerland and has competed at top invite only computer chess events such as the Top Chess Engine Competition and the Chess.com Computer Championships. Its level of play is super human, but below the state of the art reached by the large, distributed and resource intensive open source projects Stockfish and Leela Chess Zero.

Winter has relied on many machine learning algorithms and techniques over the course of its development. This includes certain clustering methods not used in any other chess programs, such as Gaussian Mixture Models and Soft K-Means.

As of Winter 0.6.2, the evaluation function relies on a small neural network for more precise evaluations.

## Installation
In order to run it on Linux, just compile it via "make" in the root directory and then run it from the root directory. Systems which do not have the newest generation of processors may lack access to the BMI2 instruction set, in this case compile with "make no_bmi" instead.

Winter does not rely on any external libraries aside from the Standard Template Library. All algorithms have been implemented from scratch. As of Winter 0.6.2 I have started to build an external codebase for neural network training.

## Training Your Own Winter Flavor

At the moment training a neural network for use in Winter is only supported in a very limited way. I intend to release the script shortly which was used in order to train the initial 0.6.2 net.

In the following I describe the steps to get from a pgn game database to a network for Winter.

 1. Get and compile the latest [pgn-extract](https://www.cs.kent.ac.uk/people/staff/djb/pgn-extract/) by David J. Barnes.
2. Use pgn-extract on your .pgn file with the arguments `-Wuci` and `--notags`. This will create a file readable by Winter.
3. Run Winter from command line. Call `gen_eval_csv filename out_filename` where filename is the name of the file generated in 2. and out_filename is what Winter should call the generated file. This will create a .csv dataset file (described below) based on pseudo-quiescent positions from the input games.
4. Train a neural network on the dataset. It is recommended to try to train something simple for now. Keep in mind I would like to refrain from making Winter rely on any external libraries.
5. Integrate the network into Winter. In the future I will probably support loading external weight files, but for now you need to replace the appropriate entries in `src/net_weights.h`.
6. `make clean` and `make` (or `make no_bmi`)

The structure of the .csv dataset generated in 3. is as follows. The first column is a boolean value indicating wether the player to move won. The second column is a boolean value indicating whether the player to move scored at least a draw. The remaining collumns are features which are somewhat sparse. An overview of these features can be found in `src/net_evaluation.h`.