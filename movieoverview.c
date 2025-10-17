#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

// Callback for listing movie names (partial match)
int list_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    static int count = 1;
    printf("%d. %s\n", count++, argv[0] ? argv[0] : "NULL");
    return 0;
}

// Callback for showing full movie details
int detail_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    printf("\n=== Movie Details ===\n");
    for (int i = 0; i < argc; i++) {
        printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("=====================\n");
    return 0;
}

int main(void) {
    sqlite3 *db;
    char *err_msg = 0;
    int rc, choice;

    // Use your uploaded database here
    rc = sqlite3_open("movies.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, " Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    while (1) {
        printf("\n--- MOVIE DATABASE SYSTEM ---\n");
        printf("1. Search Movie\n");
        printf("2. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // clear newline

        if (choice == 1) {
            char movie_name[200];
            printf("Enter movie name to search: ");
            fgets(movie_name, sizeof(movie_name), stdin);
            movie_name[strcspn(movie_name, "\n")] = 0; // remove newline

            // Step 1: Show partial matches
            char sql_list[1000];
            snprintf(sql_list, sizeof(sql_list),
                     "SELECT title FROM movie WHERE title LIKE '%%%s%%' COLLATE NOCASE;",
                     movie_name);

            printf("\n🎬 Movies matching '%s':\n\n", movie_name);

            rc = sqlite3_exec(db, sql_list, list_callback, 0, &err_msg);
            if (rc != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
            }

            // Step 2: Try showing full details for exact name match
            char sql_detail[1000];
            snprintf(sql_detail, sizeof(sql_detail),
                     "SELECT * FROM movie WHERE title = '%s' COLLATE NOCASE;", movie_name);

            rc = sqlite3_exec(db, sql_detail, detail_callback, 0, &err_msg);
            if (rc != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
            }

            printf("\n");

        } else if (choice == 2) {
            printf(" Exiting program. Goodbye!\n");
            break;

        } else {
            printf("⚠️ Invalid choice! Please enter 1 or 2.\n");
        }
    }

    sqlite3_close(db);
    return 0;
}
