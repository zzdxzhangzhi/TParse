#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <utility>
#include <limits>
#include <cstring>
#include <cstdlib>


using std::cin;
using std::cout;
using std::string;
using std::vector;
using std::pair;
using std::tuple;
using std::equal_to;
using std::less;
using std::numeric_limits;
using std::endl;
using std::flush;
using std::bind;
using std::find_if;
using std::find;
using std::find_first_of;
using std::fprintf;
using std::sprintf;
using std::swap;
using std::sort;
using std::make_pair;
using std::make_tuple;
using std::get;
using std::for_each;
using std::mismatch;
using std::move;
using namespace std::placeholders;

namespace DS {
	static int t = 0;
}

inline string&& trim(string &s)
{
	if (s.empty())
	{
		return move(s);
	}

	s.erase(0, s.find_first_not_of(" \t\r\n\v\f"));
	s.erase(s.find_last_not_of("  \t\r\n\v\f") + 1);
	return move(s);
}

vector<string>&& split(const string &origStr, const string &delim)
{
	vector<string> resStr;
	size_t last = 0;
	size_t index = origStr.find_first_of(delim, last);
	while (index != string::npos)
	{
		resStr.push_back(origStr.substr(last, index - last));
		last = index + 1;
		index = origStr.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		resStr.push_back(origStr.substr(last, index - last));
	}

	return move(resStr);
}

//produce dominating state from boundary states
//0= 'in DS', 1= 'dominated but not in DS', 2= 'future dominated'
//f is the function result serials of the vertices and DS
//t is the t-parse number
inline int rankDS(const vector<int>& f)
{
	int iNum = 0;
	for (size_t i = 0; i < DS::t + 1; i++)
		iNum = iNum * 3 + f[DS::t - i];

	return iNum;
}

//extract boundary vertex dominating state from index
//f is the function result serials of the vertices and DS
//t is the t-parse number
inline vector<int>&& unrankDS(int iNum)
{
	vector<int> f;
	for (size_t i = 0; i < DS::t + 1; i++)
	{
		f.push_back(iNum % 3);
		iNum /= 3;
	}

	return move(f);
}

//how many in DS without internal DS vertices
int initCount(int n)
{
	int iCount = 0;
	vector<int> f = unrankDS(n);
	for (auto &i : f)
	{
		if (i == 1)
			return numeric_limits<int>::max();

		if (i == 0)
			iCount++;
	}
	
	return iCount;
}

//A t-parse Operator
class tOp
{
public:
	tOp() = default;
	tOp(int iOp) : m_tok(iOp) {}
	
	string str() 
	{ 
		char tokStr[3] = ""; 
		sprintf(tokStr, "%d", m_tok); 
		return tokStr; 
	}

	bool isVertexOp() { return (m_tok <= 9); }
	bool isEdgeOp() { return (m_tok > 9); }

	int v1() { return (m_tok % 10); } //vertex 1
	int v2() { return (m_tok / 10); } //vertex 2 if tOp is edgeOp

private:
	int m_tok;
};

