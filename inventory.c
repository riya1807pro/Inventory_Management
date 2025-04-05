#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_ITEMS 100
#define USER_FILE "users.txt"

typedef struct {
    int id;
    char name[50];
    char category[30];
    int quantity;
    float price;
} Item;

Item inventory[MAX_ITEMS];
int itemCount = 0;
char currentUser[50];
char inventoryFile[100];

// ANSI Colors
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

// Function Prototypes
void saveToFile();
void loadFromFile();
void addItem();
void viewItems();
void searchItem();
void deleteItem();
void updateItem();
void sortItems();
void exportToCSV();
void lowStockAlert();
bool login();
void registerUser();
bool validateUser(const char* user, const char* pass);
void showMenu();

// Save/load inventory for current user
void saveToFile() {
    FILE *fp = fopen(inventoryFile, "w");
    for (int i = 0; i < itemCount; i++) {
        fprintf(fp, "%d %s %s %d %.2f\n",
            inventory[i].id,
            inventory[i].name,
            inventory[i].category,
            inventory[i].quantity,
            inventory[i].price);
    }
    fclose(fp);
}

void loadFromFile() {
    FILE *fp = fopen(inventoryFile, "r");
    if (!fp) return;
    while (fscanf(fp, "%d %s %s %d %f",
                  &inventory[itemCount].id,
                  inventory[itemCount].name,
                  inventory[itemCount].category,
                  &inventory[itemCount].quantity,
                  &inventory[itemCount].price) == 5) {
        itemCount++;
    }
    fclose(fp);
}

// User Registration
void registerUser() {
    char username[50], password[50];
    printf(CYAN "\nRegister a new user\n" RESET);
    printf("Enter Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    FILE *fp = fopen(USER_FILE, "a");
    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);
    printf(GREEN "✅ User registered! Now login.\n" RESET);
}

// Validate login
bool validateUser(const char* user, const char* pass) {
    char u[50], p[50];
    FILE *fp = fopen(USER_FILE, "r");
    if (!fp) return false;

    while (fscanf(fp, "%s %s", u, p) != EOF) {
        if (strcmp(user, u) == 0 && strcmp(pass, p) == 0) {
            fclose(fp);
            return true;
        }
    }
    fclose(fp);
    return false;
}

// Login Function
bool login() {
    char username[50], password[50];
    int choice;

    while (1) {
        printf(YELLOW "\n1. Register\n2. Login\nChoose: " RESET);
        scanf("%d", &choice);

        if (choice == 1) {
            registerUser();
        } else if (choice == 2) {
            printf("Username: ");
            scanf("%s", username);
            printf("Password: ");
            scanf("%s", password);

            if (validateUser(username, password)) {
                strcpy(currentUser, username);
                sprintf(inventoryFile, "%s_inventory.txt", username);
                printf(GREEN "✅ Login successful!\n" RESET);
                return true;
            } else {
                printf(RED "❌ Invalid credentials!\n" RESET);
            }
        } else {
            printf(RED "❌ Invalid option!\n" RESET);
        }
    }
}

// Inventory Features (Same as before, now user-specific)

void addItem() {
    Item newItem;
    printf(GREEN "\nEnter Item ID: " RESET); scanf("%d", &newItem.id);
    printf("Enter Item Name: "); scanf("%s", newItem.name);
    printf("Enter Category: "); scanf("%s", newItem.category);
    printf("Enter Quantity: "); scanf("%d", &newItem.quantity);
    printf("Enter Price: "); scanf("%f", &newItem.price);
    inventory[itemCount++] = newItem;
    printf(GREEN "✅ Item added successfully!\n" RESET);
}

void viewItems() {
    printf(YELLOW "\n%-5s %-15s %-12s %-10s %-10s\n", "ID", "Name", "Category", "Quantity", "Price");
    printf("----------------------------------------------------------\n" RESET);
    for (int i = 0; i < itemCount; i++) {
        printf("%-5d %-15s %-12s %-10d %-10.2f\n",
            inventory[i].id,
            inventory[i].name,
            inventory[i].category,
            inventory[i].quantity,
            inventory[i].price);
    }
    lowStockAlert();
}

void searchItem() {
    char keyword[50];
    printf(CYAN "\nEnter Item Name or ID to search: " RESET);
    scanf("%s", keyword);
    int found = 0;
    for (int i = 0; i < itemCount; i++) {
        if (strstr(inventory[i].name, keyword) || atoi(keyword) == inventory[i].id) {
            printf(GREEN "🔍 Found: %s (%s), Qty: %d, Price: %.2f\n" RESET,
                inventory[i].name, inventory[i].category,
                inventory[i].quantity, inventory[i].price);
            found = 1;
        }
    }
    if (!found) printf(RED "❌ Item not found!\n" RESET);
}

void deleteItem() {
    int id;
    printf(RED "\nEnter Item ID to delete: " RESET);
    scanf("%d", &id);
    for (int i = 0; i < itemCount; i++) {
        if (inventory[i].id == id) {
            for (int j = i; j < itemCount - 1; j++) {
                inventory[j] = inventory[j + 1];
            }
            itemCount--;
            printf(GREEN "✅ Item deleted!\n" RESET);
            return;
        }
    }
    printf(RED "❌ Item not found!\n" RESET);
}

void updateItem() {
    int id;
    printf(YELLOW "\nEnter Item ID to update: " RESET);
    scanf("%d", &id);
    for (int i = 0; i < itemCount; i++) {
        if (inventory[i].id == id) {
            printf("Enter new Name (old: %s): ", inventory[i].name); scanf("%s", inventory[i].name);
            printf("Enter new Category (old: %s): ", inventory[i].category); scanf("%s", inventory[i].category);
            printf("Enter new Quantity (old: %d): ", inventory[i].quantity); scanf("%d", &inventory[i].quantity);
            printf("Enter new Price (old: %.2f): ", inventory[i].price); scanf("%f", &inventory[i].price);
            printf(GREEN "✅ Item updated!\n" RESET);
            return;
        }
    }
    printf(RED "❌ Item not found!\n" RESET);
}

void sortItems() {
    int choice;
    printf(YELLOW "\nSort By:\n1. Name\n2. Quantity\n3. Price\nEnter choice: " RESET);
    scanf("%d", &choice);
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = i + 1; j < itemCount; j++) {
            int swap = 0;
            switch (choice) {
                case 1: if (strcmp(inventory[i].name, inventory[j].name) > 0) swap = 1; break;
                case 2: if (inventory[i].quantity > inventory[j].quantity) swap = 1; break;
                case 3: if (inventory[i].price > inventory[j].price) swap = 1; break;
                default: printf(RED "❌ Invalid choice\n" RESET); return;
            }
            if (swap) {
                Item temp = inventory[i];
                inventory[i] = inventory[j];
                inventory[j] = temp;
            }
        }
    }
    printf(GREEN "✅ Sorted successfully!\n" RESET);
    viewItems();
}

void exportToCSV() {
    char filename[100];
    sprintf(filename, "%s_inventory.csv", currentUser);
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "ID,Name,Category,Quantity,Price\n");
    for (int i = 0; i < itemCount; i++) {
        fprintf(fp, "%d,%s,%s,%d,%.2f\n",
            inventory[i].id,
            inventory[i].name,
            inventory[i].category,
            inventory[i].quantity,
            inventory[i].price);
    }
    fclose(fp);
    printf(GREEN "✅ Exported to %s\n" RESET, filename);
}

void lowStockAlert() {
    int flag = 0;
    for (int i = 0; i < itemCount; i++) {
        if (inventory[i].quantity < 5) {
            if (!flag) {
                printf(RED "\n⚠️  Low Stock Items:\n" RESET);
                flag = 1;
            }
            printf(RED "- %s (Qty: %d)\n" RESET, inventory[i].name, inventory[i].quantity);
        }
    }
}

void showMenu() {
    printf(YELLOW "\n============================================\n" RESET);
    printf(CYAN   "    Welcome, %s's Inventory Manager\n" RESET, currentUser);
    printf(YELLOW "============================================\n" RESET);
    printf("1. Add Item\n");
    printf("2. View Items\n");
    printf("3. Search Item\n");
    printf("4. Delete Item\n");
    printf("5. Update Item\n");
    printf("6. Sort Items\n");
    printf("7. Export to CSV\n");
    printf("8. Exit\n");
    printf(YELLOW "--------------------------------------------\n" RESET);
    printf("Enter choice: ");
}

int main() {
    int choice;

    if (!login()) return 0;

    loadFromFile();

    while (1) {
        showMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: addItem(); break;
            case 2: viewItems(); break;
            case 3: searchItem(); break;
            case 4: deleteItem(); break;
            case 5: updateItem(); break;
            case 6: sortItems(); break;
            case 7: exportToCSV(); break;
            case 8:
                saveToFile();
                printf(GREEN "✅ Data saved. Exiting...\n" RESET);
                exit(0);
            default: printf(RED "❌ Invalid choice! Try again.\n" RESET);
        }
    }

    return 0;
}
