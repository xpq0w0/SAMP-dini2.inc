#if defined gini_included
	#endinput
#endif
#define gini_included

/*
 * g_ini.inc
 * Version: 1.1 (last updated on 5th July, 2016)
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

#if !defined INI_CONVERT_DINI
	#define INI_CONVERT_DINI
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

#if defined INI_CONVERT_DINI
	stock dini_Exists(const file[])
		return fexist(file);

	stock dini_Remove(const file[])
		return fremove(file);

	stock dini_Create(const file[])
	{
		if (fexist(file))
			return false;

		new File:handle;
		handle = fopen(file, io_write);
		if (handle)
		{
			fclose(handle);
			return true;
		}
		return false;
	}

	stock dini_Set(const file[], const field[], const value[])
		return INI_Set(file, field, value);

	stock dini_IntSet(const file[], const field[], value)
		return INI_SetInt(file, field, value);

	stock dini_FloatSet(const file[], const field[], Float:value)
		return INI_SetFloat(file, field, value);

	stock dini_BoolSet(const file[], const field[], bool:value)
		return INI_SetInt(file, field, _:value);

	stock dini_Get(const file[], const field[])
	{
		new ret[INI_MAX_FIELD_VALUE];
		INI_Get(file, field, ret);
		return ret;
	}

	stock dini_Int(const file[], const field[])
		return INI_GetInt(file, field);

	stock Float:dini_Float(const file[], const field[])
		return INI_GetFloat(file, field);

	stock bool:dini_Bool(const file[], const field[])
		return bool:INI_GetInt(file, field);

	stock dini_UnSet(const file[], const field[])
		return INI_UnSet(file, field);

	stock dini_IsSet(const file[], const field[])
		return fmatch(file, field, 0);

	stock DINI_StripNewLine(string[])
		return INI_StripLine(string);

	stock DINI_fcopytextfile(const file[], const newname[])
	{
		new File:handles[2];

		handles[0] = fopen(file, io_read);
		if (!handles[0])
			return 0;

		handles[1] = fopen(newname, io_write);
		if (!handles[1])
		{
			fclose(handles[0]);
			return 0;
		}

		new data[INI_MAX_FIELD_NAME + INI_MAX_FIELD_VALUE];
		while (fread(handles[0], data))
		{
			INI_StripLine(data);
			strcat(data, "\r\n");
			fwrite(handles[1], data);
		}

		fclose(handles[0]);
		fclose(handles[1]);

		return 1;
	}
#endif
