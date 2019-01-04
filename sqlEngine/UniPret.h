//parses given random scheme
void parseScheme(char* buffer)
{
	char word[20];
	int schemeIndex = meta_data.schemeIndex;
	int memberindex = 0;
	int switcher = 0;
	int j = 0;
	int space = 0;

	if (schemeIndex == 0)
	{
		initStruct();
	}
	else
	{
		resizeStruct();
	}

	materializeStruct(countVars(buffer));

	for (int i = 0; buffer[i] != '\0'; i++)
	{
		if (buffer[i] != ' ' && buffer[i] != ',' && buffer[i] != ':')
		{
			word[j++] = buffer[i];
		}
		if (buffer[i] == ' ')
		{
			space++;
		}
		if (buffer[i + 1] == ',' || buffer[i + 1] == ' ' || buffer[i + 1] == '\0' || buffer[i + 1] == ':')
		{
			word[j] = '\0';

			j = 0;

			if (!strcmp(word, "scheme"))
			{
				if (schemeIndex == 0)
					readScheme = (Scheme***)malloc(sizeof(Scheme)*(schemeIndex + 1));

				else
					readScheme = (Scheme***)realloc(readScheme, sizeof(Scheme)*(schemeIndex + 1));

				continue;
			}
			if (space == 1)
			{
				meta_data.schemeName[schemeIndex] = strdup(word);
			}
			if (space == 2)
			{
				if (!switcher)
				{
					meta_data.variables[schemeIndex][memberindex] = (char*)malloc(sizeof(char)* 20);
					meta_data.variables[schemeIndex][memberindex] = strdup(word);
					switcher = 1;
				}
				else
				{
					meta_data.datatypes[schemeIndex][memberindex] = (char*)malloc(sizeof(char)* 20);
					meta_data.datatypes[schemeIndex][memberindex++] = strdup(word);
					switcher = 0;
				}
			}
		}
	}
	meta_data.schemeIndex++;
}

//universal line interpreter
void UnilineInterpret(char* buffer)
{
	char word[30];
	int schemeIndex = meta_data.schemeIndex;
	char* param1 = (char*)malloc(sizeof(char)* 20);
	int j = 0;
	int space = 0;
	int import = 0, flush = 0,join = 0;

	for (int i = 0; buffer[i] != '\0'; i++)
	{
		if (buffer[i] != ' ')
		{
			word[j++] = buffer[i];
		}
		if (buffer[i] == ' ')
		{
			space++;
		}
		if (buffer[i + 1] == ' ' || buffer[i + 1] == '\0')
		{
			word[j] = '\0';

			j = 0;
			if (!strcmp(word, "select"))
			{
				performQuery(commandExtractor(buffer));
				break;
			}
			else if (!strcmp(word, "scheme"))
			{
				if (schemeIndex == 0)
				{
					meta_data.presence = (int**)malloc(sizeof(int));
					meta_data.sizeS = (int*)malloc(sizeof(int));
					meta_data.schemeExists = (bool*)malloc(sizeof(bool));
				}
				else
				{
					meta_data.presence = (int**)realloc(meta_data.presence, sizeof(int)*(schemeIndex + 1));
					meta_data.sizeS = (int*)realloc(meta_data.sizeS, sizeof(int)*(schemeIndex + 1));
					meta_data.schemeExists = (bool*)realloc(meta_data.schemeExists, sizeof(bool)*(schemeIndex + 1));
				}
				meta_data.sizeS[schemeIndex] = 0;
				meta_data.schemeExists[schemeIndex] = false;
				parseScheme(buffer);
				break;
			}
			else if (!strcmp(word, "import"))
			{
				import = 1;
				continue;
			}
			else if (!strcmp(word, "join"))
			{
				join = 1;
				continue;
			}
			else if (!strcmp(word, "flush"))
			{
				flush = 1;
				continue;
			}
			else if (space == 1)
			{
				strcpy(param1, word);
			}
			else if (space == 2)
			{

				int type = extractIndex(word);
				
				if (import)
				{
					importScheme(param1, type);
				}
				else if (flush)
					flushS(param1, type);

				else if (join)
					Unijoin(extractIndex(param1), type);
			}

		}
	}
}

