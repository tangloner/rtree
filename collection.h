/* ----------------------------------------------------------------------------
This library contains a number of collection classes:
* they all store (void*) that point to objects
* user of these classes needs to provide data types compatible to void*, or
type cast them (e.g. (void*)a)
---------------------------------------------------------------------------- */

#ifndef COLLECTION_DEFINED
#define COLLECTION_DEFINED

#define INITSIZE 1000000			// default collection size
#define EXTENSION 1000000		// default	extension size if the collection object overflows


namespace Collection
{
	class Array;
	class Stack;
	class Queue;
	class Hash;
	class HashReader;
	class Set;
	class BinHeap;

	class Array
	{
		// data members
	protected:
		void**		m_p;		// pointer to an array of void*
		int			m_lim;		// the array size (max No. of items)
		int			m_max;		// the max current usage of array space
		int			m_ext;		//	the extension space if the array is overflow;

	public:
		Array(const int a_max = INITSIZE, const int a_ext = EXTENSION);
		Array(const Array& a_v);
		virtual ~Array();

		//	update
		virtual int append(void* a_p);
		virtual int remove(void* a_p);
		virtual int removeAt(int i);
		virtual int replaceAt(const int a_i, void* a_p);
		virtual int clean();
		virtual int partialsort(const int a_start, const int a_end, int(*a_compare)(const void* a_p0, const void* a_p1) = 0);
		virtual int sort(int(*a_compare)(const void* a_p0, const void* a_p1) = 0);
		virtual int reverse();
		virtual int	trim(const int _k);
		virtual int copy(const Array& a_array);
		virtual void removeDuplicate(int(*a_compare)(const void* a_p0, const void* a_p1) = 0);

		// search
		virtual int size() const;
		virtual void* get(const int a_i) const;
		virtual void* operator[](const int a_i) const;
		virtual int find(void* a_p, int(*a_compare)(const void* a_p0, const void* a_p1) = 0) const;
		virtual int binSearch(void*a_p, int(*a_compare)(const void* a_p0, const void* a_p1) = 0) const; // this assumes the array is sorted!!!
		virtual bool operator==(const Array& a_array) const;
	};

	class Stack :protected Array
	{
	public:
		//constructor / destructor
		Stack(const int a_max = INITSIZE, const int a_ext = EXTENSION);
		Stack(const Stack& a_s);
		virtual ~Stack();

		// update
		virtual int push(void* a_p);
		virtual void* pop();

		// search
		virtual int height() const;
		virtual void* top() const;
		virtual bool isEmpty() const;
		virtual int size() const;
	};

	class Queue : protected Array
	{
	protected:
		int		m_head;		//	head position
	public:
		Queue(const int a_max = INITSIZE, const int a_ext = EXTENSION);
		Queue(const Queue& a_q);
		virtual ~Queue();

		// update
		virtual int enqueue(void* a_p);
		virtual void* dequeue();

		// search
		virtual int length() const;
		virtual void* head() const;
		virtual bool isEmpty() const;
		virtual void* operator[](const int a_i) const; // added by tangloner for efficient queue traverse
	};


	class Hash
	{
		friend class HahReader;

	public:
		class HashEntry
		{
		public:
			const int	m_key;
			void*		m_p;
		public:
			HashEntry(const int a_key, void* a_p) :m_key(a_key), m_p(a_p){};
			virtual ~HashEntry(){};
			static int comparekey(const void* a_p0, const void* a_p1)
			{
				HashEntry* p0 = *(HashEntry**)a_p0;
				HashEntry* p1 = *(HashEntry**)a_p1;
				if (p0->m_key == p1->m_key) return 0;
				return -1;
			};
		};
		// data member
	public:
		const int	m_slot;
		Array**		m_a;
		int			m_size;

	public:
		Hash(const int a_max = INITSIZE);
		Hash(const Hash& a_h);
		virtual ~Hash();

		// update
		virtual int put(const int a_key, void* a_p);
		virtual int remove(const int a_key);
		virtual int replace(const int a_key, void* a_p);
		virtual int clean();

		//	search
		virtual int size() const;
		virtual void* get(const int) const;
	};

	class HashReader
	{
	protected:
		const Hash&	m_hash;
		int			m_curarray;
		int			m_curcontent;

	public:
		HashReader(const Hash& a_hash);
		virtual	~HashReader();

		void next();
		void first();

		bool isEnd() const;
		int getKey() const;
		void* getVal() const;
	};

	class Set : protected Hash
	{
	public:
		Set(const int a_max = INITSIZE);
		virtual	~Set();

		virtual int insert(void* a_p);
		virtual int remove(void* a_p);

		virtual int size() const;
		virtual bool in(void*) const;
	};


	class BinHeap
	{
		friend class BinHeapReader;
		class BinHeapEntry
		{
		public:
			void*			m_p;
			BinHeapEntry*	m_left;
			BinHeapEntry*	m_right;
		public:
			BinHeapEntry(void* a_p) :m_p(a_p), m_left(0), m_right(0){};
			virtual ~BinHeapEntry(){};
		};

	protected:
		int(*m_compare)(const void*, const void*);
		BinHeapEntry*		m_root;
		Queue				m_reuse;
		int					m_count;

	public:
		BinHeap(int(*a_compare)(const void*, const void*));
		virtual ~BinHeap();

		virtual	void insert(void* a_p);
		virtual void* removeTop();
		virtual void clean();

		virtual void* top() const;
		virtual int size() const;
		virtual bool isEmpty() const;
		virtual bool exist(void* a_p) const;

	};

	class BinHeapReader
	{
	protected:
		const BinHeap&	m_heap;
		Stack			m_stack;

	public:
		BinHeapReader(const BinHeap& a_heap);
		virtual	~BinHeapReader();

		void next();
		void first();

		bool isEnd() const;
		void* get() const;
	};
}

using namespace Collection;

#endif // COLLECTION_DEFINED