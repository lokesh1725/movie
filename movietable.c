#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

int header_printed = 0;

// Print movie details (for search results)
int movie_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    if (!header_printed) {
        printf("\n============================================================================================\n");
        printf("%-10s | %-30s | %-10s | %-10s | %-10s\n",
               "Movie ID", "Title", "Runtime", "Revenue", "Release");
        printf("============================================================================================\n");
        header_printed = 1;
    }

    printf("%-10s | %-30s | %-10s | %-10s | %-10s\n",
           argv[0] ? argv[0] : "NULL",
           argv[1] ? argv[1] : "NULL",
           argv[2] ? argv[2] : "NULL",
           argv[3] ? argv[3] : "NULL",
           argv[4] ? argv[4] : "NULL");
    return 0;
}

// Callback for detailed single movie overview
int overview_movie_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    printf("\n============================================================================================\n");
    printf("%-10s | %-30s | %-10s | %-10s | %-10s\n",
           "Movie ID", "Title", "Runtime", "Revenue", "Release");
    printf("============================================================================================\n");

    printf("%-10s | %-30s | %-10s | %-10s | %-10s\n",
           argv[0] ? argv[0] : "NULL",
           argv[1] ? argv[1] : "NULL",
           argv[2] ? argv[2] : "NULL",
           argv[3] ? argv[3] : "NULL",
           argv[4] ? argv[4] : "NULL");

    printf("============================================================================================\n");
    printf("Overview:\n%s\n", argv[5] ? argv[5] : "No overview available");
    printf("============================================================================================\n");
    return 0;
}

// Overview summary callbacks for directors
int overview_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    printf(" Total Movies: %s\n", argv[0] ? argv[0] : "0");
    return 0;
}

int avg_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    printf(" Average Runtime: %s minutes\n", argv[0] ? argv[0] : "NULL");
    return 0;
}

int top_revenue_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    printf(" Top Revenue Movie: %s (Revenue: %s)\n",
           argv[0] ? argv[0] : "NULL",
           argv[1] ? argv[1] : "NULL");
    return 0;
}

int main() {
    sqlite3 *db;
    char *err_msg = 0;
    int rc, choice;

    // Open database
    rc = sqlite3_open("movies.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, " Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    while (1) {
        printf("\n=========== MOVIE DATABASE SYSTEM ===========\n");
        printf("1) Search by Movie Name\n");
        printf("2) Search by Director Name\n");
        printf("3) Overview (by Movie ID)\n");
        printf("4) Exit\n");
        printf("=============================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // clear newline

        header_printed = 0;

        if (choice == 1) {
            char movie_name[100];
            printf("Enter movie name: ");
            fgets(movie_name, sizeof(movie_name), stdin);
            movie_name[strcspn(movie_name, "\n")] = 0;

            char sql[600];
            snprintf(sql, sizeof(sql),
                     "SELECT movie_id, title, runtime, revenue, release_date "
                     "FROM movie WHERE title LIKE '%%%s%%' COLLATE NOCASE;",
                     movie_name);

            rc = sqlite3_exec(db, sql, movie_callback, 0, &err_msg);
            if (rc != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
            }

        } else if (choice == 2) {
            char director_name[100];
            printf("Enter director name: ");
            fgets(director_name, sizeof(director_name), stdin);
            director_name[strcspn(director_name, "\n")] = 0;

            printf("\n Searching for movies directed by '%s'...\n", director_name);

            char sql[700];
            snprintf(sql, sizeof(sql),
                     "SELECT m.movie_id, m.title, m.runtime, m.revenue, m.release_date "
                     "FROM movie m "
                     "JOIN movie_crew mc ON m.movie_id = mc.movie_id "
                     "JOIN person p ON mc.person_id = p.person_id "
                     "WHERE p.person_name LIKE '%%%s%%' AND mc.job = 'Director' COLLATE NOCASE;",
                     director_name);

            rc = sqlite3_exec(db, sql, movie_callback, 0, &err_msg);
            if (rc != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
            } else {
                printf("\n========== OVERVIEW FOR DIRECTOR: %s ==========\n", director_name);

                char overview_sql[700];

                // Total Movies
                snprintf(overview_sql, sizeof(overview_sql),
                         "SELECT COUNT(*) FROM movie m "
                         "JOIN movie_crew mc ON m.movie_id = mc.movie_id "
                         "JOIN person p ON mc.person_id = p.person_id "
                         "WHERE p.person_name LIKE '%%%s%%' AND mc.job = 'Director' COLLATE NOCASE;",
                         director_name);
                sqlite3_exec(db, overview_sql, overview_callback, 0, &err_msg);

                // Average Runtime
                snprintf(overview_sql, sizeof(overview_sql),
                         "SELECT AVG(m.runtime) FROM movie m "
                         "JOIN movie_crew mc ON m.movie_id = mc.movie_id "
                         "JOIN person p ON mc.person_id = p.person_id "
                         "WHERE p.person_name LIKE '%%%s%%' AND mc.job = 'Director' COLLATE NOCASE;",
                         director_name);
                sqlite3_exec(db, overview_sql, avg_callback, 0, &err_msg);

                // Top Revenue Movie
                snprintf(overview_sql, sizeof(overview_sql),
                         "SELECT m.title, m.revenue FROM movie m "
                         "JOIN movie_crew mc ON m.movie_id = mc.movie_id "
                         "JOIN person p ON mc.person_id = p.person_id "
                         "WHERE p.person_name LIKE '%%%s%%' AND mc.job = 'Director' "
                         "ORDER BY m.revenue DESC LIMIT 1;",
                         director_name);
                sqlite3_exec(db, overview_sql, top_revenue_callback, 0, &err_msg);
            }

        } else if (choice == 3) {
            int movie_id;
            printf("Enter movie ID for overview: ");
            scanf("%d", &movie_id);
            getchar(); // clear newline

            printf("\n========== MOVIE OVERVIEW ==========\n");

            char sql_overview[700];
            snprintf(sql_overview, sizeof(sql_overview),
                     "SELECT movie_id, title, runtime, revenue, release_date, overview "
                     "FROM movie WHERE movie_id = %d;", movie_id);

            rc = sqlite3_exec(db, sql_overview, overview_movie_callback, 0, &err_msg);
            if (rc != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
            }

        } else if (choice == 4) {
            printf("\n Avalothaa Mudinjiruchu da sammy! TATA!\n");
            break;

        } else {
            printf("⚠️ Invalid choice! Please try again.\n");
        }
    }

    sqlite3_close(db);
    return 0;
}
