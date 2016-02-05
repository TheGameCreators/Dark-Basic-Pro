
template< class T, int DEFSZ> class ptrList
{
	typedef int(*SORTFUNC)(const T * ptrA, const T * ptrB);
public:
	ptrList():size(DEFSZ),numElements(0)
	{
		list = new T*[DEFSZ];
		if(list==0) throw "Couldn't create new ptrList";
	}

	~ptrList()
	{
		delete[] list;
		list=0;
	}

	int Count() {return numElements;}
	void Clear()
	{
		size = DEFSZ;
		numElements=0;
	}

    void deleteAndClear()
	{
		for(int i=0;i<numElements;i++)
		{
			delete list[i];
			list[i]=0;
		}
		Clear();
	}

	void Optimise()
	{
		if(DEFSZ>numElements) return;

		T** temp = new T*[numElements];
		if(list==0) throw "Couldn't create new ptrList";

		for(int i=0;i<numElements;i++)
		{
			temp[i] = list[i];
		}
		delete[] list;
		list = temp;
		size = numElements;
	}

	void Push(T* inst)
	{
		if(numElements==size)
		{
			T** temp = new T*[size*2];
			if(list==0) throw "Couldn't create new ptrList";

			for(int i=0;i<numElements;i++)
			{
				temp[i] = list[i];
			}
			delete[] list;
			list = temp;
			size*=2;
		}
		list[numElements] = inst;
		numElements++;
	}

	void Pop()
	{
		numElements--;
	}

	void Insert(int index, T* inst)
	{
		this.Push(list[numElements-1]);
		for(int i=index+1;i<numElements;i++)
		{
			list[i] = list[i-1];
		}
		list[index]=inst;
	}

	T* Remove(int index)
	{
		T* temp = list[index];
		for(int i=index+1;i<numElements;i++)
		{
			list[i-1] = list[i];
		}
		numElements--;
		return temp;
	}

	void DeleteAndRemove(int index)
	{
		T* temp = list[index];
		for(int i=index+1;i<numElements;i++)
		{
			list[i-1] = list[i];
		}
		numElements--;
		delete temp;
		temp=0;
	}


	T* operator [] (int index)
	{
		return list[index];
	}

	const T* operator [] (int index) const
	{
		return list[index];
	}


	ptrList& operator = (const ptrList& copy)
	{
		size = copy.size;
		numElements = copy.numElements;
		list = new T*[size];
		for(int i=0; i<numElements;i++)
		{
			list[i]=copy.list[i];
		}

		return *this;
	}
    
	void Sort(SORTFUNC func)
	{
		//The call back should return:
		// < 0     PtrA is less than PtrB
		//   0     PtrA is equal to PtrB
		// > 0     PtrA is greater than PtrB

		//The cast is a bit nasty, but it does the job and hides the mess here
		qsort(list,numElements,sizeof(T),(int (__cdecl *)(const void *,const void *))func);
	}


private:
	int size;
	int numElements;
	T ** list;
};