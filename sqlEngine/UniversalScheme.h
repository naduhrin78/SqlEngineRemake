

//creates parsed type scheme char** variables, char** datatype, int count
Scheme* createScheme(char** variables, char** datatypes, int count, char* name)
{
	Scheme* scheme = (Scheme*)malloc(sizeof(Scheme));
	scheme->name = strdup(name);
	scheme->array = (char**)malloc(sizeof(char)*count);
	for (int i = 0; i < count; i++){
		scheme->array[i] = (char*)malloc(sizeof(char)* 20);
	}
	return scheme;
}

void parseRandCsv(Scheme* scheme, FILE* fp, char* buffer, int type)
{
	//tok splits using , delimiter
	char* tok = strtok(buffer, ",");

	//intitalizes datatype, names are initialized already
	for (int i = 0; i < meta_data.varcount[type]; i++)
	{
		scheme->array[i] = strdup(tok);

		tok = strtok('\0', ",");
	}
}

void importScheme(char* filename, int type)
{
	char* buffer = (char*)malloc(sizeof(char)*BUFFER_SIZE);

	//length = SET_SIZE
	int length = lengthofFile(filename);

	int startPoint = meta_data.sizeS[type];

	if (meta_data.sizeS[type] < length)
		meta_data.sizeS[type] = length;

	int size = meta_data.sizeS[type];

	if (!meta_data.schemeExists[type])
	{
		meta_data.presence[type] = (int*)malloc(sizeof(int)*size);
		readScheme[type] = (Scheme**)malloc(sizeof(Scheme)*size);
		meta_data.schemeExists[type] = true;
	}
	else
	{
		meta_data.presence[type] = (int*)realloc(meta_data.presence[type], sizeof(int)*size);
		readScheme[type] = (Scheme**)realloc(readScheme[type], sizeof(Scheme)*size);
	}

	initializePresence(meta_data.presence[type], startPoint, meta_data.sizeS[type]);

	FILE* fin = fopen(filename, "r");

	fgets(buffer, sizeof(char)*BUFFER_SIZE, fin);


	while (fgets(buffer, sizeof(char)*BUFFER_SIZE, fin))
	{
		int index = atoi(buffer);
		index--;

		readScheme[type][index] = createScheme(meta_data.variables[type], meta_data.datatypes[type], meta_data.varcount[type], meta_data.schemeName[type]);

		meta_data.presence[type][index] = index;
		parseRandCsv(readScheme[type][index], fin, buffer, type);
	}
	
	fclose(fin);
}

void flushS(char* filename, int type)
{
	char* buffer = (char*)malloc(sizeof(char)*BUFFER_SIZE);

	FILE* fout = fopen(filename, "w");

	int numvars = meta_data.varcount[type];

	for (int i = 0; i < meta_data.sizeS[type]; i++)
	{

		if (meta_data.presence[type][i] != -1)
		{
			for (int j = 0; j < numvars; j++)
			{
				fprintf(fout, "%s", readScheme[type][i]->array[j]);

				if (j != numvars - 1)
				{
					fprintf(fout, ",");
				}
			}
		}
		fprintf(fout, "\n");
	}

	initializePresence(meta_data.presence[type], 0, meta_data.sizeS[type]);
	meta_data.schemeExists[type] = false;
	free(readScheme[type]);
	fclose(fout);
}

//joins 2 given schemes
void Unijoin(int type1, int type2)
{
	int i = 0;
	int schemeIndex = meta_data.schemeIndex;
	int sizeS1 = meta_data.sizeS[type1];
	int sizeS2 = meta_data.sizeS[type2];
	int length = max(sizeS1, sizeS2);
	meta_data.presence[schemeIndex] = (int*)malloc(sizeof(int)*length);

	if (sizeS1 < sizeS2)
	{
		meta_data.presence[type1] = (int*)realloc(meta_data.presence[type1], sizeof(int)*length);
		initializePresence(meta_data.presence[type1], sizeS1, length);
	}
	//presence of view
	else if (sizeS2 < sizeS1)
	{
		meta_data.presence[type2] = (int*)realloc(meta_data.presence[type2], sizeof(int)*length);
		initializePresence(meta_data.presence[type2], sizeS2, length);
	}

	initializePresence(meta_data.presence[schemeIndex], 0, length);

	int* arr = stringUnion(type1, type2);

	for (int i = 0; i < sizeS1 + sizeS2; i++)
	{

		if (meta_data.presence[type1][i] != -1 && meta_data.presence[type2][i] != -1)
			meta_data.presence[schemeIndex][i] = i;

		if (meta_data.presence[type1][i] != -1 && i < sizeS1)
		{
			for (int j = 0; j < meta_data.varcount[type1]; j++)
			{
					readScheme[schemeIndex][i]->array[j] = strdup(readScheme[type1][i]->array[j]);
			}
		}

		int v2 = meta_data.varcount[type2];
		int v1 = meta_data.varcount[type1];

		if (meta_data.presence[type2][i] != -1 && i < sizeS2)
		{
			for (int j = 0; j < v2; j++)
			{
				if (arr[j] != -1)
				{
						readScheme[schemeIndex][i]->array[arr[j]] = readScheme[type2][i]->array[j];
				}
			}
		}

	}
	meta_data.schemeIndex++;
}
