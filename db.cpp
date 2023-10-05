#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>

auto db_filename = "db1.db3";
#define CREATE_SCHEMA "\
\
    DROP TABLE IF EXISTS logos;\
    CREATE TABLE logos (id INTEGER PRIMARY KEY, value BLOB);\
\
"
static const std::string filename_logo_png = "logo.png";

void db_insert_logo(){
 try{

  SQLite::Database db(db_filename, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
  db.exec(CREATE_SCHEMA);

  FILE *fp = fopen(filename_logo_png.c_str(), "rb");
  if(fp){
    char  buf[16*1024];
    void *blob = &buf;
    const int size = static_cast<int>(fread(blob, 1, 16*1024, fp));
    fclose(fp);
    buf[size] = '\0';
    std::cout << "Inserting " + std::to_string(size) + " byte logo.." << std::endl;

    SQLite::Statement query(db, "INSERT INTO logos VALUES (NULL, ?)");
    query.bind(1, blob, size);
    query.exec();
  }
 }catch(std::exception &e){
    std::cout << "Exception: " << e.what() << std::endl;
 }

}


int main(){
    db_insert_logo();
    std::cout << "everything ok, quitting\n";

    return EXIT_SUCCESS;
}
