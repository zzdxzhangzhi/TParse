#if defined(__STDC_LIB_EXT1__)
#if (__STDC_LIB_EXT1__ >= 201112L)
#define __STDC_WANT_LIB_EXT1__ 1 /* Want the ext1 functions */
#endif
#endif


#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <utility>
#include <memory>
#include <limits>
#include <regex>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>


using std::cin;
using std::cout;
using std::string;
using std::vector;
using std::pair;
using std::tuple;
using std::equal_to;
using std::less;
//using std::numeric_limits;
using std::unique_ptr;
using std::endl;
using std::flush;
using std::bind;
using std::find_if;
using std::find;
using std::find_first_of;
using std::make_unique;
//using std::fprintf;
using std::snprintf;
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
using std::all_of;
using namespace std::placeholders;

namespace DS {
	static size_t t = 0;
	static size_t iTSpace = 0;
	const int UNDEF = 1 << 30;
}

inline string& trim(string &s)
{
	if (s.empty())
	{
		return s;
	}

	s.erase(0, s.find_first_not_of(" \t\r\n\v\f"));
	s.erase(s.find_last_not_of("  \t\r\n\v\f") + 1);
	return s;
}

const vector<string>& split(const string &origStr, vector<string>& resStr, 
	const string &delim = "  \t\r\n\v\f")
{
	
	if (origStr != "")
	{
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
	}

	return resStr;
}

inline bool is_digits(const string &str)
{
	return all_of(str.begin(), str.end(), ::isdigit); // C++11
}

// produce dominating state from boundary states
// 0= 'in DS', 1= 'dominated but not in DS', 2= 'future dominated'
// f is the function result serials of the vertices and DS
// t is the t-parse number
inline int rankDS(const vector<int>& f)
{
	int iNum = 0;
	for (size_t i = 0; i < DS::t + 1; ++i)
		iNum = iNum * 3 + f[DS::t - i];

	return iNum;
}

// extract boundary vertex dominating state from index
// f is the function result serials of the vertices and DS
// t is the t-parse number
inline unique_ptr<vector<int>> unrankDS(size_t iNum)
{
	auto f = make_unique<vector<int>>();
	for (size_t i = 0; i < DS::t + 1; ++i)
	{
		f->push_back(iNum % 3);
		iNum /= 3;
	}

	return move(f);
}

// how many in DS without internal DS vertices
int initCount(int n)
{
	int iCount = 0;
	auto f = unrankDS(n);
	for(auto iter = f->begin(); iter != f->end(); ++iter)
	{
		if (*iter == 1)
			return DS::UNDEF;

		if (*iter == 0)
			++iCount;
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
#if (__STDC_WANT_LIB_EXT1__ == 1)
		sprintf_s(tokStr, 3, "%d", m_tok); 
#else
		snprintf(tokStr, 3, "%d", m_tok);
#endif
		return tokStr; 
	}

	bool isVertexOp() { return (m_tok <= 9); }
	bool isEdgeOp() { return (m_tok > 9); }

	int v1() { return (m_tok % 10); } //vertex 1
	int v2() { return (m_tok / 10); } //vertex 2 if tOp is edgeOp

private:
	int m_tok;
};

inline void updateStatesBy(int v, const unique_ptr<vector<int>>& f, 
	const vector<int>& copyOfStates, const unique_ptr<vector<int>>& pStates, size_t iIndex)
{
	vector<int> fnew(f->cbegin(), f->cend());
	fnew[v] = 2;
	int iNew = rankDS(fnew);
	pStates->at(iIndex) = min(copyOfStates[iIndex], copyOfStates[iNew]);
}

// dynamic program for Pathwidth Dominating Set
// states is the initial state list of all the partial solution of G[i]
unique_ptr<vector<int>> pwDS(const string& pwToks, unique_ptr<vector<int>> pStates)
{
	//cout << "in pwDS, G = " << pwToks << endl;
	string tokens(pwToks);
	vector<string> pwTokList;
	split(trim(tokens), pwTokList);
	//for_each(pwTokList.cbegin(), pwTokList.cend(), [](auto &str) { cout << str << " "; });
	//cout << endl;

	for (auto strTok : pwTokList)
	{
		if (!is_digits(strTok))
			continue;

		tOp ope(strTok);
		vector<int> statesOld(pStates->cbegin(), pStates->cend());
		
		int v1 = ope.v1(); //vertex op or the first vertex of the edge op
		if (ope.isEdgeOp()) //edge op
		{
			int v2 = ope.v2(); //second vertex of the edge op
			
			for (size_t i = 0; i < DS::iTSpace; ++i)
			{
				auto f = unrankDS(i);
				if ((f->at(v1) == 0 && f->at(v2) == 2) || (f->at(v1) == 2 && f->at(v2) == 0))
					pStates->at(i) = DS::UNDEF;
				else if (f->at(v1) == 0 && f->at(v2) == 1)
					updateStatesBy(v2, f, statesOld, pStates, i);
				else if (f->at(v1) == 1 && f->at(v2) == 0)
					updateStatesBy(v1, f, statesOld, pStates, i);
			}
		}
		else //vertex op
		{
			for (size_t i = 0; i < DS::iTSpace; ++i)
			{
				auto f = unrankDS(i);
				if (f->at(v1) == 0)
				{
					vector<int> fnew(f->cbegin(), f->cend());
					fnew[v1] = 1;
					int iNew = rankDS(fnew);
					pStates->at(i) = min(statesOld[i], statesOld[iNew]) + 1;
				}					
				else if (f->at(v1) == 1)
					pStates->at(i) = DS::UNDEF;
				else if (f->at(v1) == 2)
				{
					vector<int> fn1(f->cbegin(), f->cend());
					vector<int> fn2(f->cbegin(), f->cend());
					fn1[v1] = 0;
					fn2[v1] = 1;
					int iNew1 = rankDS(fn1);
					int iNew2 = rankDS(fn2);
					pStates->at(i) = min(statesOld[iNew1], statesOld[iNew2]);
				}
			}
		}
	}

	return move(pStates);
}

// dynamic program for Treewidth Dominating Set
unique_ptr<vector<int>> twDS(string& G)
{
	G = trim(G);
	//cout << "G = " << G << endl;

	auto pStates = make_unique<vector<int>>(DS::iTSpace);
	size_t n = 0;
	for (auto iter = pStates->begin(); iter != pStates->end(); ++iter)
		*iter = initCount(n++);

	if (G.length() == 0)  // empty t-parse
		return move(pStates);

	if (G.front() != '(') // not a treewidth, only compute pathwidth t-parse
		return pwDS(G, move(pStates));

	int iLevel = 1;
	for (size_t i = 1; i < G.length(); ++i)  // doing a circle plus operator
	{
		if (G[i] == ')')
			--iLevel;
		else if (G[i] == '(')
			++iLevel;
				
		if (iLevel == 0) // left of a circle plus operator
		{
			string firstPart = G.substr(1, i - 1);
			auto pStates1 = twDS(firstPart); // strip a level of ()'s

			while (true)
			{
				string opers = G.substr(i + 1);
				size_t k = opers.find_first_of('(');  // level will imeadiately be set to 1 below
				if (k == string::npos)
					return pwDS(opers, move(pStates1)); // continue to compute pathwidth after a treewidth

				size_t i2ndStart = i + 1 + k;
				for (size_t j = i2ndStart; j < G.length(); ++j) // get 2nd (next) argument to circle plus
				{
					if (G[j] == ')')
						--iLevel;
					else if (G[j] == '(')
						++iLevel;
										
					if (iLevel == 0)
					{
						string secondPart = G.substr(i2ndStart + 1, j - 1 - i2ndStart);
						auto pStates2 = twDS(secondPart); // right of Circle plus
						//for_each(pStates->begin(), pStates->end(), [] (auto &i) mutable { i = DS::UNDEF; }); 
						pStates->assign(DS::iTSpace, DS::UNDEF);// re-initialize states for the right part use

						// now update state for circle plus
						for (size_t x = 0; x < DS::iTSpace; ++x)
						{
							for (size_t y = 0; y < DS::iTSpace; ++y)
							{
								auto f1 = unrankDS(x), f2 = unrankDS(y);
								vector<int> f;
								int iCommon = 0;

								// compute new boundary f() conditions
								for (size_t z = 0; z < DS::t + 1; ++z)
								{
									if (f1->at(z) == 0 && f2->at(z) == 0)
										++iCommon;

									if (f1->at(z) == 0 || f2->at(z) == 0)
										f.push_back(0);
									else if (f1->at(z) == 2 && f2->at(z) == 2)
										f.push_back(2);
									else
										f.push_back(1);
								}

								int iR = rankDS(f);
								int iS1 = pStates1->at(rankDS(*f1));
								int iS2 = pStates2->at(rankDS(*f2));

								if (iS1 < DS::UNDEF && iS2 < DS::UNDEF)
									pStates->at(iR) = min(pStates->at(iR), iS1 + iS2 - iCommon);
							}
						}

						if (j + 1 < G.length())
						{
							pStates1->assign(pStates->cbegin(), pStates->cend());
							i = j + 1;
							break;   // to next while iteration to look for another circle plus
						}
						else
							return move(pStates);
					}
				}
			}
		}
	}
	return move(pStates);
}

int main(int argc, char** argv)
{
	string strInput;

	while (getline(cin, strInput))
	{
		if (strInput == "")
			continue;

		string& line = trim(strInput);
		int iTParseStart = static_cast<int>(line.find_first_of('('));
		DS::t = atoi(line.substr(0, iTParseStart).c_str()); // get global t for t-parse
		//cout << "t = " << DS::t << endl;
		DS::iTSpace = static_cast<size_t>(pow(3, DS::t + 1));

		int iBestNum = DS::UNDEF;
		string tparseG = line.substr(iTParseStart);
		auto pStates = twDS(tparseG);

		for (size_t i = 0; i < DS::iTSpace; ++i)
		{
			bool flag = true;
			auto f = unrankDS(i);
			for (auto iter = f->cbegin(); iter != f->cend(); ++iter)
			{
				if (*iter == 2)
				{
					flag = false;
					break;
				}
			}

			if (flag)
				iBestNum = min(iBestNum, pStates->at(i));
		}

		cout << iBestNum << endl;

		if (DS::t == 0)
			break;
	}

	return 0;
}