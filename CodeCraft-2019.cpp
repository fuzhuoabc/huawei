#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <cstring> 
#include <algorithm>
#include <map>

using namespace std;

//type info//////////////////////////////////////////////////
typedef struct road
{
	short id,length,speed,channel,from,to,isDuplex;

}RoadInfo;

typedef struct cross
{
	short id,roadId0,roadId1,roadId2,roadId3;

}CrossInfo;

typedef struct car
{
	short id,from,to,speed,planTime;
	
	short actualTime;
	vector<int> path;                     //road path

}CarInfo;



//routie info//////////////////////////////////////////////////
#define MAX_CROSS_NUM 100
typedef int DATA_TYPE;  
const DATA_TYPE NO_EDGE = 10000000;  
vector<vector<DATA_TYPE> > gOriDistance;
vector<CarInfo*> gCarInfo;
map<int, RoadInfo*> gRoadInfo;  
vector<vector<vector<DATA_TYPE>> > gOriPath;    //node path


void getVisitPath(vector<vector<int> > path, vector<int> &visitPath, int startNode, int endNode)
{
    int k = path[startNode][endNode];
    if (k == -1)
    {
        return;
    }
    else
    {
        getVisitPath(path, visitPath, startNode, k);
        visitPath.push_back(k);
        getVisitPath(path, visitPath, k, endNode);
    }
}
void displayPath(vector<vector<DATA_TYPE> > distance, vector<vector<int> > path)
{
    for (size_t i = 0; i < path.size(); ++i)
    {
        for (size_t j = 0; j < path.size(); ++j)
        {

            if (i != j && distance[i][j] < NO_EDGE)
            {
                vector<int> visitPath;
                getVisitPath(path, visitPath, i, j);

                gOriPath[i][j][0]= i;
                for (size_t k = 0; k < visitPath.size(); ++k)
                {
                    //cout << visitPath[k] << "->";
                    gOriPath[i][j][k+1] = visitPath[k];
                }
                gOriPath[i][j][visitPath.size()+1] = j;
                //cout << j << endl;
            }
        }
        //cout << endl;
    }
}
// Floyd
vector<vector<DATA_TYPE> > floyd(vector<vector<RoadInfo*> > weights)
{
    int vertexNum = weights.size();
    vector<vector<DATA_TYPE> > distance; 

    distance.resize(vertexNum);
    for (int i = 0; i < vertexNum; i++)
    	distance[i].resize(vertexNum);
    for(int i = 0; i < vertexNum; i++)
    {
    	for(int j = 0; j < vertexNum; j++)
    	{
    		if(weights[i][j] == NULL)
    			distance[i][j] = NO_EDGE;
    		else
    			distance[i][j] = weights[i][j]->length;
    	}
    }

    vector<vector<int> > path(vertexNum, vector<int>(vertexNum, -1));  

    for (size_t k = 0; k < vertexNum; ++k)
    {
        for (size_t i = 0; i < vertexNum; ++i)
        {
            for (size_t j = 0; j < vertexNum; ++j)
            {
                if (distance[i][j] > distance[i][k] + distance[k][j])
                {
                    distance[i][j] = distance[i][k] + distance[k][j];
                    path[i][j] = k;
                }
            }
        }
    }
    displayPath(distance, path);
    return distance;
}

int get_total_time(vector<CarInfo*> carinfo, vector<vector<RoadInfo*>> adjmat)
{

}

int main(int argc, char *argv[])
{
    std::cout << "Begin" << std::endl;
	
	if(argc < 5){
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}
	
	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]); 
	std::string answerPath(argv[4]);
	
	std::cout << "carPath is " << carPath << std::endl;
	std::cout << "roadPath is " << roadPath << std::endl;
	std::cout << "crossPath is " << crossPath << std::endl;
	std::cout << "answerPath is " << answerPath << std::endl;
	
	cout<<"read cross info--------------------------------------------"<<endl;
	
	vector<vector<RoadInfo*>> AdjMatrix;
	
	fstream inpath(crossPath);
	if (!inpath.is_open()) 
	{
		cout << "Can not find " << crossPath << endl;
		exit(1);
	}
	string buff;
	int crossnum = 0;
	while (getline(inpath, buff)) 
	{
		if(buff[0] == '#')
			continue;
		crossnum++;
	}
	cout<<"cross number:"<<crossnum<<endl;

    /*vector initialize*/
	AdjMatrix.resize(crossnum);
	for(int i = 0; i < crossnum; i++)
		AdjMatrix[i].resize(crossnum);
	for(int i = 0; i < crossnum; i++)
	{
		for(int j = 0; j < crossnum; j++)
		{
			if(i == j)
			{
				RoadInfo *proadinfo = new RoadInfo;
				proadinfo->length = 0;
				AdjMatrix[i][j] = proadinfo;
			}
			else
		    	AdjMatrix[i][j] = NULL;
		}
	}

	gOriPath.resize(crossnum);
	for (int n = 0; n < crossnum; n++) 
		gOriPath[n].resize(crossnum); 
	for (int i = 0; i < crossnum; i++) 
		for (int j = 0; j < crossnum; j++) 
			gOriPath[i][j].resize(crossnum); 
    
	for(int i = 0; i < crossnum; i++)
		for(int j = 0; j < crossnum; j++)
			for(int k = 0; k < crossnum; k++)
		    	gOriPath[i][j][k] = -1;

	cout<<"read roadinfo--------------------------------------------"<<endl;
	fstream inroad(roadPath);
	if (!inroad.is_open()) 
	{
		cout << "Can not find " << roadPath << endl;
		exit(1);
	}

	while (getline(inroad, buff)) 
	{
		if(buff[0] == '#')
			continue;
		RoadInfo *proadinfo = new RoadInfo;
		sscanf((char *)buff.c_str(), "(%d, %d,%d, %d,%d, %d,%d)", &(proadinfo->id), &(proadinfo->length), &proadinfo->speed,&proadinfo->channel,
			                                                      &proadinfo->from, &proadinfo->to, &proadinfo->isDuplex);
		//cout<<proadinfo->id<<" "<<proadinfo->length<<endl;
		AdjMatrix[proadinfo->from-1][proadinfo->to-1] = proadinfo;
		if(proadinfo->isDuplex)
			AdjMatrix[proadinfo->to-1][proadinfo->from-1] = proadinfo;
		gRoadInfo.insert(pair<int, RoadInfo*>(proadinfo->id, proadinfo));  
	}

	cout<<gRoadInfo[5012]->to<<endl;

	// for(int i = 0; i < crossnum; i++)
	// {
	// 	for(int j = 0; j < crossnum; j++)
	// 	{
	// 		if(AdjMatrix[i][j] == NULL)
	// 			cout<<"NULL ";
	// 		else
	// 		    cout<<AdjMatrix[i][j]->length<<" ";
	// 	}
 //        cout<<endl;
	// }	

	gOriDistance = floyd(AdjMatrix);

	// for(int i = 0; i < crossnum; i++)
	// {
	// 	for(int j = 0; j < crossnum; j++)
	// 	{
	// 		cout<<i<<"->"<<j<<": ";
	// 		for(int k = 0; gOriPath[i][j][k+1] != -1; k++)
	// 		{
	// 			cout<<AdjMatrix[gOriPath[i][j][k]][gOriPath[i][j][k+1]]->id<<"->";
	// 		}
	// 		cout<<endl;
	// 	}
        
	// }

	cout<<"read car info--------------------------------------------"<<endl;	
	fstream incar(carPath);
	if (!incar.is_open()) 
	{
		cout << "Can not find " << roadPath << endl;
		exit(1);
	}
	while (getline(incar, buff)) 
	{
		if(buff[0] == '#')
			continue;
		CarInfo *pcarinfo = new CarInfo;
		sscanf((char *)buff.c_str(), "(%d, %d,%d, %d,%d)", &(pcarinfo->id), &(pcarinfo->from), &pcarinfo->to,&pcarinfo->speed,&pcarinfo->planTime);
		gCarInfo.push_back(pcarinfo);
	}

	cout<<"car number:"<<gCarInfo.size()<<endl;
	int from, to;
	for(int i = 0;i < gCarInfo.size(); i++)
	{
		from = gCarInfo[i]->from;
		to = gCarInfo[i]->to;
		cout<<gCarInfo[i]->id<<" "<<gCarInfo[i]->from<<" "<<gCarInfo[i]->to<<":";

		for(int k = 0; gOriPath[from-1][to-1][k+1] != -1; k++)
			gCarInfo[i]->path.push_back( AdjMatrix[ gOriPath[from-1][to-1][k] ][ gOriPath[from-1][to-1][k+1] ]->id );

	    for(int j = 0; j < gCarInfo[i]->path.size(); j++)
		 	cout<<gCarInfo[i]->path[j]<<"->";
		cout<<endl;
	}	
	
	
	// TODO:process
	cout<<"process--------------------------------------------"<<endl;	
	int totalTime;
	totalTime = get_total_time(gCarInfo, AdjMatrix);

	

	// TODO:write output file
	system("rm -rf ../config/answer.txt");
	system("touch ../config/answer.txt");

	fstream ofanswer(answerPath);
	ofanswer<<"#(carId,StartTime,RoadId...)"<<endl;



	
	
	return 0;
}