/*
 * TODO: Implement Dijkstra's algorithm as function.
 * TODO: Make single thread, single process solution.
 * TODO: Implement MPI to have multiple processes calculate distances.
 * TODO: Evaluate the solution.
 */

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_VERTICES 100000
#define MAX_DEGREE   1000
#define MAX_WEIGHT   1000

#define min(a,b) \
    ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define DEBUG 0
#define PROGRESS 1

int parse_vertex_file(char *file_name, int *nbr_vertices)
{



    puts("Parsing file...");
    return 0;
}

void find_shortest_distance()
{
    puts("Finding shortest distance...");
}


int calculate_neighbor_values() 
{
    return 0;
}

int find_smallest_node(unsigned int* values, int nbr_vertices)
{
    int i;
    unsigned int lowest = INT_MAX;
    for(i = 0; i < nbr_vertices; ++i) {
        if(values[i] < lowest)
            lowest = values[i];
    }
    return lowest;
}

int allocate_memory() 
{

    return 0;
}

int main(int argc, char** argv)
{
    int i, j;
    int mpi_error;
    int start_vert, end_vert;
    char* graph_file_name = NULL;
    int nbr_processes, this_rank;
    int nbr_lines, nbr_degrees, nbr_vertices, nbr_edges;
    int nbr_visited, nbr_unvisited;
    int v1, v2, w;
    unsigned int best_value;

    bool finished;
    bool * visited;

    unsigned int * values, * nbr_neighbors;
    int ** edges;
    int *  edge_entries;
    int ** weights;
    int *  weight_entries;

    //puts("Starting...");

    //Init MPI
    mpi_error = MPI_Init(&argc, &argv);

    // Get the number of processes 
    MPI_Comm_size(MPI_COMM_WORLD, &nbr_processes);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &this_rank);


    // Parse command line arguments
    start_vert = atoi(argv[1]);
    end_vert   = atoi(argv[2]);
    graph_file_name = argv[3];

    // Allocate memory for larger arrays
    values        = (int*) malloc(sizeof(int) * MAX_VERTICES);
    visited       = (bool*) malloc(sizeof(bool) * MAX_VERTICES);
    nbr_neighbors = (int*) malloc(sizeof(int) * MAX_VERTICES);

    edges          = (int**) malloc(sizeof(int*) * MAX_VERTICES);
    weights        = (int**) malloc(sizeof(int*) * MAX_VERTICES);
    edge_entries   = (int*) malloc(sizeof(int) * MAX_VERTICES * MAX_DEGREE);
    weight_entries = (int*) malloc(sizeof(int) * MAX_VERTICES * MAX_DEGREE);

    for(i = 0, j = 0; i < MAX_VERTICES; ++i, j+=MAX_DEGREE) {
        edges[i] = edge_entries + j;
        weights[i] = weight_entries + j;
    }


    if(this_rank == 0) {
        puts("I am your master!");
        printf("Starting from vertex %d, going to vertex %d\n", start_vert, end_vert); 


        memset(nbr_neighbors, 0, MAX_VERTICES);



        // Read data from file
        FILE * graph_file = fopen(graph_file_name, "r");
        if (graph_file == NULL) {
            printf("ERROR: Cannot open file '%s'!\n", graph_file_name);
            return -1;
        }

        nbr_lines = 0;
        nbr_vertices = 0;
        while(fscanf(graph_file, "%d %d %d\n", &v1, &v2, &w) != EOF) {
            //printf("Read this line from file: %d %d %d\n", v1, v2, w);
            weights[v1][v2] = w;
            //printf("Weight %d -> %d: %d\n",v1,v2,weights[v1][v2]);
            edges[v1][nbr_neighbors[v1]] = v2;
            nbr_neighbors[v1] += 1;
            if(v1 > nbr_vertices) {
                nbr_vertices = v1;
            }
            ++nbr_lines;
        }
        nbr_vertices += 1;
        nbr_edges = nbr_lines/2;
        //printf("There are %d lines in the input file corresponding to %d vertices.\n", nbr_lines, nbr_vertices, nbr_edges);

        // Set values for our arrays
        memset(visited, false, MAX_VERTICES);
        memset(values, INT_MAX, MAX_VERTICES);

        // Run Dijkstra's
        values[start_vert] = 0;

        nbr_visited = 0;
        finished = false;
        while(!finished) {
            int neighbor;
            unsigned int neighbor_w;
            int best;
            int best_vertex;

            // Find the lowest value vertix that we have not yet ruled out and start from here
            best = v2;
            best_value = INT_MAX;
            for(i = 0; i < nbr_vertices; ++i) {
                //printf("Vertex %d's value is %u\n", i ,values[i]);
                if ((!visited[i]) && (values[i] < best_value)) {
                    best = i;
                    best_value = values[i];
                }
            }


            //printf("Moved to %d, ", best);
            //printf("vertex %d has %d neighbors\n", best, nbr_neighbors[best]);
            // Visit each neighbor and calculate the value for that neighbor
            for(i = 0; i < nbr_neighbors[best]; ++i) {
                neighbor = edges[best][i];
                neighbor_w = weights[best][neighbor] + best_value;
                // If this new path is shorter, assign this value
                if (values[neighbor] > neighbor_w)
                    values[neighbor] = neighbor_w; 
            }

            visited[best] = true;
            ++nbr_visited;
            //printf("have visited %d\n", nbr_visited);
            if (values[end_vert] <= MAX_DEGREE) {
                best_vertex = find_smallest_node(values, nbr_vertices);
                if(values[end_vert] < best_value) {
                    best_value = values[end_vert];
                    finished = true;
                }
            }
        }

        printf("Shortest path is %d!\n", best_value);

    }






    // Finish up the MPI run
    MPI_Finalize();


    puts("Finished!");

    return 0;
}

