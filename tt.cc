#include <iostream>
#include <vector>
using namespace std;
using M = vector<vector<int>>;
M mem;

// Dynamic programming bottom-up implementation which fills up the memoization global table mem with the
// maximum sum of the elements in a subset non strictly lower than a bound b, specified as a parameter,
// that can be achieved using up to the i-th element of the subset vector specified as a parameter
// Complexity is O(n*b) with n being the number of elements of the subset and b the mentioned bound
// NOTE that subset does not have any padding so for referencing the i-th element referenced 1,...,n
// we use the indexing i-1 because we are referencing 0,...,n-1
double bounded_maximum_sum_subset(int n, int b, const vector<int> &subset)
{
    for (int i = 1; i <= n; i++)
    {
        for (int w = 1; w <= b; w++)
        {
            if (subset[i - 1] > w)
                mem[i][w] = mem[i - 1][w];
            else
                mem[i][w] = max(mem[i - 1][w], subset[i - 1] + mem[i - 1][w - subset[i - 1]]);
        }
    }
    return mem[n][b];
}

// Gets the values of the elements involved in the maxsubsetsum stored in the global table mem
// Complexity is O(w+n)
vector<int> get_bmss_elements(int n, int b, const vector<int> subset)
{
    int i = n;
    int j = b;
    vector<int> values;
    while (i >= 0 and j > 0)
    {
        if (mem[i][j] != mem[i - 1][j]) //i-th element is in the mss (i-1 in subset indexing)
        {
            values.push_back(subset[i - 1]);
            j -= subset[i - 1];
        }
        i--;
    }
    return values;
}
int main()
{
    int n; //subset size
    int b; //bound size
    cin >> n >> b;
    mem = M(n + 1, vector<int>(b + 1, 0)); //has padding for i,j=0;

    //define and read the subset of n elements
    vector<int> subset(n);
    for (int &x : subset)
        cin >> x;
    int solucio = bounded_maximum_sum_subset(n, b, subset);
    vector<int> values = get_bmss_elements(n, b, subset);
}