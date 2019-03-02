#include "global.h"

void helpmsg(const char* pgm)
{
	cout << "Suggested arguments:" << endl;
	cout << "> " << pgm << " ";
	cout << "-d 2 -f raw_data.txt -i index_file.idx -q query_data.txt -k 0.1 -m CTA" << endl;
	cout << "explanations:" << endl;
	cout << "-d: data dimensionality, e.g. 2" << endl;
	cout << "-f: a tab delimited file, format:" << endl;
	cout << "    id xmin ymin [zmin] xmax ymax [zmax]" << endl;
	cout << "-q: query file, format:" << endl;
	cout << "    id xmin ymin xmax ymax" << endl;
	cout << "-k: monochromatic reverse top-k" << endl;
	cout << "   >= 1: exact k" << endl;
	cout << "-i: index file" << endl;
	cout << "-m: method, e.g., CTA, CTA+, ACTA, ACTA*" << endl;
}

double dataSize(int objCnt, int dim)
{
	double tmpMemUsage = sizeof(double)* 2 * dim;
	return tmpMemUsage / MB;
}

int PointvsHS(const int& Dimen, const double hs[], vector<double>& tmpPT)
{
	bool isAbove = false;
	double sum = 0;
	for (int i = 0; i < Dimen; i++)
	{
		sum += hs[i] * tmpPT[i];
	}

	if (sum > hs[Dimen])
		return ABOVE;
	if (sum < hs[Dimen])
		return BELOW;

	return ON;
}

int MBRvsHS(const int& Dimen, const double hs[], const double MBR[], vector<string>& Comb)
{
	int numAbove = 0;
	int numBelow = 0;
	int numOn = 0;

	long int numOfVectices = 0;
	numOfVectices = Comb.size();

	for (int i = 0; i < numOfVectices; i++)
	{
		vector<double> tmpPt(Dimen, 0.0);
		for (int j = 0; j < Dimen; j++)
		{
			if (Comb[i][j] == '0')
			{
				tmpPt[j] = MBR[j];
			}
			else if (Comb[i][j] == '1')
			{
				tmpPt[j] = MBR[j + Dimen];
			}
		}

		int pos = PointvsHS(Dimen, hs, tmpPt);
		if (pos == ABOVE)
		{
			numAbove++;
		}
		if (pos == BELOW)
		{
			numBelow++;
		}
	}

	if (numAbove == numOfVectices)
		return ABOVE;

	if (numBelow == numOfVectices)
		return BELOW;

	return HP_OVERLAPPED;
}

void myitoa(unsigned long val, char* buf, unsigned radix)
{
	char *p;
	char *firstdig;
	char temp;
	unsigned digval;

	p = buf;
	firstdig = p;

	do
	{
		digval = (unsigned)(val%radix);
		val /= radix;

		if (digval > 9)
		{
			*p++ = (char)(digval - 10 + 'a');
		}
		else
		{
			*p++ = (char)(digval + '0');
		}
	} while (val > 0);

	*p-- = '\0';
	do{
		temp = *p;
		*p = *firstdig;
		*firstdig = temp;
		--p;
		++firstdig;
	} while (firstdig < p);
}

bool MBRisValid(const int &Dimen, const double hs[], const double mbr[], vector<string> &Comb)
{
	int numAbove = 0;
	int numBelow = 0;
	int numOn = 0;
	long int numOfVertices = Comb.size();
	if (Dimen != Comb[0].size())
	{
		cout << "Why?????" << endl;
		exit(0);
	}

	long int numOfDim = Dimen;

	for (int i = 0; i < numOfVertices; i++)
	{
		vector<double> tmpPT(numOfDim, 0.0);
		for (int j = 0; j < numOfDim; j++)
		{
			if (Comb[i][j] == '0')
				tmpPT[j] = mbr[j];
			if (Comb[i][j] == '1')
				tmpPT[j] = mbr[j + Dimen];
		}

		double sum = 0;
		for (int d = 0; d < numOfDim; d++)
		{
			sum += tmpPT[d];
		}

		if (sum > hs[Dimen])
		{
			numAbove++;
		}

		if (sum < hs[Dimen])
		{
			numBelow++;
		}
	}

	if (numAbove == numOfVertices)
		return false;
	if (numBelow == numOfVertices)
		return true;
    
    return false;
}

void genLenNBinStrs(long int len1, long int HammingDist, multimap<int, vector<char>>& binString)
{
	typedef multimap<int, vector<char>>::value_type VT;
	vector<char> hammingStr;
	if (HammingDist == 0)
	{
		for (long int i = 0; i < len1; i++)
		{
			hammingStr.push_back('0');
		}
		binString.insert(VT(0, hammingStr));
		return;
	}
	if (HammingDist == 1)
	{
		for (long int i = 0; i < len1; i++)
		{
			hammingStr.clear();
			for (long int j = 0; j < i; j++)
				hammingStr.push_back('0');
			hammingStr.push_back('1');
			for (long int j = i + 1; j < len1; j++)
				hammingStr.push_back('0');
			binString.insert(VT(1, hammingStr));
		}
		return;
	}
	GenString(len1, HammingDist, HammingDist - 1, 0, hammingStr, binString);
}

void GenString(long int stringLen, long int HammingDist, long int curLen, long int start, vector<char>& hammstr, multimap<int, vector<char>>& binString)
{
	typedef multimap<int, vector<char>>::value_type VT;

	if (curLen < 0)
		return;

	for (long int i = start; i < stringLen; i++)
	{
		for (long int j = start; j < i; j++)
			hammstr.push_back('0');
		hammstr.push_back('1');
		GenString(stringLen, HammingDist, curLen - 1, i + 1, hammstr, binString);

		if (curLen == 0)
		{
			for (long int j = i + 1; j < stringLen; j++)
			{
				hammstr.push_back('0');
			}
			vector<char> tmphamstr = hammstr;
			binString.insert(VT(HammingDist, tmphamstr));

			for (long int j = i + 1; j < stringLen; j++)
			{
				hammstr.pop_back();
			}
		}

		hammstr.pop_back();
		for (long int j = start; j < i; j++)
		{
			hammstr.pop_back();
		}
	}
}

void GenerateQuery(int D, int queries)
{
	set<int> uniqueQuery;
	srand(time(NULL));
	FILE* fp = fopen("D:\\MonoTopK\\src\\kSPR_Revision\\Query\\Query.txt", "w+");
	if (fp == NULL)
	{
		cout << "Open file error!" << endl;
		exit(0);
	}

	while (uniqueQuery.size() != queries)
	{
		double random_num = rand()*1.0 / RAND_MAX;
		if (uniqueQuery.find(int(random_num*D)) == uniqueQuery.end())
		{
			uniqueQuery.insert(int(random_num*D));
			fprintf(fp, "%d\n", int(random_num*D));
		}
	}

	fclose(fp);

}



void Generateallqueries(int D)
{
	FILE* fp = fopen("Queryall.txt", "w+");
	if (fp == NULL)
	{
		cout << "Open file error!" << endl;
		exit(0);
	}

	for (int i = 1; i <= D; i++)
	{
		fprintf(fp, "%d\n", i);
	}
	fclose(fp);
}

void removeSkylines(unordered_set<long int>& skylines, unordered_set<long int>& removeSL, unordered_set<long int>& ignoreRecords)
{
	for (auto iter = removeSL.begin(); iter != removeSL.end(); iter++)
	{
		ignoreRecords.insert(*iter);
		if (skylines.find(*iter) != skylines.end())
		{
			skylines.erase(*iter);
		}
		else
		{
			cout << "please check what happened!" << endl;
		}
	}
}

bool dominateRecords(double* a, double* b, const int dimen)
{
	for (int i = 0; i < dimen; i++)
	{
		if (a[i] < b[i])
			return false;
	}
	return true;
}

void parseRegions(string &strreg, vector<double>& regions)
{
	std::string delimiter = ";";

	size_t pos = 0;
	std::string token;
	while ((pos = strreg.find(delimiter)) != std::string::npos) {
		token = strreg.substr(0, pos);
		regions.push_back(atof(token.c_str()));
		strreg.erase(0, pos + delimiter.length());
	}
}
