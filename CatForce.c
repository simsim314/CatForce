#include "LifeAPI.h"
#include <time.h>

int main (int argc, char *argv[]) 
{
	
printf("x = 0, y = 0, rule = B3/S23\n");
clock_t begin = clock();

	New();
   
   LifeState* blck =  NewState("2o$2o!");
   LifeState* pat =  NewState("b3o$bo$3o!");
   //LifeState* pat =  NewState("6b3o$6bo$5b3o15$2o$2o7b2o3b2o$9b2o3b2o!", -5, 0);

   int numIters = 2;
   
   LifeIterator *iters[numIters];
   LifeTarget* target = NewTarget(blck);
   int activated[numIters];
   
	for(int i = 0; i < numIters; i++)
		iters[i] = NewIterator(blck, -10, -10, 20, 20);
	
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
				if(Contains(GlobalState, target, iters[i]->curx, iters[i]->cury) == NO)
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
					if(Contains(GlobalState, target, iters[i]->curx, iters[i]->cury) == NO)
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
						if(Contains(GlobalState, target, iters[j]->curx, iters[j]->cury) == NO)
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
							if(Contains(GlobalState, target, iters[j]->curx, iters[j]->cury) != YES)
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