#include <iostream>
#include <vector>
using namespace std;
using M = vector<vector<int>>;
M mem;

// Dynamic programming bottom-up implementation which fills up the memoization global table mem with the
// maximum sum of the elements in a wset non strictly lower than a bound b, specified as a parameter,
// that can be achieved using up to the i-th element of the wset vector specified as a parameter
// Complexity is O(n*b) with n being the number of elements of the wset and b the mentioned bound
// NOTE that wset does not have any padding so for referencing the i-th element referenced 1,...,n
// we use the indexing i-1 because we are referencing 0,...,n-1
void bounded_maximum_sum_wset(int n, int b, const vector<int> &wset)
{
    for (int i = 1; i <= n; i++)
    {
        for (int w = 1; w <= b; w++)
        {
            if (wset[i - 1] > w)
                mem[i][w] = mem[i - 1][w];
            else
                mem[i][w] = max(mem[i - 1][w], wset[i - 1] + mem[i - 1][w - wset[i - 1]]);
        }
    }
}

// Gets the values of the elements involved in the maxwsetsum stored in the global table mem
// Complexity is O(w+n)
vector<int> get_bmss_elements(int n, int b, const vector<int> wset)
{
    int i = n;
    int j = b;
    vector<int> values;
    while (i > 0 and j >= 0)
    {
        if (mem[i][j] != mem[i - 1][j]) //i-th element is in the mss (i-1 in wset indexing)
        {
            values.push_back(wset[i - 1]);
            j -= wset[i - 1];
        }
        i--;
    }
    return values;
}

// Scales and returns decimal weights in v stored with certain precision into ingteger values
vector<int> scale_weights(const vector<double> &v, double precision, int &b)
{
    vector<int> scaledweights(v.size());
    int multfactor = int(1 / precision);
    b *= multfactor;
    for (int i = 0; i <= v.size() - 1; i++)
    {
        scaledweights[i] = v[i] * multfactor;
    }
    return scaledweights;
}

// Erases specified values from a specified weightset
void erase_values(vector<int> &wset, const vector<int> &values)
{
    for (int value : values)
    {
        int i = 0;
        bool found = false;
        while (i < wset.size() and not found)
        {
            if (wset[i] == value)
            {
                wset.erase(wset.begin() + i);
                found = true;
            }
            i++;
        }
    }
}

// Gets a set of decimal weights v, a bound b and the weight's precision
// and obtains the minimum number of subsets with sum <= b needed so all the
// elemets in v are assigned once to a subsets and a possible distribution
void material_optimization(vector<double> &v, int b, double precision)
{
    double sum = 0;
    for (double x : v)
    {
        sum += x;
    }
    cout << "Sum of values = " << sum << endl;
    vector<int> wset = scale_weights(v, precision, b);
    M total_unscaled_values;
    int counter = 0;
    while (wset.size() > 0)
    {
        mem = M(wset.size() + 1, vector<int>(b + 1, 0));
        bounded_maximum_sum_wset(wset.size(), b, wset);
        cout << wset.size() << " elements left in the weightset, Subset counter = " << counter << ", Subsetvalue = " << mem[wset.size()][b] << endl;
        vector<int> values = get_bmss_elements(wset.size(), b, wset);
        total_unscaled_values.push_back(values);
        erase_values(wset, values);
        counter++;
    }
    cout << "Subsets needed = " << total_unscaled_values.size() << endl;
    //giving the output info
}
int main()
{
    int n;            //wset size
    int b;            //bound size
    double precision; //decimal precision for the weights
    cin >> n >> b >> precision;

    //define and read the wset of n elements
    vector<double> wset(n);
    for (double &x : wset)
        cin >> x;
    material_optimization(wset, b, precision);
}