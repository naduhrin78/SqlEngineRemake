//global variables
struct metadata
{
	char*** variables;
	char*** datatypes;
	char** schemeName;
	int schemeIndex = 0;
	bool* schemeExists;

	//array of size of all schemes
	int* sizeS;
	//number of vars in scheme
	int* varcount;

	//checks if that pointer is initialized
	int** presence;

}meta_data;


//defines arbitary structure
typedef struct Struct{
	char* name;
	char** array;
} Scheme;

typedef struct Query
{
	int* show;
	int count;
	int check;
	char* arg;
	int command;
	int scheme;
};

Scheme*** readScheme;

Scheme* createScheme(char** variables, char** datatypes, int count, char* name);
void importScheme(char* filename, int type);

void performQuery(Query* query);
Query* commandExtractor(char* buffer);
void flushS(char*filename, int type);
void Unijoin(int type1, int type2);

//counts number of variables
int countVars(char* buffer)
{
	int vars = 0;

	for (int i = 0; buffer[i] != '\0'; i++)
	{
		if (buffer[i] == ':')
			vars++;
	}

	return vars;
}

long int max(long int a, long int b)
{
	return (a > b) ? a : b;
}

//returns index of variable
int varIndex(int type, char* datatype)
{
	for (int i = 0; i < meta_data.varcount[type]; i++)
	{
		if (!strcmp(datatype, meta_data.variables[type][i]))
		{
			return i;
		}
	}

	return -1;
}

//initializes scheme for first time
void initStruct()
{
	int size = meta_data.schemeIndex + 1;
	meta_data.varcount = (int*)malloc(sizeof(int)*size);
	meta_data.schemeName = (char**)malloc(sizeof(char*)* size);
	meta_data.variables = (char***)malloc(sizeof(char**)* size);
	meta_data.datatypes = (char***)malloc(sizeof(char**)* size);
}

//resizes a table
void resizeStruct()
{
	int size = meta_data.schemeIndex + 1;
	meta_data.varcount = (int*)realloc(meta_data.varcount, sizeof(int)*size);
	meta_data.schemeName = (char**)realloc(meta_data.schemeName, sizeof(char*)*size);
	meta_data.variables = (char***)realloc(meta_data.variables, sizeof(char**)*size);
	meta_data.datatypes = (char***)realloc(meta_data.datatypes, sizeof(char**)* size);
}

//initializes a table
void materializeStruct(int count)
{
	int schemeIndex = meta_data.schemeIndex;
	meta_data.varcount[schemeIndex] = count;
	meta_data.variables[schemeIndex] = (char**)malloc(sizeof(char)* count);
	meta_data.datatypes[schemeIndex] = (char**)malloc(sizeof(char)* count);
	meta_data.schemeName[schemeIndex] = (char*)malloc(sizeof(char)* 20);
}

void initializePresence(int* array, int start, int len)
{
	for (int i = start; i < len; i++)
	{
		array[i] = -1;
	}
}

char* trimString(char*string)
{
	char* out = (char*)malloc(sizeof(char)* 100);

	int readIndex = 0, writeIndex;

	while (string[readIndex] == ' '){
		readIndex++;
	}

	for (writeIndex = 0; string[readIndex] != '\0'; readIndex++){
		if (string[readIndex] == ' ' && string[readIndex - 1] == ' '){
			continue;
		}
		out[writeIndex] = string[readIndex];
		writeIndex++;
	}
	out[writeIndex] = '\0';

	return out;
}

//returs highest index as length
int lengthofFile(char* filename)
{
	int highest = 0, temp = 0;

	char* buffer = (char*)malloc(sizeof(char)*BUFFER_SIZE);

	FILE* fin1 = fopen(filename, "r");

	fgets(buffer, sizeof(char)*BUFFER_SIZE, fin1);

	while (fgets(buffer, sizeof(char)*BUFFER_SIZE, fin1))
	{
		temp = atoi(buffer);
		if (temp > highest)
			highest = temp;
	}

	return highest;
}

//returns index at which scheme is located
int extractIndex(char* str)
{
	int schemeIndex = meta_data.schemeIndex;
	for (int i = 0; i < schemeIndex; i++)
	{
		char* schemeName = meta_data.schemeName[i];
		if (!strcmp(schemeName, str))
			return i;
	}

	return -1;
}

//returns scheme from query
int extIndexQuery(char* buffer)
{
	char word[20];
	int j = 0;
	int space = 0;

	for (int i = 0; buffer[i] != '\0'; i++)
	{
		if (buffer[i] != ' ' && buffer[i] != ',')
		{
			word[j++] = buffer[i];
		}
		if (buffer[i] == ' ')
		{
			space++;
		}
		if (buffer[i + 1] == ',' || buffer[i + 1] == ' ' || buffer[i + 1] == '\0')
		{
			word[j] = '\0';

			j = 0;

			if (space == 3)
			{
				return extractIndex(word);
			}
		}
	}
	return -1;
}


int* stringUnion(int type1, int type2)
{
	int v1 = meta_data.varcount[type1];
	int v2 = meta_data.varcount[type2];
	int count = v1 + v2;
	int* out = (int*)malloc(sizeof(int)*v2);
	int sizeS1 = meta_data.sizeS[type1];
	int sizeS2 = meta_data.sizeS[type2];
	int schemeIndex = meta_data.schemeIndex;
	int length = max(sizeS1, sizeS2);

	//resizes to add one join scheme array
	resizeStruct();

	//initializes its variables
	materializeStruct(count);

	//extends a new scheme
	readScheme = (Scheme***)realloc(readScheme, sizeof(Scheme)*(schemeIndex + 1));

	//gives a common name to joined scheme
	meta_data.schemeName[schemeIndex] = strdup("View");

	//extends new size var,existence to accomodate view
	meta_data.sizeS = (int*)realloc(meta_data.sizeS, sizeof(int)*(schemeIndex + 1));
	meta_data.schemeExists = (bool*)realloc(meta_data.schemeExists, sizeof(bool)*(schemeIndex + 1));
	meta_data.sizeS[schemeIndex] = length;

	for (int i = 0; i < v1; i++)
	{
		meta_data.datatypes[schemeIndex][i] = (char*)malloc(sizeof(char)* 20);
		meta_data.datatypes[schemeIndex][i] = strdup(meta_data.datatypes[type1][i]);
		meta_data.variables[schemeIndex][i] = (char*)malloc(sizeof(char)* 20);
		meta_data.variables[schemeIndex][i] = strdup(meta_data.variables[type1][i]);
	}

	int offset = 0,outi = 0;

	for (int j = v1; j < count; j++)
	{
		for (int k = 0; k < v1; k++)
		{
			if (!strcmp(meta_data.variables[type1][k], meta_data.variables[type2][j - v1]))
			{
				out[outi] = -1;
				break;
			}
		}

		if (out[outi] == -1)
		{
			outi++;
			offset++;
			continue;
		}
		else
		{
			out[outi] = j - offset;
			meta_data.datatypes[schemeIndex][j - offset] = (char*)malloc(sizeof(char)* 20);
			meta_data.datatypes[schemeIndex][j - offset] = strdup(meta_data.datatypes[type2][j - v1]);
			meta_data.variables[schemeIndex][j - offset] = (char*)malloc(sizeof(char)* 20);
			meta_data.variables[schemeIndex][j - offset] = strdup(meta_data.variables[type2][j - v1]);
		}
		outi++;
	}
	meta_data.varcount[schemeIndex] = count - offset;

	readScheme[schemeIndex] = (Scheme**)malloc(sizeof(Scheme)*length);

	for (int i = 0; i < length; i++)
		readScheme[schemeIndex][i] = createScheme(meta_data.variables[schemeIndex], meta_data.datatypes[schemeIndex], count - offset, "View");

	return out;
}
