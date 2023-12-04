# Player 1

I did a similar beam search type search. I think the heuristic used is very important.
I used a bunch of thing for the heuristic, but I'm sure I missed some important parts because I couldn't reach 107+.

Some of them:
- I used the number of mirrors left on the board
- Number of empty rows and columns (this obvious one made a big improvement, which I didn't find obvious at first)
- Trying to keep the number of mirrors in each row and column even. (this was after I realized odd N scored much worse)
- Trying to clear the center of the board first

With my local tests I scored an average of 1.16 when N is even and 0.92 when N is odd.
The "slow" TC computers was a problem, so I guess the top 6 spend a lot of time optimizing their code
(Just look at the massive amount of example tests)

# Player 2 (psyho)

I used beam search, going through each of the levels one by one.

Scoring Function:
Scoring function for each state :
  number of mirrors destroyed
   + (clear rows/columns * W0
   + even rows/columns * WP
   + rows/columns with 1 left mirror * W1
   + rows/columns with 2 mirrors * W2
   + rows/columns with 4 mirrors * W4) / N / 16.

W0 = 8, W1 = 1, W2 = 2, W4 = 1, WP = 3 for even N, 7 for odd N.

W0 is the single most important feature and it gave initially over 10 points. Changing WP from WP=4 to WP=3/7 gave me around 0.4. Adding W1,W2 and W4 gave a total of around 1 point.

## Simulation:
For each cell I remember the closest mirror in each direction (or edge in case there's no mirror).
For checking every possible child state (shooting rays from all direction),
I use a simulation that doesn't update that "closest mirror" table.

Hashing is used for removing duplicate states. For some reason, I couldn't create a hashing function
that would work faster than a huge precomputed table (one entry per position).

Row/column count is updated during the simulation while the score of row/column stuff is done with SSE2 intrinsics.
The nice optimization I did was to sort the states in beam search by a lexicographical order,
so that I don't have simulate all of the states from the beginning.

But in general I guess that my simulation was rather slow - while it performed good in general scenario,
it was certainly better to use different simulation methods depending on the depth of the search.

## Time Management:
I had a huge problem with using 100% of given time and since each 5% of speed improvement was a huge deal
(around 0.1) it was a huge issue. I tried to estimate time based on the amount of mirrors that have been already destroyed
(could've added current level as well, but didn't want to add NNs to my solution). Locally I generated 10K tests
(200 for each size) and saved the stats for average estimated time at some crucial points and simply used them for estimation.
And if I'm going under/over the time limit, I adjust the width of the beam search on the fly. Locally it worked incredibly well,
but on TC machine I had completely different timings that didn't scaled linearly. The reason was that during the end the bottleneck
was the memory, and my local memory speed/CPU speed ratio was much better than on TC side.
So during the very deep levels of the beam search, the time was severely underestimated and I often had to drastically reduce the width
of the beam search at the end. I made some quickfixes to combat this, but this is probably the reason why my local scores and provisional 
ones didn't match very well.

Another problem with time management for beam search is that it pays of to increase the width at the end.
I have added some stupid heuristics to achieve it, but it's far from good.

My local scores were around 108.35-108.45 (based on first 1K seeds) using the similar amount of time that I use on the server.
Unfortunately due to messed up "time management" I have no idea what my real score is.
Without any time management (fixed size of the beam) I was getting 108.15-108.20. I remember that there are 49 odd and 51 even tests in the provisional :)

One random note is that I have never had so short code in any MM. After removing all of the unused, template and debugging code,
it would take around 5K, while normally I can hardly squeeze under 20-30K.

And as a sidenote, I'm pretty sure I have missed some crucial observations. It took me a lot of time to notice the differences between 
the odd/even tests. Then suddenly a lot of things made much more sense.

The following days will be very stressful for me :) I'm expecting a rather significant shuffling in ranks (+/- 1 point might be possible).
