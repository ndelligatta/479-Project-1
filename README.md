Made by Nolan Delligatta
(no group)
893367896
ndelligatta@csu.fullerton.edu
This project has N processes generates random  5 digit unique NNDRR ID's. Then a ring sequence occurs for the processes
to find the two highest numbers out of the processes' random ID's. Another ring occurs to find out which process 
had the second highest ID, and once found, that process becomes the leader rank, and broadcasts to the Comm. World
the leader's rank.

Run 
mpic++ project1.cpp && mpirun -n N a.out

where N is a number between 6 and 20 to dictate the desired number of processes for this program.
