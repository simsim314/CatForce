#include "LifeAPI.h"
#include <time.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>

int none[] = {1, 0, 0, 1};
int flipX[] = {-1, 0, 0, 1};
int flipY[] = {1, 0, 0, -1};
int flipXY[] = {-1, 0, 0, -1};

int rot90clock[] = {0, 1, -1, 0};
int rot90anti[] = {0, -1, 1, 0};
int symmXY[] = {0, 1, 1, 0};
int symmYX[] = {0, -1, -1, 0};

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

class SearchParams
{
public:
	int maxGen;
	int numCatalysts; 
	int stableInterval;
	std::string pat;
	int xPat;
	int yPat;
	
	int searchArea[4];
	
	SearchParams()
	{
		maxGen = 250;
		numCatalysts = 2;
		stableInterval = 15;
		pat = "";
		searchArea[0] = -10;
		searchArea[1] = 0;
		searchArea[2] = 20;
		searchArea[3] = 20;
		xPat = 0;
		yPat = 0;
	}
};

class CatalystInput
{
public:

	std::string rle;
	int maxDesapear; 
	int centerX; 
	int centerY;
	char symmType; 
	
	CatalystInput(std::string line)
	{
		std::vector<std::string> elems; 
		split(line, ' ', elems);
		
		if(elems.size() != 6)
		{
			std::cout << "The line " << line << "is invalid" << std::endl;
			std::cout << "Format: cat <rle> <absense interval> <centerX> <centerY> <symm Type | + / x *>" << std::endl;
			getchar();
			exit(0);
		}
		
		rle = elems[1];
		maxDesapear = atoi(elems[2].c_str());
		centerX = atoi(elems[3].c_str());
		centerY = atoi(elems[4].c_str());
		symmType = elems[5].at(0);
	}
	
	void Print()
	{
		std::cout << rle << " " << maxDesapear << " " << centerX << " " << centerY << " " << symmType << std::endl;
	}
};	

void CharToTransVec(char ch, std::vector<int* >& trans)
{
	trans.push_back(none);
	
	if(ch == '.')
		return;
		
	if(ch == '|')
	{
		trans.push_back(flipX);
		return;
	}
	
	if(ch == '+')
	{
		trans.push_back(flipX);
		trans.push_back(flipY);
		trans.push_back(flipXY);
		return;
	}
	
	if(ch == '/')
	{
		trans.push_back(symmXY);
		return;
	}
	
	if(ch == 'x')
	{
		trans.push_back(symmYX);
		trans.push_back(rot90anti);
		trans.push_back(rot90clock);
		return;
	}
	
	if(ch == '*')
	{
		trans.push_back(flipX);
		trans.push_back(flipY);
		trans.push_back(flipXY);
		trans.push_back(symmYX);
		trans.push_back(symmXY);
		trans.push_back(rot90anti);
		trans.push_back(rot90clock);
		return;
	}
}

void ReadParams(std::string fname, std::vector<CatalystInput>& catalysts, SearchParams& params)
{
	std::ifstream infile;
	infile.open(fname.c_str(), std::ifstream::in);

	std::string Cat = "cat";
	std::string maxGen = "max-gen";
	std::string numCat = "num-catalyst";
	std::string stable = "stable-interval";
	std::string area = "search-area";
	std::string pat = "pat";
	std::string line; 
	
	while (std::getline(infile, line))
	{
		try
		{
			std::vector<std::string> elems;
			split(line, ' ', elems);
			
			if(elems.size() < 2)
				continue; 
				
			if(elems[0] == Cat) 
				catalysts.push_back(CatalystInput(line));
			
			if(elems[0] == maxGen) 
				params.maxGen = atoi(elems[1].c_str());
				
			if(elems[0] == numCat) 
				params.numCatalysts = atoi(elems[1].c_str());
				
			if(elems[0] == stable) 
				params.stableInterval = atoi(elems[1].c_str());
				
			if(elems[0] == pat) 
			{
				params.pat = elems[1];
				
				if(elems.size() > 3)
				{
					params.xPat =  atoi(elems[2].c_str());
					params.yPat =  atoi(elems[3].c_str());
				}
			}
				
			if(elems[0] == area) 
			{
				params.searchArea[0] = atoi(elems[1].c_str());
				params.searchArea[1] = atoi(elems[2].c_str());
				params.searchArea[2] = atoi(elems[3].c_str());
				params.searchArea[3] = atoi(elems[4].c_str());
			}
		}
		catch(const std::exception& ex)
		{
		}
	}
}

void GenerateStates(const std::vector<CatalystInput>& catalysts, std::vector<LifeState*>& states, std::vector<int>& maxSurvive)
{
	for(int i = 0; i < catalysts.size(); i++)
	{
		std::vector<int* > trans;
		CharToTransVec(catalysts[i].symmType, trans);
		
		const char *rle = catalysts[i].rle.c_str();
		int dx = catalysts[i].centerX;
		int dy = catalysts[i].centerY;
		int maxDesapear = catalysts[i].maxDesapear;
		
		for(int j = 0; j < trans.size(); j++)
		{
			int dxx = trans[j][0];
			int dxy = trans[j][1];
			int dyx = trans[j][2];
			int dyy = trans[j][3];
			
			states.push_back(NewState(rle, dx, dy, dxx, dxy, dyx, dyy));
			maxSurvive.push_back(maxDesapear);
		}
	}
}

void InitCatalysts(std::string fname, std::vector<LifeState*>& states, std::vector<int>& maxSurvive, SearchParams& params)
{
	std::vector<CatalystInput> catalysts;
	ReadParams(fname, catalysts, params);
	GenerateStates(catalysts, states, maxSurvive);
} 

int main (int argc, char *argv[]) 
{
	if(argc < 2)
	{
		std::cout << "Usage CatForce.exe <in file>";
		exit(0);
	}
	
	std::string result = "x = 0, y = 0, rule = B3/S23\n";
	clock_t begin = clock();
	
	New();
   
	std::vector<LifeState*> states;
	std::vector<int> maxSurvive;
	
	SearchParams params;
	InitCatalysts(argv[1], states, maxSurvive, params);
	
	LifeState* pat =  NewState(params.pat.c_str(), params.xPat, params.yPat);
	
	int numIters = params.numCatalysts ;

	LifeIterator *iters[numIters];
	
	std::vector<LifeTarget*> targets;
	
	for(int i = 0; i < states.size(); i++)
		targets.push_back(NewTarget(states[i]));
		
	std::vector<int[64][64]> data; 
	
	int activated[numIters];
	int absentCount[numIters];

	LifeState* statesArr[states.size()];
	std::copy(states.begin(), states.end(), statesArr);
	
	for(int i = 0; i < numIters; i++)
		iters[i] = NewIterator(statesArr, params.searchArea[0], params.searchArea[1], params.searchArea[2], params.searchArea[3], states.size());
	
	clock_t current = clock();
	int idx = 0; 
	int found = 0; 
	long total = 0; 
	
	do{
		int valid = Validate(iters, numIters); 
		if(valid == NO)
			continue;
		
		total++;
	}while(Next(iters, numIters, NO));
	
	std::cout << "Total Checks: " << total << std::endl;
	
	total = total / 1000000;
	
	if(total == 0)
		total++;
		
	do{
		int valid = Validate(iters, numIters); 
		
		if(valid == NO)
			continue;
			
		idx++; 
		
		if(idx % 1000000 == 0)
		{
			if((double)(clock() - current) / CLOCKS_PER_SEC > 5)
			{
				current = clock();
				std::cout << "Checked: " << (idx / 10000) / total << "%, " <<idx / 1000000 << "M / "  << total << "M, found: " << found <<", elapsed: " << (clock() - begin) / CLOCKS_PER_SEC << std::endl;
				
				std::ofstream resultsFile("results.rle");
				resultsFile << result;
				resultsFile.close();
				
			}
		}
		
		New();
		
		for(int i = 0; i < numIters; i++)
			PutState(iters[i]);
		
		Run(1);

		int collide = NO;
		
		for(int i = 0; i < numIters; i++)
		{
			if(Contains(GlobalState, targets[iters[i]->curs], iters[i]->curx, iters[i]->cury) == NO)
			{
				collide = YES;
				break;
			}
		}

		if(collide == YES)
			continue;
	

		PutState(pat);
		
		for(int i = 0; i < numIters; i++)
		{
			if(Contains(GlobalState, targets[iters[i]->curs], iters[i]->curx, iters[i]->cury) == NO)
			{
				collide = YES;
				break;
			}
		}
					
		if(collide == YES)
			continue;
	
		for(int i = 0; i < numIters; i++)
		{
			activated[i] = NO;
			absentCount[i] = 0;
		}
		
		int surviveCount = 0;
		
		for(int i = 0; i < params.maxGen; i++)
		{
			Run(1);			
			bool fail = false;

			for(int j = 0; j < numIters; j++)
			{
				if(Contains(GlobalState, targets[iters[j]->curs], iters[j]->curx, iters[j]->cury) == NO)
				{
					activated[j] = YES;
					absentCount[j]++;
					
					if(absentCount[j] > maxSurvive[iters[j]->curs])
					{
						fail = true;
						break;
					}
				}
				else
				{
					absentCount[j] = 0;
				}
			}		
			
			if(fail)
				break;
				
			int isAllActivated = YES;
			
			for(int j = 0; j < numIters; j++)
			{	
				if(activated[j] == NO || absentCount[j] != 0)
				{
					isAllActivated = NO;
					break;
				}
			}
			
			if(isAllActivated == YES)
				surviveCount++;
			else
				surviveCount = 0;
			
			if(surviveCount >= params.stableInterval)
			{
				New();
					
				for(int j = 0; j < numIters; j++)
				{
					PutState(iters[j]);
				}
				
				PutState(pat);
				result.append(GetRLE(GlobalState));
				result.append("100$");
				found++;
				
				break;
			}
		}
	}while(Next(iters, numIters, NO));
	
	std::cout << "Checked: " << (idx / 10000) / total << "%, " <<idx / 1000000 << "M / "  << total << "M, found: " << found <<", elapsed: " << (clock() - begin) / CLOCKS_PER_SEC << std::endl;
	std::ofstream resultsFile("results.rle");
    resultsFile << result;
    resultsFile.close();
	
	printf("!");
	printf("\n\nFINISH\n");
	clock_t end = clock();
	printf("Elapsed: %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);

	getchar();
}