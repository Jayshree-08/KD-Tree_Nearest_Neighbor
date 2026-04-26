#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>

#define K_DIM 4

struct Node {
    float point[K_DIM];
    char class_name[30];
    struct Node *left, *right;
};

struct Neighbor {
    float distance;
    struct Node* node;
};

struct Node* newNode(float arr[], const char* c_name)
{
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    for(int i=0; i<K_DIM; i++)
        temp->point[i] = arr[i];
    strcpy(temp->class_name, c_name);
    temp->left = temp->right = NULL;
    return temp;
}

struct Node* insert(struct Node* root, float point[], const char* c_name, int depth)
{
    if(root == NULL)
        return newNode(point, c_name);

    int cd = depth % K_DIM;
    if(point[cd] < root->point[cd])
        root->left = insert(root->left, point, c_name, depth+1);
    else
        root->right = insert(root->right, point, c_name, depth+1);

    return root;
}

float distanceSquared(float p1[], float p2[])
{
    float dist = 0;
    for(int i=0; i<K_DIM; i++)
        dist += (p1[i]-p2[i])*(p1[i]-p2[i]);
    return dist;
}

// Helper to add a neighbor to a sorted array of neighbors
void addNeighbor(struct Neighbor neighbors[], int k_neighbors, int *count, struct Node* node, float dist)
{
    // If not full, just insert and sort
    if (*count < k_neighbors) {
        neighbors[*count].distance = dist;
        neighbors[*count].node = node;
        (*count)++;
        
        // Bubble sort to keep array sorted (ascending by distance)
        for(int i=0; i<(*count)-1; i++) {
            for(int j=0; j<(*count)-i-1; j++) {
                if(neighbors[j].distance > neighbors[j+1].distance) {
                    struct Neighbor temp = neighbors[j];
                    neighbors[j] = neighbors[j+1];
                    neighbors[j+1] = temp;
                }
            }
        }
    } else {
        // If full, check if dist is smaller than the largest distance (last element)
        if (dist < neighbors[k_neighbors-1].distance) {
            neighbors[k_neighbors-1].distance = dist;
            neighbors[k_neighbors-1].node = node;
            
            // Re-sort
            for(int i=0; i<k_neighbors-1; i++) {
                for(int j=0; j<k_neighbors-i-1; j++) {
                    if(neighbors[j].distance > neighbors[j+1].distance) {
                        struct Neighbor temp = neighbors[j];
                        neighbors[j] = neighbors[j+1];
                        neighbors[j+1] = temp;
                    }
                }
            }
        }
    }
}

void kNearestNeighbors(struct Node* root, float target[], int depth,
                       struct Neighbor neighbors[], int k_neighbors, int *count)
{
    if(root == NULL)
        return;

    float d = distanceSquared(root->point, target);
    addNeighbor(neighbors, k_neighbors, count, root, d);

    int cd = depth % K_DIM;
    struct Node *nextBranch = NULL;
    struct Node *otherBranch = NULL;

    if(target[cd] < root->point[cd]) {
        nextBranch = root->left;
        otherBranch = root->right;
    } else {
        nextBranch = root->right;
        otherBranch = root->left;
    }

    kNearestNeighbors(nextBranch, target, depth+1, neighbors, k_neighbors, count);

    float diff = target[cd] - root->point[cd];
    
    // Check if we need to explore the other branch
    // If we don't have enough neighbors yet, or if the bounding box intersects the search sphere
    float current_worst_dist = (*count < k_neighbors) ? FLT_MAX : neighbors[k_neighbors-1].distance;

    if(diff*diff < current_worst_dist) {
        kNearestNeighbors(otherBranch, target, depth+1, neighbors, k_neighbors, count);
    }
}

void classifyPoint(struct Node* root, float target[], int k_neighbors)
{
    if (root == NULL) {
        printf("Tree is empty.\n");
        return;
    }

    struct Neighbor* neighbors = (struct Neighbor*)malloc(k_neighbors * sizeof(struct Neighbor));
    int count = 0;

    kNearestNeighbors(root, target, 0, neighbors, k_neighbors, &count);

    printf("\n--- Top %d Nearest Neighbors ---\n", count);
    for(int i=0; i<count; i++) {
        printf("%d: Distance = %.4f | Point = (", i+1, neighbors[i].distance);
        for(int j=0; j<K_DIM; j++) {
            printf("%.2f%s", neighbors[i].node->point[j], (j==K_DIM-1)?"":", ");
        }
        printf(") | Class = %s\n", neighbors[i].node->class_name);
    }

    // Majority voting
    char unique_classes[100][30];
    int class_counts[100] = {0};
    int num_unique = 0;

    for(int i=0; i<count; i++) {
        char* c_name = neighbors[i].node->class_name;
        int found = 0;
        for(int j=0; j<num_unique; j++) {
            if(strcmp(unique_classes[j], c_name) == 0) {
                class_counts[j]++;
                found = 1;
                break;
            }
        }
        if(!found) {
            strcpy(unique_classes[num_unique], c_name);
            class_counts[num_unique] = 1;
            num_unique++;
        }
    }

    int max_count = 0;
    int best_idx = -1;
    for(int i=0; i<num_unique; i++) {
        if(class_counts[i] > max_count) {
            max_count = class_counts[i];
            best_idx = i;
        }
    }

    printf("\n=> Predicted Class (Majority Vote): %s (with %d/%d votes)\n", unique_classes[best_idx], max_count, count);

    free(neighbors);
}

void loadIrisData(struct Node **root)
{
    FILE *fp = fopen("iris.txt","r");
    if(fp == NULL) {
        printf("Error opening iris.txt file. Make sure it's in the same directory.\n");
        return;
    }

    float point[K_DIM];
    char class_name[30];
    int count = 0;

    // Read comma-separated values from iris.txt
    while(fscanf(fp,"%f,%f,%f,%f,%29s", &point[0], &point[1], &point[2], &point[3], class_name) == 5) {
        *root = insert(*root, point, class_name, 0);
        count++;
    }

    fclose(fp);
    printf("Successfully loaded %d points from Iris dataset into KD Tree.\n", count);
}

void displayTree(struct Node* root, int level)
{
    if(root == NULL)
        return;
    for(int i = 0; i < level; i++) printf("   ");
    printf("(");
    for(int i=0; i<K_DIM; i++) printf("%.2f%s", root->point[i], (i==K_DIM-1)?"":", ");
    printf(") [%s]\n", root->class_name);

    if(root->left) {
        for(int i = 0; i < level; i++) printf("   ");
        printf("L-> \n");
        displayTree(root->left, level + 1);
    }
    if(root->right) {
        for(int i = 0; i < level; i++) printf("   ");
        printf("R-> \n");
        displayTree(root->right, level + 1);
    }
}

int main()
{
    struct Node* root = NULL;
    int choice;

    while(1)
    {
        printf("\n--- KD TREE K-NN MENU ---\n");
        printf("1. Insert Point\n");
        printf("2. Classify Point (Find K Nearest & Group)\n");
        printf("3. Display Tree\n");
        printf("4. Load Iris Dataset (iris.txt)\n");
        printf("5. Exit\n");

        printf("Enter choice: ");
        if (scanf("%d",&choice) != 1) {
            printf("Invalid input.\n");
            break;
        }

        if(choice==1)
        {
            float point[K_DIM];
            char class_name[30];

            printf("Enter %d features (space-separated): ", K_DIM);
            for(int i=0; i<K_DIM; i++) scanf("%f", &point[i]);
            
            printf("Enter class name: ");
            scanf("%s", class_name);

            root = insert(root, point, class_name, 0);
            printf("Point inserted.\n");
        }
        else if(choice==2)
        {
            float target[K_DIM];
            int k_neighbors;

            printf("Enter target %d features (space-separated): ", K_DIM);
            for(int i=0; i<K_DIM; i++) scanf("%f", &target[i]);

            printf("Enter value of K (e.g., 5): ");
            scanf("%d", &k_neighbors);

            if(k_neighbors <= 0) {
                printf("K must be greater than 0.\n");
            } else {
                classifyPoint(root, target, k_neighbors);
            }
        }
        else if(choice==3)
        {
            printf("\nKD Tree Structure:\n");
            displayTree(root,0);
        }
        else if(choice==4)
        {
            loadIrisData(&root);
        }
        else if(choice==5)
        {
            break;
        }
        else
        {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}