#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
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
        if (mem[i][j] != mem[i - 1][j]) // i-th element is in the mss (i-1 in wset indexing)
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

// checks validity of a solution, for testing and debugging purposes
// return values: 0 = all good, 1 = cond 1 not; 2 = cond 1 valid, cond 2 not; 3 = only cond 3 not
// we check all multiplying by precision, as in the way our final output will come
int checker(const vector<int> &wset, const M &total_unscaled_values, double precision, int b)
{
    // c1: check that all elements in the wset are represented
    // construct the checker matrix
    vector<vector<bool>> checker_mat;
    for (int i = 0; i < total_unscaled_values.size(); i++)
    {
        vector<bool> insert(total_unscaled_values[i].size(), false);
        checker_mat.push_back(insert);
    }
    // iterate
    for (int i = 0; i < wset.size(); i++)
    {
        bool found = false;
        for (int j = 0; found == false && j < total_unscaled_values.size(); j++)
        {
            for (int k = 0; found == false && k < total_unscaled_values[j].size(); k++)
            {
                if (not checker_mat[j][k] and wset[i] * precision == total_unscaled_values[j][k] * precision)
                {
                    found = true;
                    checker_mat[j][k] = true;
                }
            }
        }
        if (not found)
            return 1;
    }

    double b_adjust = b * precision;
    double epsilon = 0.000001;
    // c3: check that none of the subsets proposed as a solution is greater than b
    for (int i = 0; i < total_unscaled_values.size(); i++)
    {
        double sumrow = 0;
        for (int j = 0; j < total_unscaled_values[i].size(); j++)
        {
            if (checker_mat[i][j] == false)
                return 2;
            sumrow = sumrow + total_unscaled_values[i][j] * precision;
            if (sumrow > b_adjust + epsilon)
            {
                return 3;
            }
        }
    }

    return 0;
}

void write_results(const M &total_unscaled_values, double precision, int b, const vector<double> &v)
{
    ofstream outputfile("output.txt");
    outputfile << "Per als " << v.size() << " pals especificats, corresponents a les mides: ";
    for (int i = 0; i < v.size(); i++)
    {
        outputfile << " [" << v[i] << "] ";
    }
    outputfile << endl;
    outputfile << endl;
    outputfile << "En total seran necessàries " << total_unscaled_values.size() << " barres grans" << endl;
    outputfile << endl;
    double sobrant = 0;

    for (int i = 0; i < total_unscaled_values.size(); i++)
    {
        double scaled_sumrow = 0;
        double epsilon = 1e-9;
        outputfile << "La barra gran número " << i + 1 << " de mida " << b * precision << " metres es tallarà en els pals de mides: ";
        for (int j = 0; j < total_unscaled_values[i].size(); j++)
        {
            outputfile << " [" << total_unscaled_values[i][j] * precision << "] ";
            scaled_sumrow += total_unscaled_values[i][j] * precision;
        }
        outputfile << " (en metres) " << endl;
        outputfile << "D'aquesta barra en sobraran " << (((b * precision - scaled_sumrow) > epsilon) ? (b * precision - scaled_sumrow) : (0)) << " metres" << endl;
        sobrant = sobrant + b * precision - scaled_sumrow;
        outputfile << endl;
    }
    outputfile << "En total, hem aconseguit disposar els " << b * precision * total_unscaled_values.size() - sobrant << " metres de pals necessaris utilitzant " << b * precision * total_unscaled_values.size() << " metres en les barres" << endl;
    outputfile.close();
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
    vector<int> ini_wset = wset;
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

    int errorcontrol = checker(ini_wset, total_unscaled_values, precision, b);
    cout << "Process checked with error = " << errorcontrol << endl;

    if (errorcontrol == 0)
    {
        write_results(total_unscaled_values, precision, b, v);
    }
}

int main()
{
    int n;            // wset size
    int b;            // bound size
    double precision; // decimal precision for the weights
    cin >> n >> b >> precision;

    // define and read the wset of n elements
    vector<double> wset(n);
    for (double &x : wset)
        cin >> x;
    sort(wset.begin(), wset.end(), greater<double>()); // sorting for getting closer to optimality
    material_optimization(wset, b, precision);
}