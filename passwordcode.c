#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For usleep()

// Function prototypes
void gotoxy(int x, int y);
void password(void);
void addContact(void);
void listContacts(void);
void menu(void);
void exitProgram(void);

// gotoxy using ANSI escape codes (Linux)
void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

// Main Function
int main() {
    password(); // Ask for password first
    return 0;
}

// Password Function
void password() {
    const char correct_pass[] = "lock"; // change password here
    char input[50];
    int i = 0;

    system("clear");
    gotoxy(20, 3);
    printf(" Enter Password to Access Phonebook: ");

    while (1) {
        char ch = getchar();
        if (ch == '\n') break;
        if (ch == 8 && i > 0) { // Handle backspace
            printf("\b \b");
            i--;
        } else if (ch != 8) {
            printf("*");
            input[i++] = ch;
        }
    }
    input[i] = '\0';

    if (strcmp(input, correct_pass) == 0) {
        gotoxy(25, 6);
        printf("\nAccess Granted!\n");  // ✅ Added newline
        fflush(stdout);                 // ✅ Flush output
        usleep(1000000);
        system("clear");                // ✅ Clears screen properly
        menu();
    } else {
        gotoxy(25, 5);
        printf(" Wrong Password! Try again...");
        usleep(1000000);
        password(); // Retry
    }
}

// Add Contact
void addContact() {
    system("clear");
    gotoxy(30, 2);
    printf(" ADD NEW CONTACT");

    FILE *fp;
    char name[50], gmail[50], gender[10], address[50];
    double phone;

    fp = fopen("contacts.txt", "ab+");
    if (fp == NULL) {
        printf("File Error!");
        return;
    }

    gotoxy(20, 4);
    printf("Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    gotoxy(20, 5);
    printf("Address: ");
    fgets(address, sizeof(address), stdin);
    address[strcspn(address, "\n")] = 0;

    gotoxy(20, 6);
    printf("Gender: ");
    fgets(gender, sizeof(gender), stdin);
    gender[strcspn(gender, "\n")] = 0;

    gotoxy(20, 7);
    printf("Gmail: ");
    fgets(gmail, sizeof(gmail), stdin);
    gmail[strcspn(gmail, "\n")] = 0;

    gotoxy(20, 8);
    printf("Phone: ");
    scanf("%lf", &phone);
    getchar(); // clear newline

    fprintf(fp, "%s %s %s %s %.0lf\n", name, address, gender, gmail, phone);
    fclose(fp);

    gotoxy(20, 10);
    printf(" Contact Added Successfully!");
    usleep(1000000);
    menu();
}

// List Contacts
void listContacts() {
    system("clear");
    FILE *fp;
    char name[50], address[50], gender[10], gmail[50];
    double phone;

    fp = fopen("contacts.txt", "r");
    if (fp == NULL) {
        gotoxy(25, 4);
        printf("No contacts found!");
        getchar();
        menu();
        return;
    }

    gotoxy(25, 2);
    printf(" CONTACT LIST");
    printf("\n-----------------------------------------------------------");

    while (fscanf(fp, "%s %s %s %s %lf", name, address, gender, gmail, &phone) != EOF) {
        printf("\nName: %s\nAddress: %s\nGender: %s\nGmail: %s\nPhone: %.0lf\n",
               name, address, gender, gmail, phone);
        printf("-----------------------------------------------------------\n");
    }

    fclose(fp);
    printf("Press any key to return to menu...");
    getchar();
    menu();
}

// Exit Function
void exitProgram() {
    system("clear");
    gotoxy(25, 4);
    printf("📞 Thank you for using the Phonebook! 📖");
    gotoxy(25, 6);
    printf("Exiting... Goodbye! 👋");
    usleep(1500000);
    system("clear");
    exit(0);
}

// Menu Function
void menu() {
    system("clear");
    gotoxy(30, 2);
    printf("📞 PHONEBOOK MENU 📞");
    gotoxy(30, 4);
    printf("1. Add Contact");
    gotoxy(30, 5);
    printf("2. List Contacts");
    gotoxy(30, 6);
    printf("3. Exit");
    gotoxy(30, 8);
    printf("Enter your choice: ");

    char choice = getchar();
    getchar(); // Clear newline

    switch (choice) {
        case '1': addContact(); break;
        case '2': listContacts(); break;
        case '3': exitProgram(); break;
        default:
            gotoxy(30, 10);
            printf(" Invalid Choice! Try Again...");
            usleep(1000000);
            menu();
    }
}
