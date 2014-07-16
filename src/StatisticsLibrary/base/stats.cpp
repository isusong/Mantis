/*
 * Copyright 2008-2014 Iowa State University
 *
 * This file is part of Mantis.
 * 
 * This computer software was prepared by The Ames 
 * Laboratory, hereinafter the Contractor, under 
 * Interagency Agreement number 2009-DN-R-119 between 
 * the National Institute of Justice (NIJ) and the 
 * Department of Energy (DOE). All rights in the computer 
 * software are reserved by NIJ/DOE on behalf of the 
 * United States Government and the Contractor as provided 
 * in its Contract, DE-AC02-07CH11358.  You are authorized 
 * to use this computer software for Governmental purposes
 * but it is not to be released or distributed to the public.  
 * NEITHER THE GOVERNMENT NOR THE CONTRACTOR MAKES ANY WARRANTY, 
 * EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE 
 * OF THIS SOFTWARE.  
 *
 * This notice including this sentence 
 * must appear on any copies of this computer software.
 *
 * Author: Max Morris (mmorris@iastate.edu)
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include "stats.h"
#include <utility>
#include <iterator> 
using namespace std;

namespace stats {
  double mean(const vector<double>& v)
  {
    double sum = 0.0;
    const size_t n = v.size();
    for (size_t i = 0; i != n; ++i) {
      sum += v[i];
    }
    return sum / n;
  }
}


void meanAndVar(const vector<double>& v, double& vMean, double& var)
{
  using namespace stats;
  const double m = mean(v);
  const size_t n = v.size();
  double sum = 0.0;
  for (size_t i = 0; i != n; ++i) {
    const double diff = v[i] - m;
    sum += diff * diff;
  }
  vMean = m;
  var = sum / (n - 1);
}

/**
 * Indicates whether a given value is from the first or second
 * bunch of values.
 */
typedef enum {FIRST = 0, SECOND = 1} DataSetTag;

/**
 * Wrapper for a value that reveals only the value's ordinal qualities
 * and tags it with the number of its data set.
 */
template<typename T>
class Ordinal {
public:
  Ordinal(T value, DataSetTag tag) : value(value), _tag(tag) {}
  bool operator<(const Ordinal<T>& anOrdinal) const {
    return value < anOrdinal.value;
  }

  bool operator==(const Ordinal<T>& anOrdinal) const {
    return value == anOrdinal.value;
  }

  DataSetTag tag() const { return _tag; }

private:
  T value;
  DataSetTag _tag;
};

/**
 * A functor which takes a given value
 * and wraps it in an Ordinal with the
 * given tag.
 */
template<typename T>
class WrapInOrdinalWithTag {
public:
  WrapInOrdinalWithTag(DataSetTag tag) : tag(tag) {}

Ordinal<T> operator()(T value)
{
  return Ordinal<T>(value, tag);
}

private:
  DataSetTag tag;
};

/**
 * Wraps each value from values in an Ordinal and appends it to
 * result, giving each Ordinal the given the tag.
 */
template<typename T>
void appendAsOrdinalsTo(const vector<T>& values, vector<Ordinal<T> >& result,
		  DataSetTag tag)
{
  //XXX! It would be better to insert the values rather than
  //appending them. We'll always know the maximum length of the
  //final vector so there's no point in wasting time appending
  //and so having to resize the storage.
  transform(values.begin(), values.end(),
	    back_inserter(result), WrapInOrdinalWithTag<T>(tag));
}

/**
 * Returns a pair in which the first element is the sum of the ranks for values1 and
 * the second is the sum of the squares of the ranks for both values1 and values2.
 */
template<typename T>
pair<double, double>
computeRankSumSqSum(const vector<T>& values1, const vector<T>& values2)
{
  vector<Ordinal<T> > ordinals;
  ordinals.reserve(values1.size() + values2.size());
  appendAsOrdinalsTo(values1, ordinals, FIRST);
  appendAsOrdinalsTo(values2, ordinals, SECOND);
  
  sort(ordinals.begin(), ordinals.end());

  assert(ordinals.size() == values1.size() + values2.size());

  double rankSum1 = 0.0;
  double sqRankSum = 0.0;

  int size = ordinals.size();
  int numberEqual[2];
  
  for (int i = 0; i < size;) {
    Ordinal<T>& o = ordinals[i];
    int tag = o.tag();
    
    assert(tag == 1 || tag == 0);

    numberEqual[tag] = 1;
    numberEqual[1 - tag] = 0;
    for (int j = i + 1; j < size && o == ordinals[j]; ++j) {
      int tag = ordinals[j].tag();
      ++numberEqual[tag];
    }

    int totalNumberEqual = numberEqual[0] + numberEqual[1];

    //rank r = i + 1
    //sumOfRanks = r + (r+1) + ... + (r+totalNumberEqual-1)
    //     = totalNumberEqual * (r + (r+totalNumberEqual-1))/2
    //     = totalNumberEqual * ((i+1) +((i+1)+totalNumberEqual-1))/2
    //     = totalNumberEqual * (2*i + 1 + totalNumberEqual)/2
    
    double sumOfRanks = (totalNumberEqual * (2*i + 1 + totalNumberEqual))/2;

    double averageRank = sumOfRanks/totalNumberEqual;

    rankSum1 += averageRank * numberEqual[0];
	sqRankSum += totalNumberEqual * averageRank * averageRank;
    i += totalNumberEqual;
  }
  return make_pair(rankSum1, sqRankSum);
}

/**
 * Returns Conover's T1 statistic (which is approximately
 * normal) for samples x and y.
 */
double t1Statistic(const vector<double>& x, const vector<double>& y)
{
  double n = x.size();
  double m = y.size();
  double N = m + n;
  pair<double, double> r(computeRankSumSqSum(x,y));
  double T = r.first;
  double R2 = r.second;
  double N1 = N + 1;
  double top = T - n * N1/2.0;
  double bot2 = n*m/(N*(N-1))*R2 - n*m*N1*N1/(4*(N-1));
  return top / sqrt(bot2);
}
