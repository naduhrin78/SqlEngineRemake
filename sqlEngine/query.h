//checks for higher order number
int highOrder(char* a, char* b)
{
	int len1 = 0, len2 = 0;
	for (int i = 0; a[i] != '\0' && a[i] != '.'; i++)
	{
		len1++;
	}
	for (int i = 0; b[i] != '\0' && b[i] != '.'; i++)
	{
		len2++;
	}
	
	if (len1 == len2)
		return 0;

	return (len1 > len2) ? 1 : 2;
}


bool GT(char* a, char* b, int order)
{
	if (order == 2)
		return true;

	else if (atof(a) == atof(b))
		return false;
	
	else if (order == 1)
		return false;

	else if(strcmp(a,b) < 0)
		return true;

	return false;
}

bool GE(char* a, char* b, int order)
{
	if (order == 2)
		return true;

	else if (order == 1)
		return false;

	else if (strcmp(a, b) <= 0)
		return true;

	return false;
}

bool LT(char* a, char* b, int order)
{
	if (order == 1)
		return true;

	else if (order == 2)
		return false;
	
	else if (strcmp(a, b) > 0)
		return true;

	return false;
}

bool LE(char* a, char* b, int order)
{
	if (order == 1)
		return true;

	else if (atof(a) == atof(b))
		return false;
	
	else if (order == 2)
		return false;
	
	else if (strcmp(a, b) >= 0)
		return true;

	return false;
}

bool EQ(char* a, char* b, int order)
{
	if (strcmp(a, b) == 0)
		return true;

	return false;
}

bool contains(char* small, char* big)
{
	int success = 0;

	for (int i = 0; big[i] != '\0'; i++)
	{
		int j;
		for (j = 0; small[j] != '\0'; j++)
		{
			if (small[j] != big[i + j])
			{
				success = 0;
				break;
			}
			success++;
		}
		if (j == success && success > 0)
			return true;
	}

	return false;
}

Query* commandExtractor(char* buffer)
{
	char word[20];
	int showindex = 0;
	int j = 0;
	int space = 0;
	Query* query = (Query*)malloc(sizeof(Query));

	query->scheme = extIndexQuery(buffer);
	int size = meta_data.varcount[query->scheme];
	query->show = (int*)malloc(sizeof(int)*size);

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

			if (!strcmp(word, "where") || !strcmp(word, "from"))
			{
				continue;
			}
			else if (space == 3)
			{
				if (buffer[i + 1] == '\0')
					query->command = -1;
			}
			else if (space == 7)
			{
				query->arg = (char*)malloc(sizeof(char)* 20);
				strcpy(query->arg, word);
			}
			else if (!strcmp(word, "GT"))
			{
				query->command = 0;
			}
			else if (!strcmp(word, "GE"))
			{
				query->command = 1;
			}
			else if (!strcmp(word, "LT"))
			{
				query->command = 2;
			}
			else if (!strcmp(word, "LE"))
			{
				query->command = 3;
			}
			else if (!strcmp(word, "EQ"))
			{
				query->command = 4;
			}
			else if (!strcmp(word, "MATCHES"))
			{
				query->command = 5;
			}
			else if (!strcmp(word, "CONTAINS"))
			{
				query->command = 6;
			}
			else if (space == 1)
			{
				if (!strcmp(word, "*"))
				{
					for (int k = 0; k < size; k++)
					{
						query->show[k] = k;
					}
					showindex = size;
				}
				else
					query->show[showindex++] = varIndex(query->scheme, word);
			}
			else if (space == 5)
			{
				query->check = varIndex(query->scheme, word);
			}
		}
	}
	query->count = showindex;
	return query;
}

char* extractData(Query* query, int i, int choose)
{
	return readScheme[query->scheme][i]->array[choose];
}

bool decide(Query* query, int i)
{
	bool checkTrue = true;
	
	char* comp = (char*)malloc(sizeof(20));

	int order;

	if (query->command != -1)
	{
		comp = extractData(query, i, query->check);

		if (query->command < 5)
			order = highOrder(query->arg, comp);
	}

	if (query->command == 0)
	{
		checkTrue = GT(query->arg, comp, order);
	}
	else if (query->command == 1)
	{
		checkTrue = GE(query->arg, comp, order);
	}
	else if (query->command == 2)
	{
		checkTrue = LT(query->arg, comp, order);
	}
	else if (query->command == 3)
	{
		checkTrue = LE(query->arg, comp, order);
	}
	else if (query->command == 4)
	{
		checkTrue = EQ(query->arg, comp, order);
	}
	else if (query->command == 5)
	{
		if (!strcmp(query->arg, comp))
			checkTrue = true;
		else
			checkTrue = false;
	}
	else if (query->command == 6)
	{
		checkTrue = contains(query->arg, comp);
	}

	if (checkTrue)
	{
		for (int j = 0; j < query->count; j++)
		{
			printf("%s ", extractData(query, i, query->show[j]));
		}
	}

	return checkTrue;
}

void performQuery(Query* query)
{
	printf("\n");
	bool checkTrue = false;

	int size = meta_data.sizeS[query->scheme];

	for (int i = 0; i < size; i++)
	{
		if (meta_data.presence[query->scheme][i] != -1)
			checkTrue = decide(query, i);

		if (checkTrue)
			printf("\n");

		checkTrue = false;
	}
	printf("\n");
}