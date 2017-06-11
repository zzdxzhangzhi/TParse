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
using std::pow;
using std::min;
using namespace std::placeholders;

namespace DS {
	static int t = 0;
	static int iTSpace = 0;
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

vector<string>&& split(const string &origStr, const string &delim = "  \t\r\n\v\f")
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

// produce dominating state from boundary states
// 0= 'in DS', 1= 'dominated but not in DS', 2= 'future dominated'
// f is the function result serials of the vertices and DS
// t is the t-parse number
inline int rankDS(const vector<int>& f)
{
	int iNum = 0;
	for (size_t i = 0; i < DS::t + 1; i++)
		iNum = iNum * 3 + f[DS::t - i];

	return iNum;
}

// extract boundary vertex dominating state from index
// f is the function result serials of the vertices and DS
// t is the t-parse number
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

// how many in DS without internal DS vertices
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

// A t-parse Operator
class tOp
{
public:
	tOp() = default;
	explicit tOp(int iOp) : m_tok(iOp) {}
	explicit tOp(const string& sOp) : m_tok(atoi(sOp.c_str())) {}
	
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

inline void updateStatesBy(int v, const vector<int>& f, 
	const vector<int>& copyOfStates, vector<int>& states, int iIndex)
{
	vector<int> fnew = f;
	fnew[v] = 2;
	int iNew = rankDS(fnew);
	states[iIndex] = min(copyOfStates[iIndex], copyOfStates[iNew]);
}

// dynamic program for Pathwidth Dominating Set
// states is the initial state list of all the partial solution of G[i]
vector<int>&& pwDS(const string& pwToks, vector<int>& states)
{
	vector<string>&& pwTokList = split(pwToks);
	for (auto strTok : pwTokList)
	{
		tOp ope(strTok);
		vector<int> statesOld = states;
		
		int v1 = ope.v1(); //vertex op or the first vertex of the edge op
		if (ope.isEdgeOp()) //edge op
		{
			int v2 = ope.v2(); //second vertex of the edge op
			
			for (int i = 0; i < DS::iTSpace; i++)
			{
				vector<int>&& f = unrankDS(i);
				if ((f[v1] == 0 && f[v2] == 2) || (f[v1] == 2 && f[v2] == 0))
					states[i] = numeric_limits<int>::max();
				else if (f[v1] == 0 && f[v2] == 1)
					updateStatesBy(v2, f, statesOld, states, i);
				else if (f[v1] == 1 && f[v2] == 0)
					updateStatesBy(v1, f, statesOld, states, i);
			}
		}
		else //vertex op
		{
			for (int i = 0; i < DS::iTSpace; i++)
			{
				vector<int>&& f = unrankDS(i);
				if (f[v1] == 0)
				{
					vector<int> fnew = f;
					fnew[v1] = 1;
					int iNew = rankDS(fnew);
					states[i] = min(statesOld[i], statesOld[iNew]) + 1;
				}					
				else if (f[v1] == 1)
					states[i] = numeric_limits<int>::max();
				else if (f[v1] == 2)
				{
					vector<int> fn1 = f, fn2 = f;
					fn1[v1] = 0;
					fn2[v1] = 1;
					int iNew1 = rankDS(fn1);
					int iNew2 = rankDS(fn2);
					states[i] = min(statesOld[iNew1], statesOld[iNew2]);
				}
			}
		}
	}

	return move(states);
}

// dynamic program for Treewidth Dominating Set
vector<int>&& twDS(string& G)
{
	G = trim(G);
	cout << "G = " << G << endl;

	vector<int> states(DS::iTSpace);
	for_each(states.begin(), states.end(), [j = 0](auto &i) { i = initCount(j++); });

	if (G.length() == 0)  // empty t-parse
		return move(states);  

	if (G.front() != '(') // not a treewidth, only compute pathwidth t-parse
		return pwDS(G, states);

	int iLevel = 1;
	for (int i = 1; i < G.length(); i++)  // doing a circle plus operator
	{
		if (G[i] == ')')
			iLevel--;
		else if (G[i] == '(')
			iLevel++;

		vector<int> states1;
		if (iLevel == 0) // left of a circle plus operator
			states1 = twDS(G.substr(1, i - 1)); // strip a level of ()'s

		while (true)
		{
			int k = G.substr(i + 1).find_first_of('(');  // level will imeadiately be set to 1 below
			if (k == string::npos)
				return pwDS(G.substr(i + 1), states1); // continue to compute pathwidth after a treewidth

			int i2ndStart = i + 1 + k;
			for (int j = i2ndStart; j < G.length(); j++) // get 2nd (next) argument to circle plus
			{
				if (G[j] == ')')
					iLevel--;
				else if (G[j] == '(')
					iLevel++;

				vector<int> states2;
				if (iLevel == 0)
				{
					states2 = twDS(G.substr(i2ndStart + 1, j - 1)); // right of Circle plus
					for_each(states.begin(), states.end(), [j = 0](auto &i) { i = initCount(j++); }); // re-initialize states for the right part use

					// now update state for circle plus
					for (int x = 0; x < DS::iTSpace; x++)
					{
						for (int y = 0; y < DS::iTSpace; y++)
						{
							vector<int> f1 = unrankDS(x), f2 = unrankDS(y);
							vector<int> f;
							int iCommon = 0;

							// compute new boundary f() conditions
							for (int z = 0; z < DS::t + 1; z++)
							{
								if (f1[z] == 0 && f2[z] == 0)
									iCommon++;

								if (f1[z] == 0 || f2[z] == 0)
									f.push_back(0);
								else if (f1[z] == 2 && f2[z] == 2)
									f.push_back(2);
								else
									f.push_back(1);
							}

							int iR = rankDS(f);
							int iS1 = states1[rankDS(f1)];
							int iS2 = states2[rankDS(f2)];

							if (iS1 < numeric_limits<int>::max() && iS2 < numeric_limits<int>::max())
								states[iR] = min(states[iR], iS1 + iS2 - iCommon);
						}
					}

					if (j + 1 < G.length())
					{
						states1 = states;
						i = j + 1;
						break;   // to next while iteration to look for another circle plus
					}
					else
						return move(states);
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	string strInput;

	while (getline(cin, strInput))
	{
		if (strInput == "")
			continue;

		string line = trim(strInput);
		int iTParseStart = line.find_first_of('(');
		DS::t = atoi(line.substr(0, iTParseStart).c_str()); // get global t for t-parse
		cout << "t = " << DS::t << endl;
		DS::iTSpace = pow(3, DS::t + 1);

		int iBestNum = numeric_limits<int>::max();
		vector<int> states = twDS(line.substr(iTParseStart));

		for (int i = 0; i < DS::iTSpace; i++)
		{
			bool flag = true;
			for (auto fvalue : unrankDS(i))
			{
				if (fvalue == 2)
				{
					flag = false;
					break;
				}
			}

			if (flag)
				iBestNum = min(iBestNum, states[i]);
		}

		cout << iBestNum << endl;

		if (DS::t == 0)
			break;
	}

	return 0;
}