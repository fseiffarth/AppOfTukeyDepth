# Applications of the Graph Tukey Depth

## Usage of code (Instructions for Ubuntu):

As a prerequisite you need at least *Cmake 3.20* and *gcc 9.4.0*:

**Clone the Repository:**

> Clone this repository and navigate to *AppOfTukeyDepth*: ```git clone git@github.com:fseiffarth/AppOfTukeyDepth.git && cd AppOfTukeyDepth```

**Installation** (*compiling of or-tools need some minutes!*)

> Make bash script files executable: ```chmod u+x installation.sh && chmod u+x examples.sh``` <br>
> Install the code with: ```./installation.sh```

**Examples (MUTAG, Cuneiform and KarateClub, more data can be downloaded [here](https://chrsmrrs.github.io/datasets/docs/datasets/))**

> **Run all examples**  <br>
> ```./examples.sh```

> **Run single examples** <br>
>> **Tukey Depth:** <br>
>> ```./TukeyDepth/build/TukeyDepth -i Graphs/ -o out/ -db MUTAG Cuneiform -t 24``` <br>
>> ```./TukeyDepth/build/TukeyDepth -i Graphs/KarateClub.edges -o out/ -t 24``` <br>
>>**Tukey Depth Approximation:** <br>
>> ```./Approximation/build/ApproximateTukeyDepth -i Graphs/ -o out/ -db MUTAG Cuneiform -t 24``` <br>
>> ```./Approximation/build/ApproximateTukeyDepth -i Graphs/KarateClub.edges -o out/ -t 24``` <br>
>> **Core Examples:** <br>
>> ```./Approximation/build/ComputeCore -i Graphs/KarateClub.edges -o out/ -t 24``` <br>
>> ```./Approximation/build/ComputeCore -i Graphs/ -o out/ -db MUTAG Cuneiform -t 24``` <br>
>> **Evaluation** <br>
>> ```./Evaluation/build/EvaluateApproximation -i out/ -o out/ -db MUTAG Cuneiform KarateClub```

