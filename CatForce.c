#include "LifeAPI.h"
#include <time.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::string line;

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
		
		if(elems.size() != 5)
		{
			std::cout << "The line " << line << "is invalid" << std::endl;
			std::cout << "Format: <rle> <absense interval> <centerX> <centerY> <symm Type | + / x *>" << std::endl;
			getchar();
			exit(0);
		}
		
		rle = elems[0];
		maxDesapear = atoi(elems[1].c_str());
		centerX = atoi(elems[2].c_str());
		centerY = atoi(elems[3].c_str());
		symmType = elems[4].at(0);
	}
	
	void Print()
	{
		std::cout << rle << " " << maxDesapear << " " << centerX << " " << centerY << " " << symmType << std::endl;
	}
};	

void CharToTransVec(char ch, std::vector<int* >& trans)
{
	int none[] = {1, 0, 0, 1};
	int flipX[] = {-1, 0, 0, 1};
	int flipY[] = {1, 0, 0, -1};
	int flipXY[] = {-1, 0, 0, -1};
	
	int rot90clock[] = {0, 1, -1, 0};
	int rot90anti[] = {0, -1, 1, 0};
	int symmXY[] = {0, 1, 1, 0};
	int symmYX[] = {0, -1, -1, 0};
	
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

void ReadCatalyst(std::string fname, std::vector<CatalystInput>& catalysts)
{
	std::ifstream infile;
	infile.open(fname.c_str(), std::ifstream::in);

	while (std::getline(infile, line))
	{
		catalysts.push_back(CatalystInput(line));
	}
}

void GenerateStates(const std::vector<CatalystInput>& catalysts, std::vector<LifeState*>& states)
{
	for(int i = 0; i < catalysts.size(); i++)
	{
		std::vector<int* > trans;
		CharToTransVec(catalysts[i].symmType, trans);
		
		
		const char *rle = catalysts[i].rle.c_str();
		int dx = catalysts[i].centerX;
		int dy = catalysts[i].centerY;
		for(int j = 0; j < trans.size(); j++)
		{
			int dxx = trans[j][0];
			int dxy = trans[j][1];
			int dyx = trans[j][2];
			int dyy = trans[j][3];

			states.push_back(NewState(rle, dx, dy, dxx, dxy, dyx, dyy));
		}
			
	}
}

void InitCatalysts(std::string fname, std::vector<LifeState*>& states)
{
	std::vector<CatalystInput> catalysts;
	ReadCatalyst(fname, catalysts);
	GenerateStates(catalysts, states);
} 

int main (int argc, char *argv[]) 
{

	printf("x = 0, y = 0, rule = B3/S23\n");
	clock_t begin = clock();

	New();
   
	//LifeState* blck =  NewState("2o$2o!");
	std::vector<LifeState*> states;
	
	InitCatalysts("1.in", states);
	LifeState* pat =  NewState("b3o$bo$3o!");
	//LifeState* pat =  NewState("6b3o$6bo$5b3o15$2o$2o7b2o3b2o$9b2o3b2o!", -5, 0);

	int numIters = 2;

	LifeIterator *iters[numIters];
	
	std::vector<LifeTarget*> targets;
	
	for(int i = 0; i < states.size(); i++)
		targets.push_back(NewTarget(states[i]));
		
	//LifeTarget* target = NewTarget(blck);
	int activated[numIters];

	LifeState* statesArr[states.size()];
	std::copy(states.begin(), states.end(), statesArr);
	
	for(int i = 0; i < numIters; i++)
		iters[i] = NewIterator(statesArr, -10, -10, 20, 20, states.size());
	
	do{
		int valid = Validate(iters, numIters); 
		
		if(valid == YES)
		{
			New();
			
			for(int i = 0; i < numIters; i++)
			{
				PutState(iters[i]);
			}

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

			if(collide == NO)
			{
				PutState(pat);
				
				for(int i = 0; i < numIters; i++)
				{
					if(Contains(GlobalState, targets[iters[i]->curs], iters[i]->curx, iters[i]->cury) == NO)
					{
						collide = YES;
						break;
					}
				}
			}			
			
			if(collide == NO)
			{ 
				for(int i = 0; i < numIters; i++)
					activated[i] = NO;
				
				int surviveCount = -1;
				
				for(int i = 0; i < 250; i++)
				{

					Run(1);			
					
					for(int j = 0; j < numIters; j++)
					{
						if(Contains(GlobalState, targets[iters[j]->curs], iters[j]->curx, iters[j]->cury) == NO)
						{
							activated[j] = YES;
						}
					}		
					
					
					int isAllActivated = YES;
					
					for(int j = 0; j < numIters; j++)
					{	
						if(activated[j] == NO)
						{
							isAllActivated = NO;
							break;
						}
					}
					
					
					if(isAllActivated == YES)
					{
						int survive = YES;
						
						for(int j = 0; j < numIters; j++)
						{
							if(Contains(GlobalState, targets[iters[j]->curs], iters[j]->curx, iters[j]->cury) != YES)
							{
								survive = NO;
								surviveCount = -1;
								break;
							}
						}	
						
						if(survive == YES)
							surviveCount++;
							
						if(surviveCount > 15)
						{
							New();
								
							for(int j = 0; j < numIters; j++)
							{
								PutState(iters[j]);
							}

							PutState(pat);

							{
								printf(GetRLE(GlobalState));
								printf("100$");
							}
							
							break;
						}
					}
				}
			}
		}
	}while(Next(iters, numIters, NO));

	printf("!");
	printf("\n\nFINISH");
	clock_t end = clock();
	printf("Elapsed: %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);

	getchar();
}