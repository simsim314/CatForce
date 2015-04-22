//CatForce - Catalyst search utility based on LifeAPI using brute force. 
//Written by Michael Simkin 2015
#include "LifeAPI.h"
#include <time.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <iomanip>

const int none[] = {1, 0, 0, 1};
const int flipX[] = {-1, 0, 0, 1};
const int flipY[] = {1, 0, 0, -1};
const int flipXY[] = {-1, 0, 0, -1};

const int rot90clock[] = {0, 1, -1, 0};
const int rot90anti[] = {0, -1, 1, 0};
const int symmXY[] = {0, 1, 1, 0};
const int symmYX[] = {0, -1, -1, 0};

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
	int startGen; 
	std::string outputFile;
	std::string fullReportFile;
	int searchArea[4];
	int maxW;
	int maxH;
	std::vector<std::string> targetFilter; 
	std::vector<int> filterdx; 
	std::vector<int> filterdy; 
	std::vector<int> filterGen; 
	
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
		startGen = 1;
		outputFile = "results.rle";
		maxW = -1;
		maxH = -1;
		fullReportFile = "";
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

void CharToTransVec(char ch, std::vector<const int* >& trans)
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
	
	if(ch == '/' || ch == '\\')
	{
		trans.push_back(symmXY);
		return;
	}
	//For 180 degree symetrical 
	if(ch == 'x')
	{
		trans.push_back(rot90clock);
		trans.push_back(flipX);
		trans.push_back(symmXY);
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
	std::string startGen = "start-gen";
	std::string numCat = "num-catalyst";
	std::string stable = "stable-interval";
	std::string area = "search-area";
	std::string pat = "pat";
	std::string outputFile = "output";
	std::string filter = "filter";
	std::string maxWH = "fit-in-width-height";
	std::string fullReport = "full-report";
	
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
			
			if(elems[0] == startGen) 
				params.startGen = atoi(elems[1].c_str());
			
			if(elems[0] == outputFile) 
			{
				params.outputFile = elems[1];
				
				for(int i = 2; i < elems.size(); i++)
				{	
					params.outputFile.append(" ");
					params.outputFile.append(elems[i]);
				}
			}
			
			if(elems[0] == fullReport) 
			{
				params.fullReportFile = elems[1];
				
				for(int i = 2; i < elems.size(); i++)
				{	
					params.fullReportFile.append(" ");
					params.fullReportFile.append(elems[i]);
				}
			}
			
			if(elems[0] == filter) 
			{
				params.filterGen.push_back(atoi(elems[1].c_str()));
				params.targetFilter.push_back(elems[2]);
				params.filterdx.push_back(atoi(elems[3].c_str()));
				params.filterdy.push_back(atoi(elems[4].c_str()));
			}
			
			if(elems[0] == maxWH)
			{
				params.maxW = atoi(elems[1].c_str());
				params.maxH = atoi(elems[2].c_str());
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
		std::vector<const int* > trans;
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

void XYStartGenPerState(const std::vector<LifeTarget*> &targets, LifeState* pat, const SearchParams& params, const std::vector<LifeState*>& states, std::vector<std::vector<std::vector<int> > >& statexyGen)
{

	for(int i = 0; i < states.size(); i++)
	{
		std::vector<std::vector<int> > xyVec; 
		
		for(int x = 0; x < 64; x++)
		{
			std::vector<int>  xVec; 
			
			for(int y = 0; y < 64; y++)
			{
				New();
				PutState(states[i], x, y);
				PutState(pat);
				int j; 
				
				for(j = 0; j < params.maxGen + 5; j++)
				{
					if(Contains(GlobalState, targets[i], x, y) == NO)
					{
						break;
					}
					Run(1);
				}
				
				if(j == params.maxGen + 4)
					j = -1;
				
				xVec.push_back(j - 1);
			}
			
			xyVec.push_back(xVec);
		}
		
		statexyGen.push_back(xyVec);
	}
}

void PreIteratePat(LifeState* pat, std::vector<LifeState*>& preIterated, const SearchParams& params)
{
	New();
	PutState(pat);
		
	for(int i = 0; i < params.maxGen + 5; i++)
	{
		LifeState* t = NewState();
		Copy(t, GlobalState);
		preIterated.push_back(t);
		Run(1);
	}
}

std::string GetRLE(const std::vector<std::vector<int> >& life2d)
{
	if(life2d.size() == 0)
		return "";
		
	if(life2d[0].size() == 0)
		return "";
	
	int h = life2d[0].size();
	
    std::stringstream result;

	int eol_count = 0; 
	
	for(int j = 0; j < h; j++)
	{
            int last_val = -1;
            int run_count = 0;
			
         for(int i = 0; i < life2d.size(); i++)
		{
                    int val = life2d[i][j];

                    // Flush linefeeds if we find a live cell
                    if(val == 1 && eol_count > 0)
					{
                            if(eol_count > 1)
                                result << eol_count;
                            
							result << "$";
                            
                            eol_count = 0;
					}

                    // Flush current run if val changes
                    if (val == 1 - last_val)
                    {
						if(run_count > 1)
							result << run_count;
						
						if(last_val == 1)
							result << "o"; 
						else
							result << "b";
							
                            
						run_count = 0;
					}

                    run_count++;
                    last_val = val;
                }

            // Flush run of live cells at end of line
            if (last_val == 1)
                {
                    if(run_count > 1)
                        result << run_count;
                            
                    result << "o";
                            
                    run_count = 0;
                }

            eol_count++;
	}
        
	return result.str();
}

class CategoryContainer
{
public:
	std::vector<std::pair<LifeState*, std::vector<LifeState*> > > categories;
	int catDelta; 
	LifeState* tempState;
	
	CategoryContainer()
	{
		catDelta = 5;
		tempState = NewState();
	}
	
	CategoryContainer(int cats)
	{
		catDelta = cats;
		tempState = NewState();
	}
	
	bool SameCategory(LifeState* a, LifeState* b)
	{
		if(a->gen > b->gen)
			Evolve(b, a->gen - b->gen);
		else if(a->gen < b->gen)
			Evolve(a, b->gen - a->gen);
		
		for(int i = 0; i < catDelta; i++)
		{
			if(AreEqual(a, b) == YES)
				return true;
			
			Evolve(a, 1);
			Evolve(b, 1);
		}
		
		return false;
	}
	
	void Add(LifeState* init, LifeState* afterCatalyst, LifeState* catalysts)
	{
		LifeState* result = NewState();
			
		for(int i = 0; i < categories.size(); i++)
		{
			ClearData(result);
			Copy(result, afterCatalyst, COPY);
			Copy(result, catalysts, XOR);
			ClearData(tempState);
			Copy(tempState, categories[i].first, COPY);
			
			if(SameCategory(tempState, result))
			{
				std::vector<LifeState*>* vec = &categories[i].second;
				ClearData(result);
				Copy(result, init, COPY);
				vec->push_back(result);
				return;
			}
		}
		
		LifeState* categoryKey = NewState();
		Copy(categoryKey, afterCatalyst, COPY);
		Copy(categoryKey, catalysts, XOR);
		
		ClearData(result);
		Copy(result, init, COPY);	
		std::vector<LifeState*> categoryVec; 
		categoryVec.push_back(result);
		categories.push_back(std::pair<LifeState*, std::vector<LifeState*> >(categoryKey, categoryVec));
	}
	
	std::string CategoriesRLE()
	{
		int largestCategory = 0; 
		
		for(int i = 0; i < categories.size(); i++)
		{
			std::vector<LifeState*>* vec = &(categories[i].second);
			if(vec->size() > largestCategory)
				largestCategory = vec->size();
		}
		const int Dist = 35 + 64; 
		
		int width = Dist * largestCategory;
		int height = Dist * categories.size();
		std::vector<std::vector<int> > vec;
		
		for(int i = 0; i < width; i++)
		{
			std::vector<int> temp; 
			
			for(int j = 0; j < height; j++)
				temp.push_back(0);
				
			vec.push_back(temp);
		}
		
		for(int cat = 0; cat < categories.size(); cat++)
		{
			const std::vector<LifeState*>& vecCat = categories[cat].second;
			
			for(int l = 0; l < vecCat.size(); l++)
				for(int j = 0; j < N; j++)
					for(int i = 0; i < N; i++)
						vec[Dist * l + i][Dist * cat + j] = Get(i, j, vecCat[l]->state);
			
		}
		
		return GetRLE(vec);
	}
};

class SearchSetup
{
public:

	std::string result;
	clock_t begin;
	std::vector<LifeState*> states;
	std::vector<int> maxSurvive;
	SearchParams params;
	LifeState* pat;
	int numIters;
	std::vector<LifeIterator*> iters;
	std::vector<LifeTarget*> targetFilter; 
	std::vector<LifeTarget*> targets;
	std::vector<std::vector<std::vector<int> > > statexyGen; 
	std::vector<LifeState*> preIterated;
	std::vector<int> activated;
	std::vector<int> absentCount;
	std::vector<LifeState*> statesArr;
	clock_t current;
	long long idx; 
	int found; 
	long long total; 
	std::string fullReport;
	unsigned short int counter; 
	CategoryContainer cats;
	
	void Init(char* inputFile)
	{
		result = "x = 0, y = 0, rule = B3/S23\n";
		begin = clock();
		InitCatalysts(inputFile, states, maxSurvive, params);
		pat =  NewState(params.pat.c_str(), params.xPat, params.yPat);
		numIters = params.numCatalysts;
		
		for(int i = 0; i < params.targetFilter.size(); i++)
			targetFilter.push_back(NewTarget(params.targetFilter[i].c_str(), params.filterdx[i], params.filterdy[i]));
	
		for(int i = 0; i < states.size(); i++)
			targets.push_back(NewTarget(states[i]));
		
		XYStartGenPerState(targets, pat, params, states, statexyGen);
		
		PreIteratePat(pat, preIterated, params);
		
		for(int i = 0; i < numIters; i++)
		{
			iters.push_back(NewIterator(&states[0], params.searchArea[0], params.searchArea[1], params.searchArea[2], params.searchArea[3], states.size()));
			activated.push_back(0);
			absentCount.push_back(0);
		}
		
		current = clock();
		idx = 0; 
		found = 0; 
		total = 1; 
		counter = 0; 
		
		int fact = 1;
		
		for(int i = 0; i < numIters; i++)
		{
			total *= (iters[i] -> w); 
			total *= (iters[i] -> h); 
			total *= (iters[i] -> s); 
			fact *= (i + 1);
		}
		
		total /= fact;
		
		std::cout << "Approximated Total: " << total << std::endl;
		total = total / 1000000;
		
		if(total == 0)
			total++;
			
		fullReport = "x = 0, y = 0, rule = B3/S23\n";
	}
		
	int FilterMaxGen()
	{
		int maxGen = -1;

		for(int j = 0; j < targetFilter.size(); j++)
		{
			if(params.filterGen[j] > maxGen)
				maxGen = params.filterGen[j];
		}
		
		return maxGen;
	}
	
	void Report()
	{
		float percent = (idx / 10000) / (total * 1.0);
		int sec = (clock() - begin) / CLOCKS_PER_SEC;
		int estimation = 0; 
		int checkPerSecond = idx / (sec * 1000);
		
		if(percent > 0)
			estimation = (sec * 100) / percent;
			
		std::cout << std::setprecision(1)  << std::fixed << percent << "%, " <<idx / 1000000 << "M / "  << total << "M, found: " << found <<", passed: ";
		PrintTime(sec);
		std::cout <<", estimation: ";
		PrintTime(estimation);
		std::cout <<", " << std::setprecision(1)  << std::fixed << checkPerSecond << "K/sec" << std::endl;
				
		std::ofstream resultsFile(params.outputFile.c_str());
		resultsFile << result;
		resultsFile.close();
		
		std::ofstream catResultsFile((std::string("Categories-") + params.outputFile).c_str());
		catResultsFile << "x = 0, y = 0, rule = B3/S23\n";
		catResultsFile << cats.CategoriesRLE();
		catResultsFile.close();
		
		
		if(params.fullReportFile.length() != 0)
		{
			std::ofstream allfile(params.fullReportFile.c_str());
			allfile << fullReport;
			allfile.close();
		}
	}
	
	void PrintTime(int sec)
	{
		int hr = sec / 3600;
		int min = (sec / 60) - hr * 60;
		int secs = sec - 3600 * hr - 60 * min;
		std::cout << hr << ":"  << min << ":" << secs;
		return;
		
	}
	void IncreaseIndexAndReport()
	{
		counter++; 
		
		if(counter == 0)
		{
			idx += 65536;
			
			if(idx % (1048576) == 0)
			{
				if((double)(clock() - current) / CLOCKS_PER_SEC > 10)
				{
					current = clock();
					Report();
				}
			}
		}
		
	}
	
	int ValidateMinWidthHeight()
	{
		int minX = iters[0]->curx;
		int minY = iters[0]->cury;
		int maxX = iters[0]->curx;
		int maxY = iters[0]->cury;
		
		for(int i = 1; i < numIters; i++)
		{
			if(iters[i]->curx > maxX)
				maxX = iters[i]->curx;
			
			if(iters[i]->cury > maxY)
				maxY = iters[i]->cury;
				
			if(iters[i]->curx < minX)
				minX = iters[i]->curx;
			
			if(iters[i]->cury < minY)
				minY = iters[i]->cury;
		}

		if(maxX - minX >= params.maxW || maxY - minY >= params.maxH)
			return NO;
		else
			return YES;
	}
	
	int LastNonActiveGeneration()
	{
		int minIter = 1000000;
		
		for(int i = 0; i < numIters; i++)
		{
			int startGen = statexyGen[iters[i]->curs][(iters[i]->curx + 64) % 64][(iters[i]->cury + 64) % 64];
			if(startGen < minIter)
				minIter = startGen;
				
			if(minIter < params.startGen)
				break;
		}
		
		return minIter;
		
	}
	
	void PutItersState()
	{
		for(int i = 0; i < numIters; i++)
			PutState(iters[i]);
	}
	
	int CatalystCollide()
	{
		Run(1);

		for(int i = 0; i < numIters; i++)
		{
			if(Contains(GlobalState, targets[iters[i]->curs], iters[i]->curx, iters[i]->cury) == NO)
			{
				return YES;
			}
		}

		return NO;
	}
	
	void InitActivationCounters()
	{
		for(int i = 0; i < numIters; i++)
		{
			activated[i] = NO;
			absentCount[i] = 0;
		}
	}
	
	bool UpdateActivationCountersFail()
	{
		for(int j = 0; j < numIters; j++)
		{
			if(Contains(GlobalState, targets[iters[j]->curs], iters[j]->curx, iters[j]->cury) == NO)
			{
				activated[j] = YES;
				absentCount[j]++;
				
				if(absentCount[j] > maxSurvive[iters[j]->curs])
				{
					return true;
				}
			}
			else
			{
				absentCount[j] = 0;
			}
		}	

		return false;
			
	}
	
	bool FilterForCurrentGenFail()
	{
		for(int j = 0; j < targetFilter.size(); j++)
		{
			if(GlobalState->gen == params.filterGen[j] && Contains(GlobalState, targetFilter[j]) == NO)
			{
				return true;
			}
		}
		
		return false;
	}
	
	bool IsAllActivated()
	{
		for(int j = 0; j < numIters; j++)
		{	
			if(activated[j] == NO || absentCount[j] != 0)
			{
				return NO;
			}
		}
		
		return YES;
	}
	
	void PutCurrentState()
	{
		New();
			
		for(int j = 0; j < numIters; j++)
		{
			PutState(iters[j]);
		}
		
		PutState(pat);
	}
	
	bool ValidateFilters(const int& maxFilterGen)
	{
		for(int j = 0; j <= maxFilterGen; j++)
		{
			for(int k = 0; k < params.filterGen.size(); k++)
			{
				if(GlobalState->gen == params.filterGen[k] && Contains(GlobalState, targetFilter[k]) == NO)
					return false;
			}
			
			Run(1);
		}
		
		return true;
	}
};

int main (int argc, char *argv[]) 
{
	if(argc < 2)
	{
		std::cout << "Usage CatForce.exe <in file>";
		exit(0);
	}
	
	//LifeAPI initialization. 
	New();
	LifeState* init = NewState();
	LifeState* afterCatalyst = NewState();
	LifeState* catalysts = NewState();
	
	SearchSetup setup; 
	setup.Init(argv[1]);
	
	//optimization to use const in if - compiler should optimize and skip
	const bool validateWH = setup.params.maxW > 0 && setup.params.maxH > 0;
	const bool hasFilter = setup.params.targetFilter.size() > 0;
	const bool reportAll = setup.params.fullReportFile.length() != 0;
	const bool hasFilterDontReportAll = hasFilter && !reportAll;

	const int filterMaxGen = setup.FilterMaxGen();
	const int iterationMaxGen = setup.params.maxGen;
	const int numIters = setup.numIters;
	//Main loop of search on iters
	do{
		int valid = Validate(&setup.iters[0], numIters); 
		
		if(valid == NO)
			continue;
		
		//width-height validation enabled
		if(validateWH)
		{
			valid = setup.ValidateMinWidthHeight();
		}
		
		setup.IncreaseIndexAndReport();
		
		//Valid remains YES after width-height Validation
		if(valid == NO)
			continue;
		
		int minIter = setup.LastNonActiveGeneration();
		
		//Activation before first generation allowed to be activated
		if(minIter <  setup.params.startGen)
			continue;
		
		//Place catalysts first and check if they collide. 
		New();
		
		setup.PutItersState();
		
		if(setup.CatalystCollide() == YES)
			continue;
	
		PutState(setup.preIterated[minIter]);
		
		//Initial setup countters for absense and activation
		setup.InitActivationCounters();
		
		int surviveCount = 0;

		for(int i = minIter; i < iterationMaxGen; i++)
		{
			Run(1);	
			
			//Fail if some catalyst is idle for too long - updates the counters for them otherwise. 
			if(setup.UpdateActivationCountersFail())
				break;
				
			//const bool optimization - will skip this always. 
			if(hasFilterDontReportAll)
			{
				//Validate filters if any of them exist. Will validate on current gen of GlobalState
				if(setup.FilterForCurrentGenFail())
					break;
			}
			
			if(setup.IsAllActivated())
				surviveCount++;
			else
				surviveCount = 0;
			
			//If everything was actuvated and stable for stableInterval then report. 
			if(surviveCount >= setup.params.stableInterval)
			{
				bool valid = true; 
				
				//If has fitlter validate them;
				if(hasFilter)
				{
					setup.PutCurrentState();
					valid = setup.ValidateFilters(filterMaxGen);
				}
				
				//If all filters validated update results
				if(valid)
				{
					New();
					setup.PutItersState();
					ClearData(catalysts);
					ClearData(init);
					ClearData(afterCatalyst);
					
					Copy(catalysts, GlobalState, COPY);
					setup.PutCurrentState();
					Copy(init, GlobalState, COPY);
					Run(i - setup.params.stableInterval + 2);
					Copy(afterCatalyst, GlobalState, COPY);
					setup.cats.Add(init, afterCatalyst, catalysts);
					
					setup.result.append(GetRLE(GlobalState));
					setup.result.append("100$");
					setup.found++;
				}
				
				//if reportAll - ignore filters and update fullReport
				if(reportAll)
				{
					setup.PutCurrentState();
					
					setup.fullReport.append(GetRLE(GlobalState));
					setup.fullReport.append("100$");
				}
				break;
			}
		}
	}while(Next(&setup.iters[0], numIters, NO));
	
	//Print report one final time (update files with the final results). 
	setup.Report();
	
	printf("\n\nFINISH\n");
	clock_t end = clock();
	printf("Total elapsed time: %f seconds\n", (double)(end - setup.begin) / CLOCKS_PER_SEC);

	getchar();
}
