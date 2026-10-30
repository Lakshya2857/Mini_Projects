
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 50

// Student record and BST node
typedef struct Student {
    int id;
    char name[MAX];
    char dept[MAX];
    struct Student *left;
    struct Student *right;
} Student;

// BST structure
typedef struct {
    Student *root;
    long long comparisons;
} BST;

// Create new student node
Student* createStudent(int id, const char name[], const char dept[]) {
    Student *node = (Student*)malloc(sizeof(Student));

    if (node == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    node->id = id;
    snprintf(node->name, MAX, "%s", name);
    snprintf(node->dept, MAX, "%s", dept);

    node->left = NULL;
    node->right = NULL;

    return node;
}

// Recursive insertion
Student* insertNode(BST *tree, Student *root,
                    int id, const char name[],
                    const char dept[], int *ok) {

    if (root == NULL) {
        *ok = 1;
        return createStudent(id, name, dept);
    }

    tree->comparisons++;

    if (id < root->id) {
        root->left = insertNode(tree, root->left,
                                id, name, dept, ok);
    }
    else if (id > root->id) {
        root->right = insertNode(tree, root->right,
                                 id, name, dept, ok);
    }
    else {
        *ok = 0;
    }

    return root;
}

// Insert student
void insert(BST *tree, int id,
            const char name[], const char dept[]) {

    int ok = 0;

    tree->root = insertNode(tree, tree->root,
                            id, name, dept, &ok);

    if (ok) {
        printf("Inserted: %d - %s\n", id, name);
    }
    else {
        printf("Duplicate! Enrolment ID %d already exists.\n", id);
    }
}

// Search student
Student* search(Student *root, int id) {

    if (root == NULL || root->id == id) {
        return root;
    }

    if (id < root->id) {
        return search(root->left, id);
    }

    return search(root->right, id);
}

// Display student
void displayStudent(Student *node) {

    if (node != NULL) {
        printf("\nStudent Found!\n");
        printf("ID: %d\n", node->id);
        printf("Name: %s\n", node->name);
        printf("Department: %s\n", node->dept);
    }
}

// Find minimum node
Student* minNode(Student *root) {

    while (root != NULL && root->left != NULL) {
        root = root->left;
    }

    return root;
}

// Delete student
Student* deleteStudent(Student *root, int id, int *found) {

    if (root == NULL) {
        return NULL;
    }

    if (id < root->id) {
        root->left = deleteStudent(root->left, id, found);
    }
    else if (id > root->id) {
        root->right = deleteStudent(root->right, id, found);
    }
    else {

        *found = 1;

        // Case 1: Leaf node
        if (root->left == NULL && root->right == NULL) {
            printf("(Case: Leaf node)\n");
            free(root);
            return NULL;
        }

        // Case 2: Only right child
        if (root->left == NULL) {
            printf("(Case: One right child)\n");

            Student *temp = root->right;
            free(root);
            return temp;
        }

        // Case 2: Only left child
        if (root->right == NULL) {
            printf("(Case: One left child)\n");

            Student *temp = root->left;
            free(root);
            return temp;
        }

        // Case 3: Two children
        printf("(Case: Two children)\n");

        Student *temp = minNode(root->right);

        root->id = temp->id;
        strcpy(root->name, temp->name);
        strcpy(root->dept, temp->dept);

        int dummy = 0;

        root->right = deleteStudent(root->right,
                                    temp->id, &dummy);
    }

    return root;
}

// Inorder traversal
void inorder(Student *root) {

    if (root != NULL) {
        inorder(root->left);

        printf("%d | %s | %s\n",
               root->id, root->name, root->dept);

        inorder(root->right);
    }
}

// Preorder traversal
void preorder(Student *root) {

    if (root != NULL) {

        printf("%d | %s | %s\n",
               root->id, root->name, root->dept);

        preorder(root->left);
        preorder(root->right);
    }
}

// Postorder traversal
void postorder(Student *root) {

    if (root != NULL) {

        postorder(root->left);
        postorder(root->right);

        printf("%d | %s | %s\n",
               root->id, root->name, root->dept);
    }
}

// Calculate tree height
int height(Student *root) {

    if (root == NULL) {
        return 0;
    }

    int l = height(root->left);
    int r = height(root->right);

    return 1 + (l > r ? l : r);
}

// Count students
int countStudents(Student *root) {

    if (root == NULL) {
        return 0;
    }

    return 1 + countStudents(root->left)
             + countStudents(root->right);
}

// Insert 10 sample records
void insertSamples(BST *tree) {

    int ids[] = {50, 30, 70, 20, 40,
                 60, 80, 35, 45, 65};

    char *names[] = {
        "Aman", "Riya", "Karan", "Simran", "Rahul",
        "Neha", "Arjun", "Pooja", "Vikram", "Anjali"
    };

    char *depts[] = {
        "CSE", "IT", "ECE", "CSE", "ME",
        "IT", "CSE", "ECE", "CSE", "IT"
    };

    for (int i = 0; i < 10; i++) {
        insert(tree, ids[i], names[i], depts[i]);
    }
}

// Compare balanced and sorted insertion
void compareOrders() {

    int balanced[] = {
        50, 30, 70, 20, 40, 60, 80,
        10, 25, 35, 45, 65, 75, 90, 5
    };

    int sorted[] = {
        5, 10, 20, 25, 30, 35, 40,
        45, 50, 60, 65, 70, 75, 80, 90
    };

    BST b = {NULL, 0};
    BST s = {NULL, 0};

    for (int i = 0; i < 15; i++) {
        char name[] = "X";
        char dept[] = "Y";

        b.root = insertNode(&b, b.root,
                            balanced[i], name, dept,
                            &(int){0});

        s.root = insertNode(&s, s.root,
                            sorted[i], name, dept,
                            &(int){0});
    }

    printf("\nInserting 15 records:\n");

    printf("Balanced-looking order -> Comparisons: %lld, Height: %d\n",
           b.comparisons, height(b.root));

    printf("Sorted order -> Comparisons: %lld, Height: %d\n",
           s.comparisons, height(s.root));

    // Free temporary trees
    // (done using the helper below)
    // See freeTree declaration below.
}

// Free allocated memory
void freeTree(Student *root) {

    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

// Main function
int main() {

    BST tree = {NULL, 0};

    int choice, id;
    int found;

    char name[MAX];
    char dept[MAX];

    do {

        printf("\n====================================\n");
        printf("    STUDENT RECORD SEARCH SYSTEM\n");
        printf("====================================\n");

        printf("1. Insert Student\n");
        printf("2. Search Student\n");
        printf("3. Delete Student\n");
        printf("4. Inorder Traversal\n");
        printf("5. Preorder Traversal\n");
        printf("6. Postorder Traversal\n");
        printf("7. Load 10 Sample Records\n");
        printf("8. Compare Balanced vs Sorted Insertion\n");
        printf("9. Show Tree Height\n");
        printf("10. Count Students\n");
        printf("11. Display Insertion Comparisons\n");
        printf("12. Reset Comparison Counter\n");
        printf("0. Exit\n");

        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            freeTree(tree.root);
            return 1;
        }

        switch (choice) {

            case 1:

                printf("Enter enrolment ID: ");
                scanf("%d", &id);

                printf("Enter student name: ");
                scanf(" %49[^\n]", name);

                printf("Enter department: ");
                scanf(" %49[^\n]", dept);

                insert(&tree, id, name, dept);
                break;

            case 2:

                printf("Enter ID to search: ");
                scanf("%d", &id);

                found = 0;
                Student *result = search(tree.root, id);

                if (result != NULL) {
                    displayStudent(result);
                }
                else {
                    printf("Student not found!\n");
                }
                break;

            case 3:

                printf("Enter ID to delete: ");
                scanf("%d", &id);

                found = 0;

                tree.root = deleteStudent(tree.root, id, &found);

                if (found) {
                    printf("Student deleted successfully.\n");
                }
                else {
                    printf("Student not found!\n");
                }
                break;

            case 4:

                printf("\nInorder Traversal:\n");
                inorder(tree.root);
                break;

            case 5:

                printf("\nPreorder Traversal:\n");
                preorder(tree.root);
                break;

            case 6:

                printf("\nPostorder Traversal:\n");
                postorder(tree.root);
                break;

            case 7:

                insertSamples(&tree);

                printf("Sample records processed.\n");
                printf("Total students: %d\n",
                       countStudents(tree.root));
                break;

            case 8:

                compareOrders();
                break;

            case 9:

                printf("Tree Height: %d\n",
                       height(tree.root));
                break;

            case 10:

                printf("Total Students: %d\n",
                       countStudents(tree.root));
                break;

            case 11:

                printf("Insertion Comparisons: %lld\n",
                       tree.comparisons);
                break;

            case 12:

                tree.comparisons = 0;
                printf("Comparison counter reset.\n");
                break;

            case 0:

                printf("Exiting Student Record Search System.\n");
                break;

            default:

                printf("Invalid choice!\n");
        }

    } while (choice != 0);

    freeTree(tree.root);

    return 0;
}
