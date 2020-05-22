# TreeWasserstein
A python module for fast calculation of the wasserstein distance on tree metrics implemented by C++. This package is based on `pybind11`.

## Usage

### Install
```
pip install ../Tree-Wasserstein
```

### Test
``` 
pipenv run make test
```

### Example
``` 
import tw
first_prob = [0, 0, 1]
second_prob = [1 / 3, 1 / 3, 1 / 3]
edges = [(0, 1, 1), (0, 2, 1)]
tw.distance(first_prob, second_prob, edges) # 1.0
 ```

### Performance
See [here](examples/example.ipynb)


## Reference
- Evans, Steven N., and Frederick A. Matsen. “The Phylogenetic Kantorovich-Rubinstein Metric for Environmental Sequence Samples.” Journal of the Royal Statistical Society: Series B (Statistical Methodology) 74.3 (2012): 569–592. Crossref. Web.
- Le, Tam, et al. "Tree-sliced variants of wasserstein distances." Advances in neural information processing systems. 2019.
- Flamary, R., and N. Courty. "POT: Python Optimal Transport." (2017).
- Aric A. Hagberg, Daniel A. Schult and Pieter J. Swart, “Exploring network structure, dynamics, and function using NetworkX”, in Proceedings of the 7th Python in Science Conference (SciPy2008), Gäel Varoquaux, Travis Vaught, and Jarrod Millman (Eds), (Pasadena, CA USA), pp. 11–15, Aug 2008
- https://github.com/pybind/pybind11
- https://github.com/pybind/python_example
