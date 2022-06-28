# Applications of the Graph Tukey Depth

## Usage of code (Instructions for Ubuntu):

As a prerequisite you need at least *Cmake 3.20* and *gcc 9.4.0*:

**Clone the Repository:**

> Clone this repository and navigate to *AppOfTukeyDepth*: ```git clone git@github.com:fseiffarth/AppOfTukeyDepth.git && cd AppOfTukeyDepth```

**Installation**

> Make bash script files executable: ```chmod u+x installation.sh && chmod u+x examples.sh``` <br>
> Install the code with: ```./installation.sh```

**Examples (MUTAG and Cuneiform and KarateClub, more data can be downloaded [here](https://chrsmrrs.github.io/datasets/docs/datasets/))**

> **Run all examples** 
>> ```./examples.sh```

> **Run single examples**
>> Tukey Depth:
>>> ```./TukeyDepth/build/TukeyDepth -i Graphs/ -o out/ -db MUTAG Cuneiform -t 24```
>>> ```./TukeyDepth/build/TukeyDepth -i Graphs/KarateClub.edges -o out/ -t 24```

>>Tukey Depth Approximation:
>>> ```./Approximation/build/ApproximateTukeyDepth -i Graphs/ -o out/ -db MUTAG Cuneiform -t 24```
>>> ```./Approximation/build/ApproximateTukeyDepth -i Graphs/KarateClub.edges -o out/ -t 24```

>> **Core Examples:**
>>> ```./Approximation/build/ComputeCore -i Graphs/KarateClub.edges -o out/ -t 24```
>>> ```./Approximation/build/ComputeCore -i Graphs/ -o out/ -db MUTAG Cuneiform -t 24```

>> **Evaluation**
>>> ```./Evaluation/build/EvaluateApproximation -i out/ -o out/ -db MUTAG Cuneiform KarateClub```

