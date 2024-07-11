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

Great for large instances of TSP.
The first results better than single threaded are above 76 cities (e.g. eil76)

#### Compilation
```bash
make OMP=1 N=n -j2
```
Where:
- N=n is the number of threads to use, default is 2.

#### Execution
```bash
./little -r <filename> -n <number_of_cities> [-v] [-i]
```
As in the single thread version.

#### Example
```bash
./little -r data/eil76.tsp -n 76 -v
```

## Results

All Berlin52 and eil76 instances computes in less than 7 hours. (less with less cities).  
For KroA100 and a280 instances, above 40 cities, it starts to takes some times.