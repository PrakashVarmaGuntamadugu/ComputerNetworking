
#include "route.h"

int main(){

	int linkState[MAX_ROUTERS][MAX_ROUTERS]; //	Declaring 2D array for store roue link state
	int rotuterName[MAX_ROUTERS];		//	Declaring 1D array for store and map router name
	int i;

	int2DArrayIni(MAX_ROUTERS,MAX_ROUTERS,linkState);	// initializing 2D array 
	intArrayIni(MAX_ROUTERS,rotuterName,-1);	// initializing 1D array 
	
	
	createLinkState(ROUTEINFORFILENAME,linkState,rotuterName);	//storing route link state form input file..
	noumberOfRouter=findNoumberOfRouter(rotuterName);		// finding out  number of routers

	//Use the Dijkstra’s algorithm to find the least cost path all the routers
	for(i=0;i<noumberOfRouter;i++){
		dijkstra(linkState,rotuterName,i);	// Use the Dijkstra’s algorithm to find the least cost path one of the router
	}
	distanceVector(linkState,rotuterName);	//Use the Distance Vector (DV) algorithm to find the least cost path all the routers
   
 
    return 0;
}


// find and print ,write out put for the Distance Vector (DV) algorithm 
void distanceVector(int linkState[MAX_ROUTERS][MAX_ROUTERS],int rotuterName[MAX_ROUTERS]){
	
	int i,j,k;
	int linkStateForDV[noumberOfRouter][noumberOfRouter];
	int linkStateForDVRouter[noumberOfRouter][noumberOfRouter];
	int outPutArry[noumberOfRouter];

	stateCopy(linkState,linkStateForDV);
	int a=2;
	//  Loop for Distance Vector until Convergence
	while(a!=0){
		a=0;
		for(i=0;i<noumberOfRouter;i++){
			for(j=0;j<noumberOfRouter;j++){
				for(k=0;k<noumberOfRouter;k++){
					if(linkStateForDV[i][k]==-1 &&linkState[i][j]!=-1 && linkStateForDV[j][k]!=-1){
						a++;
						linkStateForDV[i][k]=linkStateForDV[j][k]+linkState[i][j];
					}else if((linkStateForDV[i][k]>linkStateForDV[j][k]+linkState[i][j]) &&linkState[i][j]!=-1 && linkStateForDV[j][k]!=-1){
						linkStateForDV[i][k]=linkStateForDV[j][k]+linkState[i][j];
						a++;
					}
				}
			}

		}


	}
	// Loop to get all distance vectors form Convergence Vector
	for(i=0;i<noumberOfRouter;i++){
		int2DArrayIni(noumberOfRouter,noumberOfRouter,linkStateForDVRouter);
		distanceVectorForRoute(linkState,linkStateForDV,linkStateForDVRouter,i);	// get one of router distance vectors form Convergence Vector
		intArrayIni(noumberOfRouter,outPutArry,-1);
		distanceVectorOutPut(outPutArry,linkStateForDVRouter,i);	// arrance output
		// print and write out put one of router
		if(i==0){
			printAndWriteOutPut(outPutArry, rotuterName,2,i);
		}else{
			printAndWriteOutPut(outPutArry, rotuterName, 4,i);
		}	
		
	}
}

// create distance vectors for one of routers
void distanceVectorForRoute(int linkState[MAX_ROUTERS][MAX_ROUTERS],int linkStateForDV[noumberOfRouter][noumberOfRouter],int linkStateForDVRouter[noumberOfRouter][noumberOfRouter],int sourceIndex ){
	int i,r;
	for(i=0;i<noumberOfRouter;i++){
		if(i!=sourceIndex){
			for(r=0;r<noumberOfRouter;r++){
				if(linkState[sourceIndex][r]!=-1 && r!=sourceIndex){
					linkStateForDVRouter[i][r]=linkState[sourceIndex][r]+linkStateForDV[i][r];
				}
				
			}
		}
	}

}

void stateCopy(int linkState[MAX_ROUTERS][MAX_ROUTERS],int linkStateForDV[noumberOfRouter][noumberOfRouter]){

	int i,r;
	for(i=0;i<noumberOfRouter;i++){
		for(r=0;r<noumberOfRouter;r++){
			linkStateForDV[i][r]=linkState[i][r];
		}
	}
}

// output index array distance vectors one of routers
void distanceVectorOutPut(int outPutArry[noumberOfRouter],int linkStateForDVRouter[noumberOfRouter][noumberOfRouter],int sourceIndex){
	int i,r;
	int min, minIndex;
	outPutArry[sourceIndex]=-2;
	for(i=0;i<noumberOfRouter;i++){
		min=-1;
		minIndex=-1;
		if(i!=sourceIndex){
			for(r=0;r<noumberOfRouter;r++){
				if(linkStateForDVRouter[i][r]!=-1 && r!=sourceIndex && (linkStateForDVRouter[i][r]<min || min==-1)){
					min=linkStateForDVRouter[i][r];
					minIndex=r;
				}
				
			}
			outPutArry[i]=minIndex;
		}
	}
}


// find and print ,write out put for the Dijkstra’s algorithm  
void dijkstra(int linkState[MAX_ROUTERS][MAX_ROUTERS],int rotuterName[MAX_ROUTERS],int sourceIndex){

	int leastCost[noumberOfRouter];
	int leastCostPath[noumberOfRouter];
	int visitedNode[noumberOfRouter];
	int outPutArry[noumberOfRouter];
	int currentVisitIndex;
	
	intArrayIni(noumberOfRouter,leastCost,-1);	// initializing leastCost array 
	intArrayIni(noumberOfRouter,leastCostPath,-1);	// initializing leastCostPath array 
	intArrayIni(noumberOfRouter,visitedNode,-1);	// initializing visitedNode array 
	intArrayIni(noumberOfRouter,outPutArry,-1);	// initializing outPutArry array 
	
	//first source node change
	leastCost[sourceIndex]=0;	
	visitedNode[sourceIndex]=1;
	outPutArry[sourceIndex]=-2;
		
	
	currentVisitIndex=findNextVisitNode(linkState,visitedNode,leastCost,leastCostPath,sourceIndex);	// finding out next visit node
	while(currentVisitIndex!=-1){
		visitedNode[currentVisitIndex]=1;
		leastCostPath[sourceIndex]=-2;
		currentVisitIndex=findNextVisitNode(linkState,visitedNode,leastCost,leastCostPath,currentVisitIndex); // finding out next visit node
	}
	
	int i,tem;
	for(i=0;i<noumberOfRouter;i++){
		if(leastCostPath[i]==sourceIndex){
			outPutArry[i]=i;
		}
	}
	// arrange output array form dijkstra sorted path
	for(i=0;i<noumberOfRouter;i++){
		if(leastCostPath[i]!=sourceIndex && leastCostPath[i]!=-2 ){
			tem=leastCostPath[i];
			while(1){
				if(leastCostPath[tem]==sourceIndex){
					outPutArry[i]=outPutArry[tem];
					break;
				}
				tem=leastCostPath[tem];
			}
		}
	}
	// print and write output one  router
	if(sourceIndex==0){
		printAndWriteOutPut(outPutArry, rotuterName,1,sourceIndex);
	}else{
		printAndWriteOutPut(outPutArry, rotuterName, 3,sourceIndex);
	}	
	
}


// finding out next node and path modified  for the Dijkstra’s algorithm 
int findNextVisitNode(int linkState[MAX_ROUTERS][MAX_ROUTERS],int visitedNode[noumberOfRouter],int leastCost[noumberOfRouter],int leastCostPath[noumberOfRouter],int currentVistedIndex){
	
	int i;
	int nextVisitIndex=-1;
	for(i=0;i<noumberOfRouter;i++){
		// modified path cost using  Dijkstra’s algorithm 
		if(linkState[currentVistedIndex][i]!=-1 && visitedNode[i]!=1){
			if(leastCost[i]==-1 || (leastCost[i]>leastCost[currentVistedIndex]+linkState[currentVistedIndex][i])){
				leastCost[i]=linkState[currentVistedIndex][i]+leastCost[currentVistedIndex];
				leastCostPath[i]=currentVistedIndex;
			}
			
		}
		// find next visit node
		if((nextVisitIndex==-1 || (leastCost[nextVisitIndex]>leastCost[i]&&leastCost[i]!=-1)) && visitedNode[i]!=1 && linkState[currentVistedIndex][i]!=-1){
				nextVisitIndex=i;
		}
	}
	return nextVisitIndex;
}

// print and write out put for one router
void printAndWriteOutPut(int outPutArry[noumberOfRouter],int rotuterName[MAX_ROUTERS], int routingTypeForMessage,int sourceIndex){
	FILE *fp;
	
	int i;
	if(routingTypeForMessage==1){	// first router and Dijkstra’s algorithm 
		// opean log file
		if ((fp=fopen("DV.txt","w"))==NULL){
			printf("Error! log file open error");
			exit(1);         
		}
		printf("Using Dijkstra’s algorithm\n");
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		printf("\n");
		fprintf(fp, "Using Dijkstra’s algorithm\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	}else if(routingTypeForMessage==2){		// first router and Distance Vector (DV) algorithm 
		if ((fp=fopen("LS.txt","w"))==NULL){
			printf("Error! log file open error");
			exit(1);         
		}
		printf("using Distance-Vector algorithm\n");
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		printf("\n");
		fprintf(fp, "using Distance-Vector algorithm\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	}else if(routingTypeForMessage==3){		// other router and Dijkstra’s algorithm 
		if ((fp=fopen("DV.txt","a"))==NULL){
			printf("Error! log file open error");
			exit(1);         
		}
	}else if(routingTypeForMessage==4){		// other router and Distance Vector (DV) algorithm 
		if ((fp=fopen("LS.txt","a"))==NULL){
			printf("Error! log file open error");
			exit(1);         
		}
	}
	printf("forwarding table for %c\n",rotuterName[sourceIndex]);
	for(i=0;i<noumberOfRouter;i++){
		if(sourceIndex!=i){
			printf("%c (%c, %c)\n",rotuterName[i],rotuterName[sourceIndex],rotuterName[outPutArry[i]]);	//print route path
			fprintf(fp, "%c (%c, %c)\n",rotuterName[i],rotuterName[sourceIndex],rotuterName[outPutArry[i]]);//write route path
		}
	}
	printf("-------------\n");
	fprintf(fp, "-------------\n");
	fclose(fp);
}


// create roue link state form input file..
void createLinkState(char *routeFile,int linkState[MAX_ROUTERS][MAX_ROUTERS],int rotuterName[MAX_ROUTERS]){

	FILE *routeFD;
	char sourceName,destinationName;
	int cost;
	int sourceIndex,destinationIndex;
	// open cache stored file  
	if ((routeFD=fopen(routeFile,"r"))==NULL){
		printf("Error! opening file..");
		exit(1);       
	}
	
	// checking url is  cached or not
	while(!feof(routeFD)){
		fscanf(routeFD,"%c %c %d\n",&sourceName,&destinationName,&cost);
		sourceIndex=findAndAddRouteName(rotuterName,sourceName);
		destinationIndex=findAndAddRouteName(rotuterName,destinationName);
		linkState[sourceIndex][destinationIndex]=cost;
		linkState[destinationIndex][sourceIndex]=cost;
		linkState[sourceIndex][sourceIndex]=0;
		linkState[destinationIndex][destinationIndex]=0;
	}
	fclose(routeFD);

}


// add route name into the route name map array
int findAndAddRouteName(int *rotuterNameArr,char Name){
	int i;
	for(i=0;i<MAX_ROUTERS;i++){
		if(rotuterNameArr[i]==-1){
			rotuterNameArr[i]=Name;
			return i;
		}else if(rotuterNameArr[i]==Name){
			return i;
		}
	}
	return -1;
}

//  to find number of route
int findNoumberOfRouter(int *rotuterNameArr){
	int i;
	for(i=0;i<MAX_ROUTERS;i++){
		if(rotuterNameArr[i]==-1){
			return i;
		}
	}
	return -1;
}

// initializing 1D array 
void intArrayIni(int rows,int array[rows],int value){
	int i;
	for(i=0;i<rows;i++){
		array[i]=value;
	}
}

// initializing 2D array 
void int2DArrayIni(int rows,int cols,int array[rows][cols]){
	int i,r;
	for(i=0;i<rows;i++){
		for(r=0;r<cols;r++){
			array[i][r]=-1;
		}
	}
}

