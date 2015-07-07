#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_DATA 512
#define MAX_ROWS 100

struct Address{
  int id;
  int set;
  char name[MAX_DATA];
  char email[MAX_DATA];
  int max_data;
};

struct Database{
  struct Address rows[MAX_ROWS];
  int max_rows;
};

struct Connection{
  FILE *file;
  struct Database *db;
};

void Database_close(struct Connection *conn);


void die(struct Connection *conn, const char *message){

  if(errno){
    perror(message);
  } else{
    printf("ERROR: %s\n", message);
  }

  Database_close(conn);
  
}

void Address_print(struct Address *addr){

  printf("%d %s %s\n", 
	 addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn){

  int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);
  if(rc != 1) die(conn, "Failed to load database.");
}

struct Connection *Database_open(const char *filename, char mode){

  struct Connection *conn = malloc(sizeof(struct Connection));
  if(!conn) die(conn, "Memory error");

  conn->db = malloc(sizeof(struct Database));
  if(!conn->db) die(conn, "Memory error");

  if(mode == 'c'){
    conn->file = fopen(filename, "w");
  } else{
    conn->file = fopen(filename, "r+");

    if(conn->file){
      Database_load(conn);
    }
  }

  if(!conn->file) die(conn, "Failed to open the file.");

  return conn;
}

void Database_close(struct Connection *conn){

  if(conn){
    if(conn->file) fclose(conn->file);
    if(conn->db) free(conn->db)
    free(conn);
  }
}

void Database_write(struct Connection *conn){

  rewind(conn->file);

  int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
  if(rc != 1) die(conn, "Failed to write database.");

  rc = fflush(conn->file);
  if(rc == -1) die(conn, "Cannot flush database");

}

void Database_create(struct Connection *conn, int max_rows, int max_data){
  
  int i=0;

  conn->db->max_rows = max_rows;
   
  for(i=0; i < conn->db->max_rows; i++){
    // make a prototype to intialize it
    struct Address addr = {.id = i, .set = 0, .max_data = max_data};
    // then just assign it
    conn->db->rows[i] = addr;
  }
}

void Database_set(struct Connection *conn, int id, const char *name,
		  const char *email){

  struct Address *addr = &conn->db->rows[id];
  if(addr->set) die(conn, "Already set, delete it first");

  int max_rows = conn->db->max_rows;
  int max_data = addr->max_data;
  
  addr->set = 1;
  // Warning: bug, read the "How to Break It" and fix this
  char *res = strncpy(addr->name, name, max_rows);
  res[max_data - 1] = '\0';
  // demonstrate the strncpy bug
  if(!res) die(conn, "Name copy failed");

  res = strncpy(addr->email, email, max_data);
  if(!res) die(conn, "Email copy failed");
}

void Database_get(struct Connection *conn, int id){

  struct Address *addr = &conn->db->rows[id];

  if(addr->set){
    Address_print(addr);
  } else{
    die(conn, "ID is not set");
  }
}

void Database_delete(struct Connection *conn, int id){
  struct Address addr = {.id = id, .set = 0};
  conn->db->rows[id] = addr;
}

void Database_list(struct Connection *conn){

  int i=0;
  struct Database *db = conn->db;

  for(i=0; i < db->max_rows; i++){
    struct Address *cur = &db->rows[i];

    if(cur->set){
	Address_print(cur);
    }
  }
}

int main(int argc, char *argv[]){

  if(argc < 3) die(NULL, "USAGE: ex17 <dbfile> <action> [action params]");

  char *filename = argv[1];
  char action = argv[2][0];
  struct Connection *conn = Database_open(filename, action);
  int max_rows = conn->db->max_rows;
  int id = 0;

  
  if(argc > 3) id = atoi(argv[3]); //converts string to integer
  if(id >= max_rows) die(conn, "There's not that many records.");

  switch(action){
    
  case 'c':
    if(argc != 5) die(conn, "need filename, max_rows, max_data to create");
    
    int max_rows = atoi(argv[3]);
    int max_data = atoi(argv[4]);
    Database_create(conn, max_rows, max_data);
    Database_write(conn);
    break;
    
  case 'g':
    if(argc != 4) die(conn, "Need an id to get");

    Database_get(conn, id);
    break;
    
  case 's':
    if(argc != 6) die(conn, "Need id, name, email to set");

    Database_set(conn, id, argv[4], argv[5]);
    Database_write(conn);
    break;

  case 'd':
    if(argc != 4) die(conn, "Need id to delete");

    Database_delete(conn, id);
    Database_write(conn);
    break;

  case 'l':
    Database_list(conn);
    break;

  default:
    die(conn, "Invalid action, only: c=create, g=get, s=set, d=del, l=list");

    
  }

  Database_close(conn);

  return 0;
}
