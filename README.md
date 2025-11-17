# Little TSP Optimization

This is an implementation of the little Algorithm for the solving the Traveling Salesman Problem (TSP) in C.  
The initial solution is computed using the nearest neighbor algorithm, and we optimize it using the 2-opt algorithm.


## How to use

### Single thread version

Great for all instances of TSP.

#### Compilation
```bash
make -j2
```

#### Execution
```bash
./little -r <filename> -n <number_of_cities> [-v] [-i]
```
Where:
- filename is the name of the tsp data file. (e.g. [Berlin52](data/berlin52.tsp) data file). Mandatory.
- number_of_cities is at most the number of cities in the data file. Mandatory.
- -v is an optional flag to enable verbose mode (show all the intermediate results). (default is off). 
- -i is an optional flag to just compute the initial solution. (default is off).

#### Example
```bash
./little -r data/berlin52.tsp -n 40 -v
```
### Multi-thread version

Great for large instances of TSP. Or not...  
For now I don't know how to get a floor size to start multi-threading neither for the depth of the tree.  

A good way to see improvement: eil76 and 50 towns.  
More testing need to be done, and in main.c there is a comparison between seq and omp.

#### Compilation
```bash
make OMP=1 [N=n] -j2
```
Where:
- N=n is the number of threads to use. I recommand the number of CPUs on your system not mandatory

#### Execution
```bash
./little -r <filename> -n <number_of_cities> [-v] [-i] -t <num_threads>
```
As in the single thread version and the number of threads to use.

#### Example
```bash
./little -r data/eil76.tsp -n 76 -v
```

## Results

All Berlin52 and eil76 instances computes in approximately 3 hours.  
(Sequential)
Time (in seconds) of compute for n cities in different datasets.
| n    | berlin52 | eil76 | kroA100 | a280  |
| :--- | -------- | ----- | ------- | ----- |
| 6    | 0        | 0     | 0       | 0     |
| 10   | 0        | 0     | 0       | 0     |
| 15   | 0        | 0     | 0       | 0     |
| 20   | 1        | 0     | 0       | 1     |
| 25   | 3        | 0     | 1       | 1     |
| 30   | 553      | 0     | 4       | 3     |
| 35   | 2        | 2     | 19      | 79    |
| 40   | 2        | 1     | 144     | 19721 |
| 45   | 507      | 6     | 10267   | x     |
| 50   | 9431     | 35    | 71626   | x     |
| 52   | 10946    | 271   | x       | x     |
| 55   | /        | 443   | x       | x     |
| 60   | /        | 1508  | x       | x     |
| 70   | /        | 36580 | x       | x     |
| 76   | /        | 12493 | x       | x     |

### Comparison Sequential / Parallel

Time (in seconds) of compute for n cities in Berlin52 and eil76 for each of their sequential and parallel version.  
The parallel version is using openMP and 16 threads (the same number my CPU has).

| n    | berlin52_seq | berlin52_omp | speedup_berlin52 | eil76_seq | eil76_omp | speedup_eil76 |
| :--- | ------------ | ------------ | ---------------- | --------- | --------- | ------------- |
| 6    | 0,0          | 0,0          | 0,0              | 0,0       | 0,0       | 0,0           |
| 10   | 0,0          | 0,0          | 0,2              | 0,0       | 0,0       | 0,1           |
| 15   | 0,0          | 0,0          | 2,0              | 0,0       | 0,0       | 0,2           |
| 20   | 0,1          | 0,0          | 6,4              | 0,0       | 0,0       | 9,5           |
| 25   | 2,7          | 0,2          | 14,0             | 0,0       | 0,0       | 17,3          |
| 30   | 553,0        | 43,1         | 12,8             | 0,0       | 0,0       | 2,6           |
| 35   | 1,7          | 0,1          | 14,6             | 1,2       | 0,1       | 18,5          |
| 40   | 1,9          | 0,1          | 17,7             | 0,6       | 0,1       | 9,3           |
| 45   | 507,0        | 41,4         | 12,3             | 5,6       | 0,5       | 11,4          |
| 50   | 9431,0       | 496,4        | 19,0             | 32,2      | 3,2       | 10,2          |
| 52   | 10946,0      | 479,8        | 22,8             | 271,0     | 24,3      | 11,2          |
| 55   | /            | /            | /                | 443,0     | 61,0      | 7,3           |
| 60   | /            | /            | /                | 1508,0    | 181,9     | 8,3           |
| 70   | /            | /            | /                | 36580,0   | 3324,0    | 11,0          |
| 76   | /            | /            | /                | 12493,0   | 1668,4    | 7,5           |

Sometimes, adding 16 threads can improve performance up to x22.8 times. But average are x11 for berlin52 and x8 for eil76.