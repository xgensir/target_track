#define WATCH_SPAN		64					// watch span in frame_period
#define BIRDS_N			48			// 12 CAMs, 4 each
#define TAR_N     5
#define TRACK_N   8
#define XSIZE_DIF 5
#define YSIZE_DIF 5
#define WEIGHT_DIF 10
#define SCORE 5
typedef struct _BIRD{		// store the attri of a bird
	int valid;
	int fn;
	unsigned int clk_low8;
	int x,y;
	int xe,ye;
	int xsiz,ysiz;
	int weight;			
	int shapeValue;						
	struct _BIRD *pExcld0;				// exclusive of another bird
	struct _BIRD *pExcld1;
}BIRD;

BIRD birdTable[WATCH_SPAN][BIRDS_N];

typedef	struct {
	BIRD *pbuf[WATCH_SPAN];		// ringbuf of birds
	int num[WATCH_SPAN];			// how many active bird at this frame
	int icurr,ilast;					
//	int fn_curr;
	int clk;									// given by clk-report
}BIRD_CTRL;

typedef struct{
	int x,y;
	int xe,ye;
//	int xsiz,ysiz;
	int weight;			
}TAR;
typedef struct {
	TAR tarlist[TAR_N];	
	int valid;
	int trackValue;	
	
}TRACK;
typedef struct{
TRACK trackTable[TRACK_N];//TRACK_N TRACK
int num[TRACK_N];					//how many targets at this track
int rank[TRACK_N];
int icurr;  						//current clk
}TRACK_CTRL;
void init_track(TRACK_CTRL *pTkc)
{
	int i;
	for(i=0;i<TRACK_N;i++)
	{
		pTkc->trackTable[i].trackValue=0;
		pTkc->trackTable[i].valid=0;
		pTkc->num[i]=0;
		pTkc->rank[i]=i;
	}
	pTkc->icurr=0;
	
}
int track_a_frame(TRACK_CTRL *pTkc,int frameNum,BIRD_CTRL *pbc)
{
	int i,j;
	int flag[TRACK_N]={0};
	if(pbc->num[frameNum]==0)
			return 0;
	//第一次遍历
	for(i=0;i<pbc->num[frameNum];i++)//遍历当前帧的每一个bird
	{
		if((pbc->pbuf[frameNum])[i].valid)//if this bird is valid
			{
				for(j=0;j<TRACK_N;j++)//遍历trackTable中的每个track
				{
					if(pTkc->trackTable[pTkc->rank[j]].valid)//if this track is valid
						{
							if(location_same(&(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]),pbc->pbuf[frameNum]+i))//if the same location
								{
									(pbc->pbuf[frameNum]+i)->valid=0;
									(pbc->pbuf[frameNum]+i)->pExcld0->valid=0;
									(pbc->pbuf[frameNum]+i)->pExcld1->valid=0;
									pTkc->trackTable[pTkc->rank[j]].trackValue+=(pbc->pbuf[frameNum]+i)->shapeValue;
									continue;
								}
								
						}
				}
			}
	}
	sort(pTkc);
	//第二次遍历
	for(i=0;i<pbc->num[frameNum];i++)//遍历当前帧的每一个bird
	{
		if((pbc->pbuf[frameNum])[i].valid)//if this bird is valid
			{
				for(j=0;j<TRACK_N;j++)//遍历trackTable中的每个track
				{
					if(flag[pTkc->rank[j]]==0)
						{
							if(pTkc->trackTable[pTkc->rank[j]].valid)//if this track is valid
								{
									if(target_same(&(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]),pbc->pbuf[frameNum]+i))//if the same location
										{
											flag[pTkc->rank[j]]=1;
											pTkc->trackTable[pTkc->rank[j]].trackValue+=(pbc->pbuf[frameNum]+i)->shapeValue;
											(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]).x=(pbc->pbuf[frameNum]+i)->x;
											(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]).xe=(pbc->pbuf[frameNum]+i)->xe;
											(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]).y=(pbc->pbuf[frameNum]+i)->y;
											(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]).ye=(pbc->pbuf[frameNum]+i)->ye;
											(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]).weight=(pbc->pbuf[frameNum]+i)->weight;
											 pTkc->num[pTkc->rank[j]]++;
											if(pTkc->num[pTkc->rank[j]]==TAR_N)
													pTkc->num[pTkc->rank[j]]-=TAR_N;
											continue;
										}
										
								}
						  else////////put this bird into new track
								{
									pTkc->trackTable[pTkc->rank[j]].valid=1;
									flag[pTkc->rank[j]]=1;
									pTkc->trackTable[pTkc->rank[j]].trackValue+=(pbc->pbuf[frameNum]+i)->shapeValue;
									(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]).x=(pbc->pbuf[frameNum]+i)->x;
									(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]).xe=(pbc->pbuf[frameNum]+i)->xe;
									(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]).y=(pbc->pbuf[frameNum]+i)->y;
									(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]).ye=(pbc->pbuf[frameNum]+i)->ye;
									(pTkc->trackTable[pTkc->rank[j]].tarlist[pTkc->num[pTkc->rank[j]]]).weight=(pbc->pbuf[frameNum]+i)->weight;
									pTkc->num[pTkc->rank[j]]++;
									if(pTkc->num[pTkc->rank[j]]==TAR_N)
											pTkc->num[pTkc->rank[j]]-=TAR_N;
									continue;
								}
					}
						
				}
			}
	}
	
	for(j=0;j<TRACK_N;j++)//遍历trackTable中的每个track
	{
		if(flag[j]==0)//this track at current frame no target,trackValue reduce score
			{
				if(pTkc->trackTable[j].valid)
					{
						pTkc->trackTable[j].trackValue-=SCORE;
						if(pTkc->trackTable[j].trackValue<=0)
							{
								pTkc->trackTable[j].trackValue=0;
								pTkc->trackTable[j].valid=0;
							}
					}
			}
		else
			{
				flag[j]=0;//make flag clear
			}
	}
	sort(pTkc);
}
void sort(TRACK_CTRL *pTkc)
{
	int i,j,temp;
	for(i=0;i<TRACK_N;i++)
	{
		for(j=TRACK_N;j>i;j--)////冒泡排序
		{
			if(pTkc->trackTable[pTkc->rank[j]].trackValue>pTkc->trackTable[pTkc->rank[j-1]].trackValue)
				{
					temp=pTkc->rank[j];
					pTkc->rank[j]=pTkc->rank[j-1];
					pTkc->rank[j-1]=temp;
				}
		}
 }
}
bool location_same(TAR *first,BIRD *second)
{
	if((first->x<second->xe)&&(first->xe>second->x)&&(first->y<second->ye)&&(first->ye>second->y))
		{
			return true;
		}
		return false;
}
bool target_same(TAR *first,BIRD *second)
{
	int x1,y1,x2,y2
	x1=first->xe-first->x;
	y1=first->ye-first->y;
	x2=second->xe-second->x;
	y2=second->ye-second->y;
	if((abs(x1-x2)<XSIZE_DIF)&&(abs(y1-y2)<YSIZE_DIF)&&(abs(first->weight-second->weight)<WEIGHT_DIF))
		{
			return true;
		}
		return false;
}