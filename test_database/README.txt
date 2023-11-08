From the professor link four PGN files have been downloaded. They contain the PGN games of Gligoric, Hort, korochnoi, and Shirov.

The Python "csv_creator" files take those four files as input and create four different .csv tables with the following structure:
- "san" containing the SAN notation of the game;
- "player" containing the player of the game (between the aforementioned four);
- "site" containing the site where the game was hosted.
The biggest of those file contains all the games in the four .pgn files (total: ---). The others contain a downsampling of it: 20%, 5%, 1%.

TO COMPLETE