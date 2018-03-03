
#include <stdio.h>
#include <stdlib.h>
 


#define MAX_ROUTERS 10
#define ROUTEINFORFILENAME "router.txt"

int noumberOfRouter;

void intArrayIni(int rows,int array[rows],int value);
void int2DArrayIni(int rows,int cols,int array[rows][cols]);
void createLinkState(char *routeFile,int linkState[MAX_ROUTERS][MAX_ROUTERS],int rotuterName[MAX_ROUTERS]);
int findAndAddRouteName(int *rotuterNameArr,char Name);
int findNoumberOfRouter(int *rotuterNameArr);
void dijkstra(int linkState[MAX_ROUTERS][MAX_ROUTERS],int rotuterName[MAX_ROUTERS],int sourceIndex);
int findNextVisitNode(int linkState[MAX_ROUTERS][MAX_ROUTERS],int visitedNode[noumberOfRouter],int leastCost[noumberOfRouter],int leastCostPath[noumberOfRouter],int currentVistedIndex);
void stateCopy(int linkState[MAX_ROUTERS][MAX_ROUTERS],int linkStateForDV[noumberOfRouter][noumberOfRouter]);
void distanceVector(int linkState[MAX_ROUTERS][MAX_ROUTERS],int rotuterName[MAX_ROUTERS]);
void distanceVectorForRoute(int linkState[MAX_ROUTERS][MAX_ROUTERS],int linkStateForDV[noumberOfRouter][noumberOfRouter],int linkStateForDVRouter[noumberOfRouter][noumberOfRouter],int sourceIndex );
void distanceVectorOutPut(int outPutArry[noumberOfRouter],int linkStateForDVRouter[noumberOfRouter][noumberOfRouter],int sourceIndex);
void printAndWriteOutPut(int outPutArry[noumberOfRouter],int rotuterName[MAX_ROUTERS], int routingTypeForMessage,int sourceIndex);
