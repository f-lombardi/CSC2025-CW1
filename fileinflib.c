/*
 * Replace the following string of 0s with your student number
 * 160437414
 */
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "sys/param.h"


#include "fileinflib.h"

/* is_user_exec helper function forward declaration */
bool is_user_exec(mode_t mode, uid_t owner_uid, gid_t owner_gid);

/***************************************************************************/
/***** FILEINFLIB.H IMPLEMENTATIONS ****************************************/
/***************************************************************************/

/* see mode2str specification comments in fileinflib.h */
char *mode2str(mode_t mode, uid_t owner_uid, gid_t owner_gid) {
  char *mode_s = (char *)calloc(MODE_S_LEN, sizeof(char)); /*dynamically allocates the mode_s struct*/

  if (mode == NULL){ /*if the mode is equal to null*/
    errno = ENOMEM; /*sets the error*/
  }
  if (mode > MODE_MAX || mode < MODE_MIN){ /*checks the mode minimum and the mode maximum*/
    errno = EDOM; /*sets the error*/
    return NULL; /*returns null and exits function*/
  }
  if ((mode/0010000) % 2 != 1 || (mode/0010000) == 1) { /*bit checking*/
    if (S_ISDIR(mode)){
      mode_s[0] = 'd'; /*checks and assigns d(directory)*/
    }
    else if (S_ISREG(mode)){
      if (is_user_exec(mode, owner_uid, owner_gid)){ /*checks to see if the file is user executable*/
        mode_s[0] = 'e'; /*if user is executive then assigns e*/
      }
      else{
        mode_s[0] = 'f'; /*if not then assigns f(regular file)*/
      }
    }
    else if (S_ISLNK(mode)){
      mode_s[0] = 'l'; /*assigns l(symbolic link)*/
    }
    else {
      mode_s[0] = 'o'; /*assigns o(other type of file)*/
    }

    if (mode & S_IRUSR){ /*indexed mode string*/
      mode_s[1] = 'r'; /*checks and assigns read to the 1st character*/
    }
    else{
      mode_s[1] = '-'; /*if not an r, then replace with a -*/
    }
    if (mode & S_IWUSR){ /*indexed mode string*/
      mode_s[2] = 'w'; /*assigned write to the 2nd character*/
    }
    else{
      mode_s[2] = '-'; /*if not an w, then replace with a -*/
    }
    if (mode & S_IXUSR){ /*indexed mode string*/
      mode_s[3] = 'x'; /*assigned x(execute) to the 3rd character*/
    }
    else{
      mode_s[3] = '-'; /*if not an x, then replace with a -*/
    }
    if (mode & S_IRGRP){ /*indexed mode string*/
      mode_s[4] = 'r'; /*assigned r to the 4th character*/
    }
    else{
      mode_s[4] = '-'; /*if not an r, then replace with a -*/
    }
    if (mode & S_IWGRP){ /*indexed mode string*/
      mode_s[5] = 'w'; /*assigned w to the 5th character*/
    }
    else{
      mode_s[5] = '-'; /*if not an w, then replace with a -*/
    }
    if (mode & S_IXGRP){ /*indexed mode string*/
      mode_s[6] = 'x'; /*assigned x to the 6th character*/
    }
    else{
      mode_s[6] = '-'; /*if not an x, then replace with a -*/
    }
    if (mode & S_IROTH){ /*indexed mode string*/
      mode_s[7] = 'r'; /*assigned r to the 7th character*/
    }
    else{
      mode_s[7] = '-'; /*if not an r, then replace with a -*/
    }
    if (mode & S_IWOTH){ /*indexed mode string*/
      mode_s[8] = 'w'; /*assigned w to the 8th character*/
    }
    else{
      mode_s[8] = '-'; /*if not an w, then replace with a -*/
    }
    if (mode & S_IXOTH){ /*indexed mode string*/
      mode_s[9] = 'x'; /*assigned x to the 9th character*/
    }
    else{
      mode_s[9] = '-'; /*if not an x, then replace with a -*/
    }
    mode_s[10] = '\0'; /*add a null terminator to end of string*/
  }
  else {
    errno = EDOM; /*sets the error*/
    return NULL; /*returns null and exits function*/
  }
    return mode_s; /*returns dynamically allocated string*/
}


/* see getfileinf specification comments in fileinflib.h */
fileinf *getfileinf(char *path) {
struct stat buff;
  struct stat *bu = &buff;

  if (lstat (path, bu) == -1){
    return NULL; /*returns null and exits function*/
  }

  fileinf *fi = malloc(sizeof(fileinf));
  /*assigns file mode to mode2str function, using error checking*/
  fi->dirname = (char*)calloc(strlen(path), sizeof(char));
  /*assigns file mode to mode2str function, using error checking*/
  fi->basename = (char*)calloc(strlen(path), sizeof(char));

  if (!fi || !fi->dirname || !fi->basename) {
    errno = ENOMEM; /*sets the error*/
    delfileinf(fi); /*frees the memory allocated*/
    return NULL; /*returns null and exits function*/
}

  fi->dirname = dirname(path); /*allocates dirname the path length*/
  fi->basename = basename(path); /*allocates basename the path length*/
  fi->mode_s = mode2str(bu->st_mode, bu->st_uid, bu->st_gid);
  fi->type = fi->mode_s[0] - 'd';
  fi->size = bu->st_size; /*allocates buff size*/

  if (strlen(path) > MAXPATHLEN){ /*Checks the length of the path is greater than the MAXPATHLEN*/
    errno = EINVAL; /*sets the error*/
    return NULL; /*returns null and exits function*/
  }
  return fi;
}

/* see delfileinf specification comments in fileinflib.h */
void delfileinf(fileinf *fi) {
  if (fi) {
    if (fi->mode_s)
      free(fi->mode_s); /*deallocate the memory assigned to mode_s*/
    if (fi->basename)
      free (fi->basename); /*deallocate the memory assigned to basename*/
    if (fi->dirname)
      free (fi->dirname); /*deallocate the memory assigned to dirname*/
  free(fi);
  }
}

/***************************************************************************/
/***** HELPER FUNCTIONS ****************************************************/
/***************************************************************************/

/*
 * is_user_exec helper function to test user execute permission for given
 * file mode and file owner uid and file owner gid.
 * Uses getuid() to get the uid of the calling process and getgid() to get the
 * gid of the calling process.
 * This function is not part of the filecmdrlib interface.
 */
bool is_user_exec(mode_t mode, uid_t owner_uid, gid_t owner_gid) {
    if (owner_uid == getuid())
        return mode & S_IXUSR;

    if (owner_gid == getgid())
        return mode & S_IXGRP;

    return mode & S_IXOTH;
}

/***************************************************************************/
