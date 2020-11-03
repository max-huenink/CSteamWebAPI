#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "callApi.c"
#include "cJSON.c"
#include <libxml/parser.h>

char *createParams(char *param_fmt, char *param1)
{
    char *params;
    int paramLen = strlen(param_fmt) + strlen(param1);
    params = malloc(paramLen);
    sprintf(params, param_fmt, param1);
    return params;
}

int main()
{
    char *path_fmt = "%s/%s/v%s/"; // Interface, Method, Version
    char *path;
    char *method = "GetOwnedGames";
    char *aGameID = "440";
    char *mySteamID = "76561198109850663";
    int keyNeeded = 0;
    char *interface;
    char *version;
    char *ID;
    char *param_fmt;
    char *params;
    if (strcmp(method, "GetGlobalAchievementPercentagesForApp") == 0)
    {
        interface = "ISteamUserStats";
        version = "0002";
        ID = aGameID;
        param_fmt = "gameID=%s";
    }
    else if (strcmp(method, "GetPlayerSummaries") == 0)
    {
        interface = "ISteamUser";
        version = "0002";
        keyNeeded = 1;
        ID = mySteamID;
        param_fmt = "steamids=%s";
    }
    else if (strcmp(method, "GetFriendList") == 0)
    {
        interface = "ISteamUser";
        version = "0001";
        keyNeeded = 1;
        ID = mySteamID;
        param_fmt = "steamid=%s";
    }
    else if (strcmp(method, "GetPlayerAchievements") == 0)
    {
        return -1;
        interface = "ISteamUserStats";
        version = "0001";
        keyNeeded = 1;
        // Needs both steamid and appid
        //ID = mySteamID;
        params = "appid=%s&steamid=%s";
    }
    else if (strcmp(method, "GetuserStatsForGame") == 0)
    {
        return -1;
        interface = "ISteamUserStats";
        version = "0002";
        keyNeeded = 1;
        // Needs both steamid and appid
        //ID = mySteamID;
        params = "appid=%s&steamid=%s";
    }
    else if (strcmp(method, "GetOwnedGames") == 0)
    {
        interface = "IPlayerService";
        version = "0001";
        keyNeeded = 1;
        ID = mySteamID;
        param_fmt = "steamid=%s&include_appinfo=1&include_played_free_games=1";
    }
    else if (strcmp(method, "GetRecentlyPlayedGames") == 0)
    {
        interface = "IPlayerService";
        version = "0001";
        keyNeeded = 1;
        ID = mySteamID;
        param_fmt = "steamid=%s";
    }
    else if (strcmp(method, "GetSchemaForGame") == 0)
    {
        interface = "ISteamUserStats";
        version = "2";
        keyNeeded = 1;
        ID = aGameID;
        param_fmt = "appid=%s";
    }

    params = createParams(param_fmt, ID);
    //int paramLen = strlen(param_fmt) + strlen(ID);
    //params = malloc(paramLen);
    //sprintf(params, param_fmt, ID);

    int pathLen = strlen(interface)+strlen(method)+strlen(version)+strlen(path_fmt);
    path = malloc(pathLen);
    sprintf(path,path_fmt,interface,method,version);

    char *responseFileName = CallWebApi(path, params, keyNeeded);
    free(params);
    free(path);

    xmlDocPtr doc = xmlParseFile(responseFileName);

    /*
    cJSON *json = cJSON_GetObjectItemCaseSensitive(cJSON_Parse(response), "response");
    const cJSON *players = cJSON_GetObjectItemCaseSensitive(json,"players");
    const cJSON *player;
    cJSON_ArrayForEach(player, players)
    {
        cJSON *steamid = cJSON_GetObjectItemCaseSensitive(player, "steamid");
        cJSON *personaname = cJSON_GetObjectItemCaseSensitive(player, "personaname");
        cJSON *realname = cJSON_GetObjectItemCaseSensitive(player, "realname");
        printf("SteamID: %s\n",steamid->valuestring);
        printf("PersonaName: %s\n",personaname->valuestring);
        printf("RealName: %s\n",realname->valuestring);
        printf("\n");
    }
    */
    //xmlParseMemory(response, strlen(response));

    
    //char *str = cJSON_Print(json);
    //printf("cJSON: \n%s\n\n", str);

    return 0;
}
