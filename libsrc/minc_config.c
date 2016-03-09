#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stdio.h>
#include <ctype.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "minc_config.h"

static const char *_CONFIG_VAR[]=
  {
      "MINC_FORCE_V2",
      "MINC_COMPRESS",
      "MINC_CHUNKING",
      "MINC_LOGFILE",
      "MINC_LOGLEVEL",
      "MINC_MAX_FILE_BUFFER_KB",
      "MINC_MAX_MEMORY_KB",
      "MINC_FILE_CACHE_MB",
      "MINC_CHECKSUM",
      "MINC_PREFER_V2_API"
  };

/*settings cache*/
static const char*_CONFIG_VAL[MICFG_COUNT]={(const char*)0};
static int _CONFIG_PRESENT[MICFG_COUNT]={0};

/** Simple function to read a user's .mincrc file, if present.
 */
static int
miread_cfg(const char *name, char *buffer, int maxlen)
{
    FILE *fp;
    int result = 0;
    char *home_ptr = getenv("HOME");
    char path[256];

    if (home_ptr != NULL) {
      strncpy(path, home_ptr, sizeof(path) - 1);
    }
    else {
      path[0] = '\0';
    }
    strcat(path, "/.mincrc");
    
    if ((fp = fopen(path, "r")) != NULL) {
        while (fgets(buffer, maxlen, fp)) {
            if (buffer[0] == '#') {
                continue;
            }
            if (!strncasecmp(buffer, name, strlen(name))) {
                char *tmp = strchr(buffer, '=');
                if (tmp != NULL) {
                    tmp++;
                    while (isspace(*tmp)) {
                        tmp++;
                    }
                    strncpy(buffer, tmp, maxlen);
                    result = 1;
                    break;
                }
            }
        }
        fclose(fp);
    }
    return (result);
}

const char * miget_cfg_str(int id)
{
  if(id<0 || id>=MICFG_COUNT) return "";
  
  if(!_CONFIG_VAL[id]) 
  {
    const char *name=_CONFIG_VAR[id];
    char buffer[256];
    char *var_ptr;

    if ((var_ptr = getenv(name)) != NULL) {
        strncpy(buffer, var_ptr, sizeof(buffer) - 1);
        _CONFIG_PRESENT[id]=1;
    }
    else {
        if (!miread_cfg(name, buffer, sizeof(buffer))) {
          _CONFIG_PRESENT[id]=0;
          buffer[0]='\0';
        } else {
          _CONFIG_PRESENT[id]=1;
        }
    }
    /*small memory leak here, unless we deallocate the settings cache*/
    _CONFIG_VAL[id]=strdup(buffer);
  }
  return _CONFIG_VAL[id];
}

int miget_cfg_present(int id)
{
  if(id <0 || id>=MICFG_COUNT) return 0;
  
  miget_cfg_str(id);
  return _CONFIG_PRESENT[id];
}


int miget_cfg_bool(int id)
{
  const char *_var=miget_cfg_str(id);
  return atoi(_var) != 0;
}

int miget_cfg_int(int id)
{
  const char *_var=miget_cfg_str(id);
  return atoi(_var);
}

double miget_cfg_double(int id)
{
  const char *_var=miget_cfg_str(id);
  return atof(_var);
}
