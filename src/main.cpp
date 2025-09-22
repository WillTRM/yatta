#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <string>

/*
 *   unused - Ignored in this case, see the documentation for sqlite3_exec
 *    count - The number of columns in the result set
 *     data - The row's data
 *  columns - The column names
*/
static int callback(void *unused, int count, char **data, char **columns)
{
   std::cout << data[0] << ". " << data[1] << " (" << data[2] << ")\n";
   return 0;
}

void reset_table(sqlite3 *db, char *error_msg, int dc)
{
   const char* sql_drop = "DROP TABLE if EXISTS tasks";
   const char* sql_create = "CREATE TABLE tasks (NAME TEXT, DUE TEXT)";
   // executes the passed SQL statement and for every result sow calls 
   dc = sqlite3_exec(db, sql_drop, callback, 0, &error_msg);
   dc = sqlite3_exec(db, sql_create, callback, 0, &error_msg);

   if(dc != SQLITE_OK){
      std::cout << "SQL error: " << error_msg << "\n";
      sqlite3_free(error_msg);
   } else {
      std::cout << "Tasks cleared successfully\n";
   }
}

// who cares about sql injection this runs locally
void add_to_table(sqlite3 *db, char *error_msg, int dc, std::string task, std::string due_date)
{
   std::stringstream sql_insert;
   sql_insert << "INSERT INTO tasks (NAME, DUE) VALUES ('" << task << "', '" << due_date << "')";
   dc = sqlite3_exec(db, sql_insert.str().c_str(), callback, 0, &error_msg);

   if(dc != SQLITE_OK){
      std::cout << "SQL error: " << error_msg << "\n";
      sqlite3_free(error_msg);
   } else {
      std::cout << "Task added successfully\n";
   }
}

void remove_from_table(sqlite3 *db, char *error_msg, int dc, int id_to_remove)
{
   std::stringstream sql_remove;
   sql_remove << "DELETE FROM tasks WHERE rowid = " << id_to_remove;
   sqlite3_exec(db, sql_remove.str().c_str(), callback, 0, &error_msg);
}

void show_all_tasks(sqlite3 *db, char *error_msg, int dc)
{
   const char* sql_select = "SELECT rowid, * FROM tasks";
   sqlite3_exec(db, sql_select, callback, 0, &error_msg);
}

int main()
{
   sqlite3 *db;
   char *error_msg = 0;
   int dc = sqlite3_open("tasks.db", &db);

   if (dc) {
      std::cout << "Cannot open database\n";
      exit(1);
   } else {
      std::cout << "Database opened\n\n";
   }

   int done = 0;
   while (not done) {
      std::cout << "Select an option:\n";
      std::cout << "1. Show all tasks\n";
      std::cout << "2. Add a task\n";
      std::cout << "3. Complete a task\n";
      std::cout << "4. Clear all tasks\n";
      std::cout << "5. Exit\n";
      int choice;
      std::cin >> choice;

      std::string task_name;
      std::string due_date;
      int task_num;

      switch (choice) {
         case 1:
            std::cout << "\n";
            show_all_tasks(db, error_msg, dc);
            std::cout << "\n";
            break;

         case 2:
            
            std::cout << "\nEnter name of task: ";
            std::cin.ignore();
            std::getline(std::cin, task_name);
            std::cout << "Enter when task should be done by: ";
            std::cin.ignore();
            std::getline(std::cin, due_date);
            add_to_table(db, error_msg, dc, task_name, due_date);
            std::cout << "\n";
            break;

         case 3:
            std::cout << "Enter ID of task to complete: ";
            std::cin >> task_num;
            remove_from_table(db, error_msg, dc, task_num);
            break;

         case 4:
            reset_table(db, error_msg, dc);
            break;

         case 5:
         done = 1;
            break;
      }
   }

   sqlite3_close(db);
   return 0;
}