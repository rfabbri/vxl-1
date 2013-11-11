#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <math.h>
#include "kofn.h"

double drand()
{
  return (double)rand() / (double)32767;
}

int rand_in_range (int a, int b)
{
    assert (a <= b);
    int x = rand();
    return (int) (double((b-a)*x)/32767.0 + a);
}

// O(n) implementation.
static void
_kOfN_largeK (int k, int n, int* values)
{
    assert (k > 0);
    assert (k <= n);
    int j = 0;
    for (int i = 0; i < n; i++) {
        double prob = (double) (k - j) / (n - i);
        //assert (prob <= 1);
        //double x = Random::rand1.fp ();
        double x = drand();
        assert(x<=1);
        if (x <= prob) {
            values[j++] = i;
        }

        if (j==k)
          break;
    }
    assert (j == k);
}

// O(k*lg(k)) implementation; constant factor is about 2x the constant
// factor for the O(n) implementation.
static void
_kOfN_smallK (int k, int n, int* values)
{
    assert (k > 0);
    assert (k <= n);
    if (k == 1) {
        //values[0] = Random::rand1.i32 (0, n - 1);
        values[0] = rand_in_range(0, n - 1);
        assert(values[0]<=(n-1));
        return;
    }
    int leftN = n / 2;
    int rightN = n - leftN;
    int leftK = 0;
    int rightK = 0;
    for (int i = 0; i < k; i++) {
        //int x = Random::rand1.i32 (0, n - i - 1);
        int x = rand_in_range(0, n - i - 1);
        assert(x<=(n-i-1));
        if (x < leftN - leftK) {
            leftK++; 
        } else {
            rightK++;
        }
    }
    if (leftK > 0) { _kOfN_smallK (leftK, leftN, values); }
    if (rightK > 0) { _kOfN_smallK (rightK, rightN, values + leftK); }
    for (int i = leftK; i < k; i++) {
        values[i] += leftN;
    }
}

// Return k randomly selected integers from the interval [0,n), in
// increasing sorted order.
void
kOfN (int k, int n, int* values)
{
    assert (k >= 0);
    assert (n >= 0);
    if (k == 0) { return; }
    static double log2 = log (2.0);
    double klogk = k * log ((double)k) / log2;
    if (klogk < n / 2) {
        _kOfN_smallK (k, n, values);
    } else {
        _kOfN_largeK (k, n, values);
    }
}

