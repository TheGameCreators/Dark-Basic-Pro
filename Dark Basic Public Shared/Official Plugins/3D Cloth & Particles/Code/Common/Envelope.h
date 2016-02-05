#ifndef DBPROPHYSICS_ENVELOPE_H
#define DBPROPHYSICS_ENVELOPE_H

#include "LinkList.h"

#define ENV_TIME_TOLERANCE 0.00001f

enum eInterpMode{INTERP_LINEAR,INTERP_STEP,INTERP_BEZIER};

template<class T> class KeyFrame
{
public:
	KeyFrame(float t, T * val){time=t;value=*val;}
	float time;
	T value;	
};

template<class T> 
class Envelope
{
public:
	Envelope(T& def){defaultVal = new T(def);mode=INTERP_LINEAR;}
	~Envelope() 
	{
		delete defaultVal;
		defaultVal=0;
	}

	void getValue(float time, T* out)
	{	
		if(keys.gotoFirstItem())
		{
			if(time<=keys.getCurObject()->time)
			{
				*out = keys.getCurObject()->value;
				return;
			}

			do
			{
				if(keys.getCurLink()->next)
				{
					float nextTime = keys.getCurLink()->next->data->time;
					if(time<nextTime)
					{
						float lastTime = keys.getCurObject()->time;
						float t = (time-lastTime)/(nextTime-lastTime);
						if(mode==INTERP_LINEAR)
						{
							keys.getCurObject()->value.interpolateLinear(keys.getCurLink()->next->data->value,t,out);
						}
						return;
					}
				}	
				else
				{
					*out = keys.getCurObject()->value;
					return;
				}
			}
			while(keys.gotoNextItem());
			return;
		}	
		*out = *defaultVal;
	}

	int addKey(float time, T * value)
	{	
		if(keys.gotoFirstItem())
		{
			if(fabs(time-keys.getCurObject()->time)<ENV_TIME_TOLERANCE) 
				return -1;
			else if(time<keys.getCurObject()->time)
			{
				keys.Add(new KeyFrame<T>(time,value));
				return 0;
			}

			int count=0;
			do
			{
				if(!keys.getCurLink()->next)
				{
					//No more in list, so add at end
					keys.addAfter(new KeyFrame<T>(time,value),keys.getCurLink());
					return count;
				}

				if(fabs(time-keys.getCurLink()->next->data->time)<ENV_TIME_TOLERANCE) 
					return -1;
				else if(time<keys.getCurLink()->next->data->time)
				{
					//Next item is later than item we're adding
					keys.addAfter(new KeyFrame<T>(time,value),keys.getCurLink());
					return count;
				}
				count++;
			}
			while(keys.gotoNextItem());
		}

		keys.Add(new KeyFrame<T>(time,value));
		return 0;
	}

	bool deleteKey(int n)
	{
		if(keys.gotoFirstItem())
		{
			int count=0;
			do
			{
				if(count==n)
				{
					Link< KeyFrame<T> > * curLink = keys.getCurLink();
					delete curLink->data;
					curLink->data=0;
					keys.Delete(curLink);
					return true;
				}
				count++;
			}
			while(keys.gotoNextItem());
		}
		return false;
	}

	bool deleteKeyAtTime(float time)
	{
		if(keys.gotoFirstItem())
		{
			do
			{
				if(fabs(time-keys.getCurObject()->time)<ENV_TIME_TOLERANCE)
				{
					Link< KeyFrame<T> > * curLink = keys.getCurLink();
					delete curLink->data;
					curLink->data=0;
					keys.Delete(curLink);
					return true;
				}
			}
			while(keys.gotoNextItem());
		}
		return false;
	}

	int Count()
	{
		return keys.Count();
	}

	bool getKeyValue(int n, T* out)
	{
		if(keys.gotoFirstItem())
		{
			int count=0;
			do
			{
				if(count==n)
				{
					*out=keys.getCurObject()->value;
					return true;
				}
				count++;
			}
			while(keys.gotoNextItem());
		}
		return false;
	}

	bool getKeyTime(int n, float * out)
	{
		if(keys.gotoFirstItem())
		{
			int count=0;
			do
			{
				if(count==n)
				{
					*out=keys.getCurObject()->time;
					return true;
				}
				count++;
			}
			while(keys.gotoNextItem());
		}
		return false;
	}

	LinkedList< KeyFrame<T> > keys;
	T* defaultVal;
	eInterpMode mode;
};


#endif
