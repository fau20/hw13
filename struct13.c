#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

struct pop_entry {
  int year;
  int population;
  char boro[15];
};

void read_csv() {
  int fd = open("./nyc_pop.csv", O_RDONLY, 0777);
  if(errno) {
    printf("Error: %s\n", strerror(errno));
    return;
  }
  struct stat st;
  stat("nyc_pop.csv", &st);
  if(errno) {
    printf("Error: %s\n", strerror(errno));
    return;
  }

  char file[st.st_size];
  read(fd, file, st.st_size);
  if(errno) {
    printf("Error: %s\n", strerror(errno));
    return;
  }

  int i = 0;
  int count = 0;
  for(i = 0; i < st.st_size; i++) {
    if(file[i] == '\n') count++;
  }
  count = count - 1;

  struct pop_entry entry[5 * count];
  char * borough[5];
  borough[0] = "Manhattan";
  borough[1] = "Brooklyn";
  borough[2] = "Queens";
  borough[3] = "Bronx";
  borough[4] = "Staten Island";

  char * ch = file;
  ch = strchr(ch, '\n') + 1;

  int y;
  int pop[5];

  int j;

  for(i = 0; i < count; i++) {
    sscanf(ch, "%d,%d,%d,%d,%d,%d", &y, pop, pop+1, pop+2, pop+3, pop+4);
    for(j = 0; j < 5; j++) {
      entry[5 * i + j].population = pop[j];
      entry[5 * i + j].year = y;
      strcpy(entry[5 * i + j].boro, borough[j]);
    }
  }
  ch = strchr(ch, '\n') + 1;
  int out = open("./outfile", O_WRONLY | O_CREAT | O_TRUNC, 0777);
  write(out, entry, 5*count*sizeof(struct pop_entry));
}

void print_entry(struct pop_entry p, int index) {
  printf("%d: year: %d\tboro: %s\tpop:%d\n", index, p.year, p.boro, p.population);
}

void read_data() {
  int o = open("./outfile", O_RDONLY, 0777);
  struct stat st;
  stat("./outfile", &st);
  if(errno) {
    printf("Error: %s\n", strerror(errno));
    return;
  }
  struct pop_entry entry[st.st_size / sizeof(struct pop_entry)];
  read(o, entry, st.st_size);

  int i;
  for(i = 0; i < st.st_size / sizeof(struct pop_entry); i++) {
    print_entry(entry[i], i);
  }
}

void add_data() {
  printf("Enter year boro pop: ");
  char ch[500];
  fgets(ch, 500, stdin);
  struct pop_entry p;
  sscanf(ch, "%d %s %d\n", &p.year, p.boro, &p.population);
  int o = open("./outfile", O_WRONLY | O_APPEND | O_CREAT, 0777);
  if(errno) {
    printf("Error: %s\n", strerror(errno));
    return;
  }
  write(o, &p, sizeof(struct pop_entry));
}

void update_data() {
  read_data();
  int file = open("./outfile", O_WRONLY, 0777);
  char ch[500];
  int index;
  printf("entry to update: ");
  fgets(ch, 500, stdin);
  lseek(file, index * sizeof(struct pop_entry), SEEK_SET);
  sscanf(ch, "%d\n", &index);

  char ch2[500];
  printf("Enter year boro pop: ");
  fgets(ch2, 500, stdin);
  struct pop_entry p;
  sscanf(ch2, "%d %s %d\n", &p.year, p.boro, &p.population);
  lseek(file, index * sizeof(struct pop_entry), SEEK_SET);
  write(file, &p, sizeof(struct pop_entry));
}

int main(int argc, char * argv[]) {
  if(argc > 1) {
  if(strcmp(argv[1], "-read_csv") == 0) read_csv();
  if(strcmp(argv[1], "-read_data") == 0) read_data();
  if(strcmp(argv[1], "-add_data") == 0) add_data();
  if(strcmp(argv[1], "-update_data") == 0) update_data();
}
  return 0;
}
