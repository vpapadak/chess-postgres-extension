From the professor link four PGN files have been downloaded. They contain the PGN games of Gligoric, Hort, korochnoi, and Shirov.

The Python "csv_creator" files take those four files as input and create four different .csv tables with the following structure:
- "san" containing the SAN notation of the game;
- "player" containing the player of the game (between the aforementioned four);
- "site" containing the site where the game was hosted.
The biggest of those file contains all the games in the four .pgn files (total: 15.448 games). The others contain a downsampling of it: 20%, 5%, 1%.

The performance of the extension created is benchmarked through four queries (one query for each function) at four different scale factors.

To sum up:
Gligoric, Hort Horchnoi, Shirov PGNs
--- through "csv_creator" --->
csv_games_1, _5, _20, _100
--- through "db_creation_and_load" -->
populated games database

queries.sql + DB
--- benchmarker.ipynb -->
result times