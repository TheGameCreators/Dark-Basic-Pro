/*
void B_BobExist(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0 && bob[bobindex].deletecount==0)//V107d delete count check added
			*((int*)*(glob.programpointer+0)) = 1;
		else
			*((int*)*(glob.programpointer+0)) = 0;
	}
	glob.programpointer+=2;
}

void B_BobX(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0)
			*((int*)*(glob.programpointer+0)) = bob[bobindex].x;
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=2;
}

void B_BobY(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0)
			*((int*)*(glob.programpointer+0)) = bob[bobindex].y;
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=2;
}

void B_BobOffsetX(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0)
			*((int*)*(glob.programpointer+0)) = bob[bobindex].hspotx;
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=2;
}

void B_BobOffsetY(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0)
			*((int*)*(glob.programpointer+0)) = bob[bobindex].hspoty;
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=2;
}

void B_BobWidth(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0)
		{
			int imageindex = bob[bobindex].image;
			*((int*)*(glob.programpointer+0)) = (int)(image[imageindex].width*bob[bobindex].xscale);
		}
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=2;
}

void B_BobHeight(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0)
		{
			int imageindex = bob[bobindex].image;
			*((int*)*(glob.programpointer+0)) = (int)(image[imageindex].height*bob[bobindex].yscale);
		}
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=2;
}

void B_BobImage(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0)
			*((int*)*(glob.programpointer+0)) = bob[bobindex].imageid;
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=2;
}

void B_BobScaleX(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0)
			*((int*)*(glob.programpointer+0)) = (int)(bob[bobindex].xscale*100.0f);
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=2;
}

void B_BobScaleY(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0)
			*((int*)*(glob.programpointer+0)) = (int)(bob[bobindex].yscale*100.0f);
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=2;
}

void B_BobMirrored(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0)
		{
			if(bob[bobindex].mirrored==FALSE)
				*((int*)*(glob.programpointer+0)) = 0;
			else
				*((int*)*(glob.programpointer+0)) = 1;
		}
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=2;
}

void B_BobFlipped(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex!=0)
		{
			if(bob[bobindex].flipped==FALSE)
				*((int*)*(glob.programpointer+0)) = 0;
			else
				*((int*)*(glob.programpointer+0)) = 1;
		}
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=2;
}

int ReturnBobOverlapResult(int bobindex, int bobindexB)
{
	if(bob[bobindex].userdefined==TRUE && bob[bobindexB].userdefined==TRUE)
	{
		int imageA = bob[bobindex].image;
		int imageB = bob[bobindexB].image;
		if(imageA>0 && imageB>0)
		{
			int widthA=(int)(image[imageA].width*bob[bobindex].xscale);
			int heightA=(int)(image[imageA].height*bob[bobindex].yscale);
			int widthB=(int)(image[imageB].width*bob[bobindexB].xscale);
			int heightB=(int)(image[imageB].height*bob[bobindexB].yscale);
			int Ax1=bob[bobindex].x-bob[bobindex].hspotx;
			int Ay1=bob[bobindex].y-bob[bobindex].hspoty;
			int Ax2=(bob[bobindex].x-bob[bobindex].hspotx)+widthA;
			int Ay2=(bob[bobindex].y-bob[bobindex].hspoty)+heightA;
			int Bx1=bob[bobindexB].x-bob[bobindexB].hspotx;
			int By1=bob[bobindexB].y-bob[bobindexB].hspoty;
			int Bx2=(bob[bobindexB].x-bob[bobindexB].hspotx)+widthB;
			int By2=(bob[bobindexB].y-bob[bobindexB].hspoty)+heightB;

			// Rough collision bound box check
			if(Ax1<=Bx2 && Ay1<=By2 && Ax2>Bx1 && Ay2>By1)
			{
				// Engage fine collision detection by masking both bobs in a bitcheck
				return DB_ReturnImageMaskCollision(imageA,Ax1,Ay1,Ax2,Ay2,imageB,Bx1,By1,Bx2,By2,bob[bobindex].flipped,bob[bobindex].mirrored,bob[bobindexB].flipped,bob[bobindexB].mirrored);
			}
			else
				return 0;
		}
		else
			return 0;
	}
	else
		return 0;
}

void B_BobHitLLL(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex>0)
		{
			int overlap=0;
			int bobBid = *((int*)*(glob.programpointer+2));
			if(bobBid>0 && bobBid<=TABLEREFMAX)
			{
				// Check Specific Bob
				int bobBindex = bobtable[bobBid];
				if(bobBindex>0)
				{
					if(bob[bobBindex].refbobid>0)
						overlap = ReturnBobOverlapResult(bobindex, bobBindex);
					else
						overlap = 0;
				}
				else
					RunTimeError(ERROR_BOBNOTEXIST);
			}
			else
			{
				// Check All bobs
				int collisionwith=0;
				int bobBindex = BobStart;
				while(bobBindex!=-1)
				{
					if(bobBindex!=bobindex)
					{
						if(ReturnBobOverlapResult(bobindex, bobBindex)==1)
						{
							if(bob[bobBindex].refbobid>0)
								collisionwith=bob[bobBindex].refbobid;
							break;
						}
					}
					bobBindex = bob[bobBindex].next;
				}
				overlap = collisionwith;
			}

			if(overlap>0 && bob[bobindex].bobhitoverlapstore==0)
				*((int*)*(glob.programpointer+0)) = overlap;
			else
				*((int*)*(glob.programpointer+0)) = 0;

			bob[bobindex].bobhitoverlapstore = overlap;
		}
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=3;
}

void B_BobCollisionLLL(void)
{
	int bobid = *((int*)*(glob.programpointer+1));
	if(bobid<=0 || bobid>TABLEREFMAX)
		RunTimeError(ERROR_BOBILLEGALNUMBER);
	else
	{
		int bobindex = bobtable[bobid];
		if(bobindex>0)
		{
			int overlap=0;
			int bobBid = *((int*)*(glob.programpointer+2));
			if(bobBid>0 && bobBid<=TABLEREFMAX)
			{
				// Check Specific Bob
				int bobBindex = bobtable[bobBid];
				if(bobBindex>0)
				{
					if(bob[bobBindex].refbobid>0)
						*((int*)*(glob.programpointer+0)) = ReturnBobOverlapResult(bobindex, bobBindex);
					else
						*((int*)*(glob.programpointer+0)) = 0;
				}
				else
					RunTimeError(ERROR_BOBNOTEXIST);
			}
			else
			{
				// Check All bobs
				int collisionwith=0;
				int bobBindex = BobStart;
				while(bobBindex!=-1)
				{
					if(bobBindex!=bobindex)
					{
						if(ReturnBobOverlapResult(bobindex, bobBindex)==1)
						{
							if(bob[bobBindex].refbobid>0)
								collisionwith=bob[bobBindex].refbobid;
							break;
						}
					}
					bobBindex = bob[bobBindex].next;
				}
				*((int*)*(glob.programpointer+0)) = collisionwith;
			}
		}
		else
			RunTimeError(ERROR_BOBNOTEXIST);
	}
	glob.programpointer+=3;
}
*/