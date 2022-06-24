# CCApprox

**Results of Experiments:**

-see *out/ICDE_2022/*

**Usage of Code:**

1. Clone this repository and navigate into *CCApprox* folder
2. Create folders *GraphData* and *ExternalLibraries* in the parent folder of *CCApprox*: ```mkdir ../GraphData && mkdir ../ExternalLibraries```
3. Download the SNAP 6.0 library from http://snap.stanford.edu/snap/download.html and unpack in *ExternalLibraries* folder
4. Download the graphs from http://snap.stanford.edu/data/index.html and save unpacked *.txt* in *GraphData* folder or sub-folders
5. Create build inside *CCAprox* folder and compile:
   ```mkdir build && cd build && cmake .. && make -o2 -j 12```
6. Convert the graphs: ```./ConvertGraphs```
7. Compile and run the executables:
   1. ```./ExpSamplingRuntime``` for sampling runtime experiment
   2. ```./ExpSamplingQuality``` for sampling quality experiment
   3. ```./ExpClosureRuntime``` for closure runtime experiment
   4. ```./ExpExactCore``` for exact core computation
    
       | Optional Arguments | ```-i```  | ```-t```  | ```--generators``` | ```--generator_seeds``` | ```--core_iterations``` | ```--max_nodes``` | ```--max_edges``` |
       | :---:   | :-: | :-: | :------------: | :-----------------: | :------------------: | :------------: | :------------: |
       | Seconds | input path | thread num | generator number | generator seed | iterations of the core | max graph size | max graph edges |
   5. ```./ExpApproxCore``` for approximate core computation
   
       | Optional Arguments | ```-i```  | ```-o```  | ```-t```  | ```--generators``` | ```--generators_end```| ```--generators_step``` | ```--generator_seeds``` | ```--threshold``` | ```--threshold_end``` | ```--threshold_step``` | ```--core_iterations```  | ```--samples``` | ```--sample_seed```  | ```--max_nodes``` | ```--max_edges``` |
       | :---:   | :-: | :-: | :-: | :------------: | :-----------------: | :------------------: | :------------------: | :------------------: | :------------------: | :------------------: | :------------------: | :------------: | :------------: | :------------: | :------------: |
       | Seconds | input path | output path | thread num | generator first size | generator last size | generator step size | generator seed | threshold smallest size | threshold largest size | threshold step | iterations of the core | number of samples | sample seed | max graph size | max graph edges |


*Testing*
```-i ../../GraphData/RealWorld/ --generators 10 --threshold 0.01 0.5 0.6 0.7 0.8 0.9 --threads 16 --save_outerplanar_approx --save_load_samples --no_tree```