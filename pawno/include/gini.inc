/*
 * g_ini.inc
 * Version: 1.0 (last updated on 5th July, 2016)
 * By Gammix
 *
 * FUNCTIONS:
 * native INI_Get(const file[], const field[], dest[], maxlength = sizeof (dest));
 * native INI_GetInt(const file[], const field[]);
 * native Float:INI_GetFloat(const file[], const field[]);
 * native INI_Set(const file[], const field[], const value[]);
 * native INI_SetInt(const file[], const field[], value);
 * native INI_SetFloat(const file[], const field[], Float:value);
 * native INI_UnSet(const file[], const field[]);
 *
 */


#if !defined INI_FILE_TIMEOUT
	#define INI_FILE_TIMEOUT (1000)
#endif

#if !defined INI_MAX_INSTANCES
	#define INI_MAX_INSTANCES (32)
#endif

#if !defined INI_MAX_FIELDS
	#define INI_MAX_FIELDS (64)
#endif

#if !defined INI_MAX_FIELD_NAME
	#define INI_MAX_FIELD_NAME (32)
#endif

#if !defined INI_MAX_FIELD_VALUE
	#define INI_MAX_FIELD_VALUE (256)
#endif

static g_INIFileName[INI_MAX_INSTANCES][INI_MAX_FIELD_NAME];
static File:g_INIFile[INI_MAX_INSTANCES];

static g_INITimer[INI_MAX_INSTANCES];

static g_INIFieldsCount[INI_MAX_INSTANCES];
static g_INIFields[INI_MAX_INSTANCES][INI_MAX_FIELDS][INI_MAX_FIELD_NAME char];
static g_INIValues[INI_MAX_INSTANCES][INI_MAX_FIELDS][INI_MAX_FIELD_VALUE char];

stock static INI_OpenFile(const file[])
{
	for (new i; i < INI_MAX_INSTANCES; i++)
	{
		if (g_INIFileName[i][0] && strcmp(g_INIFileName[i], file))
		{
		    if (i == (INI_MAX_INSTANCES - 1))
				goto OPEN_NEW;
				
		    continue;
		}
		
		if (g_INITimer[i] != 0)
  			return i;

	OPEN_NEW:
	    if (!fexist(file))
			g_INIFile[i] = fopen(file, io_readwrite);
		else
			g_INIFile[i] = fopen(file, io_read);
			
		if (!g_INIFile[i])
		    return -1;
		fclose(g_INIFile[i]);
		    
        strcat(g_INIFileName[i], file);
		    
		g_INITimer[i] = SetTimerEx("INI_OnFileTimeout", INI_FILE_TIMEOUT, false, "i", i);
        g_INIFieldsCount[i] = 0;
        
        new pos;
		new data[INI_MAX_FIELD_NAME + INI_MAX_FIELD_VALUE];
		new extract[INI_MAX_FIELD_VALUE];
		while (g_INIFieldsCount[i] < INI_MAX_FIELDS && fread(g_INIFile[i], data))
		{
			INI_StripLine(data);

			pos = strfind(data, "=", false);
			if (pos == -1)
				continue;

			strmid(extract, data, 0, pos);
			strpack(g_INIFields[i][g_INIFieldsCount[i]], extract);
			
			strmid(extract, data[pos + 1], pos + 1, strlen(data));
			strpack(g_INIValues[i][g_INIFieldsCount[i]], extract);

			g_INIFieldsCount[i]++;
		}
		
		return i;
	}
	
	return -1;
}

forward INI_OnFileTimeout(i);
public  INI_OnFileTimeout(i)
{
	g_INIFile[i] = fopen(g_INIFileName[i], io_write);
	if (!g_INIFile[i])
 		return;
		
	new field[INI_MAX_FIELD_VALUE];
	new value[INI_MAX_FIELD_VALUE];
	new data[INI_MAX_FIELD_NAME + INI_MAX_FIELD_VALUE];
	for (new x, y = g_INIFieldsCount[i]; x < y; x++)
	{
	    strunpack(field, g_INIFields[i][x]);
	    strunpack(value, g_INIValues[i][x]);
	    format(data, sizeof (data), "%s=%s\r\n", field, value);
	    fwrite(g_INIFile[i], data);
	}
	
	fclose(g_INIFile[i]);
    g_INITimer[i] = 0;
    g_INIFileName[i][0] = EOS;
}

stock static INI_StripLine(string[])
{
	new l = strlen(string);

	if (string[l - 2] == '\r')
		string[l - 2] = '\0';

	if (string[l - 1] == '\n')
		string[l - 1] = '\0';
}

stock INI_Get(const file[], const field[], dest[], maxlength = sizeof (dest))
{
	new id = INI_OpenFile(file);
	if (id == -1)
	    return 0;
	    
	new extract[INI_MAX_FIELD_VALUE];
	for (new i, j = g_INIFieldsCount[id]; i < j; i++)
	{
	    strunpack(extract, g_INIFields[id][i]);
	    if (field[0] && strcmp(extract, field))
	        continue;

	    strunpack(dest, g_INIValues[id][i], maxlength);
	    return 1;
	}
	
	return 0;
}

stock INI_GetInt(const file[], const field[])
{
	new id = INI_OpenFile(file);
	if (id == -1)
	    return -1;

	new extract[INI_MAX_FIELD_VALUE];
	for (new i, j = g_INIFieldsCount[id]; i < j; i++)
	{
	    strunpack(extract, g_INIFields[id][i]);
	    if (field[0] && strcmp(extract, field))
	        continue;

	    strunpack(extract, g_INIValues[id][i]);
	    return strval(extract);
	}

	return -1;
}

stock Float:INI_GetFloat(const file[], const field[])
{
	new id = INI_OpenFile(file);
	if (id == -1)
	    return -1.0;

	new extract[INI_MAX_FIELD_VALUE];
	for (new i, j = g_INIFieldsCount[id]; i < j; i++)
	{
	    strunpack(extract, g_INIFields[id][i]);
	    if (field[0] && strcmp(extract, field))
	        continue;

	    strunpack(extract, g_INIValues[id][i]);
	    return floatstr(extract);
	}

	return -1.0;
}

stock INI_Set(const file[], const field[], const value[])
{
	new id = INI_OpenFile(file);
	if (id == -1)
	    return 0;

	new extract[INI_MAX_FIELD_VALUE];
	for (new i, j = g_INIFieldsCount[id]; i < j; i++)
	{
	    strunpack(extract, g_INIFields[id][i]);
	    if (field[0] && strcmp(extract, field))
	    	continue;

	    strpack(g_INIValues[id][i], value);
	    return 1;
	}

	if (g_INIFieldsCount[id] < INI_MAX_FIELDS)
	{
		strpack(g_INIFields[id][g_INIFieldsCount[id]], field);
		strpack(g_INIValues[id][g_INIFieldsCount[id]], value);
		g_INIFieldsCount[id]++;
  		return 1;
	}
	
	return 0;
}

stock INI_SetInt(const file[], const field[], value)
{
	new id = INI_OpenFile(file);
	if (id == -1)
	    return 0;

	new extract[INI_MAX_FIELD_VALUE];
	for (new i, j = g_INIFieldsCount[id]; i < j; i++)
	{
	    strunpack(extract, g_INIFields[id][i]);
	    if (field[0] && strcmp(extract, field))
			continue;

		valstr(extract, value);
	    strpack(g_INIValues[id][i], extract);
	    return 1;
	}

	if (g_INIFieldsCount[id] < INI_MAX_FIELDS)
	{
		strpack(g_INIFields[id][g_INIFieldsCount[id]], field);
		valstr(extract, value);
		strpack(g_INIValues[id][g_INIFieldsCount[id]], extract);
		g_INIFieldsCount[id]++;
  		return 1;
	}

	return 0;
}

stock INI_SetFloat(const file[], const field[], Float:value)
{
	new id = INI_OpenFile(file);
	if (id == -1)
	    return 0;

	new extract[INI_MAX_FIELD_VALUE];
	for (new i, j = g_INIFieldsCount[id]; i < j; i++)
	{
	    strunpack(extract, g_INIFields[id][i]);
	    if (field[0] && strcmp(extract, field))
	    	continue;
		
		format(extract, sizeof (extract), "%f", value);
	    strpack(g_INIValues[id][i], extract);
	    return 1;
	}

	if (g_INIFieldsCount[id] < INI_MAX_FIELDS)
	{
		strpack(g_INIFields[id][g_INIFieldsCount[id]], field);
		format(extract, sizeof (extract), "%f", value);
		strpack(g_INIValues[id][g_INIFieldsCount[id]], extract);
		g_INIFieldsCount[id]++;
  		return 1;
	}

	return 0;
}

stock INI_UnSet(const file[], const field[])
{
	new id = INI_OpenFile(file);
	if (id == -1)
	    return 0;

	new bool:shift;
	new extract[INI_MAX_FIELD_VALUE];
	for (new i, j = g_INIFieldsCount[id]; i < j; i++)
	{
	    if (shift)
	    {
	        g_INIFields[id][i - 1] = g_INIFields[id][i];
	        g_INIFields[id][i][0] = EOS;
	        g_INIValues[id][i - 1] = g_INIValues[id][i];
	       	g_INIValues[id][i][0] = EOS;
	    }
	    else
	    {
		    strunpack(extract, g_INIFields[id][i]);
		    if (field[0] && strcmp(extract, field))
		        continue;

	        g_INIFields[id][i][0] = EOS;
	        g_INIValues[id][i][0] = EOS;

			shift = true;
		}
	}
	
	if (shift)
	{
		g_INIFieldsCount[id]--;
		return 1;
	}
	
	return 0;
}
