#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <time.h>

void Filedetails(struct stat statBuf){

  char perms[10];
  struct passwd* pwd;
  struct group* group;
  int i;


  if(S_ISDIR(statBuf.st_mode)){
    printf("Type: directory\n"); 
  }

  if(S_ISCHR(statBuf.st_mode)){
    printf("Type: character\n");
  }

  if(S_ISBLK(statBuf.st_mode)){
    printf("Type: block\n");
  }

  if(S_ISREG(statBuf.st_mode)){
    printf("Type: regular file\n");
  }

  if(S_ISFIFO(statBuf.st_mode)){
    printf("Type: pipe\n");
  }

  if(S_ISLNK(statBuf.st_mode)){
    printf("Type: link\n");
  }

  if(S_ISSOCK(statBuf.st_mode)){
    printf("Type: socket\n");
  }


  perms[9] = '\0';


  for(i = 0; i < 9; i++){
    perms[i] = '-';
  }


  if(statBuf.st_mode & S_IRUSR){
    perms[0] = 'r'; 
  }

  if(statBuf.st_mode & S_IWUSR){
    perms[1] = 'w';
  }

  if(statBuf.st_mode & S_IXUSR){
    perms[2] = 'x';
  }

  if(statBuf.st_mode & S_IRGRP){
    perms[3] = 'r';
  }

  if(statBuf.st_mode & S_IWGRP){
    perms[4] = 'w';
  }

  if(statBuf.st_mode & S_IXGRP){
    perms[5] = 'x';
  }

  if(statBuf.st_mode & S_IWOTH){
    perms[6] = 'r';
  }

  if(statBuf.st_mode & S_IROTH){
    perms[7] = 'w';
  }
  if(statBuf.st_mode & S_IXOTH){
    perms[8] = 'x';
  }

  printf("Permissions: %s\n", perms);

 
  if((pwd = getpwuid(statBuf.st_uid)) == NULL){
    printf("Owner: %d\n", statBuf.st_uid);
  } else {
    printf("Owner: %s\n", pwd -> pw_name);
  }


  if((group = getgrgid(statBuf.st_gid)) == NULL){
    printf("Group: %d\n", statBuf.st_gid);
  } else {
    printf("Group: %s\n", group -> gr_name);
  }


  printf("Last Modified: %s", ctime(&(statBuf.st_mtime)));
  printf("Links: %lu\n", statBuf.st_nlink);


  if((S_ISCHR(statBuf.st_mode))|| S_ISBLK(statBuf.st_mode)) {
    printf("Major: %d, Minor %d\n", major(statBuf.st_rdev), minor(statBuf.st_rdev));
     printf("I-Node Number: %lu \n\n", statBuf.st_ino);
  } else {
    printf("Size: %ld\n", statBuf.st_size);
    printf("I-Node Number: %lu \n\n", statBuf.st_ino);
  }

}

int main(int argc, char *argv[]){

  struct stat statBuf;
  struct dirent* ent;
  DIR* dir;
  int i, j;


  if (argc > 1){


    for(i = 1; i < argc; i++){
      if(lstat(argv[i], &statBuf) == -1){
	printf("Lstat failed on this line: %s", argv[i]);
	exit(2);
      }


      printf("File: %s\n", argv[i]);      
      Filedetails(statBuf);

    }
  } else {

    if((dir = opendir(".")) == NULL){
      printf("opendir failed.\n");
      exit(3);
    }


    while((ent = readdir(dir)) != NULL){
      if(lstat(ent -> d_name, &statBuf) == -1){
        printf("Lstat failed.\n");
        exit(2);
      }


      printf("File: %s\n", ent -> d_name);
      Filedetails(statBuf);
      
    }
  }



  return 0; 

}
