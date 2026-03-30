#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#define k 2

struct Node {
    float point[k];
    struct Node *left, *right;
};

struct Node* newNode(float arr[])
{
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));

    for(int i=0;i<k;i++)
        temp->point[i] = arr[i];

    temp->left = temp->right = NULL;

    return temp;
}

struct Node* insert(struct Node* root, float point[], int depth)
{
    if(root == NULL)
        return newNode(point);

    int cd = depth % k;

    if(point[cd] < root->point[cd])
        root->left = insert(root->left, point, depth+1);
    else
        root->right = insert(root->right, point, depth+1);

    return root;
}

float distanceSquared(float p1[], float p2[])
{
    float dist = 0;

    for(int i=0;i<k;i++)
        dist += (p1[i]-p2[i])*(p1[i]-p2[i]);

    return dist;
}

void nearestNeighbor(struct Node* root, float target[], int depth,
                     struct Node** best, float* bestDist)
{
    if(root == NULL)
        return;

    float d = distanceSquared(root->point, target);

    if(d < *bestDist)
    {
        *bestDist = d;
        *best = root;
    }

    int cd = depth % k;

    struct Node *nextBranch = NULL;
    struct Node *otherBranch = NULL;

    if(target[cd] < root->point[cd])
    {
        nextBranch = root->left;
        otherBranch = root->right;
    }
    else
    {
        nextBranch = root->right;
        otherBranch = root->left;
    }

    nearestNeighbor(nextBranch, target, depth+1, best, bestDist);

    float diff = target[cd] - root->point[cd];

    if(diff*diff < *bestDist)
        nearestNeighbor(otherBranch, target, depth+1, best, bestDist);
}

int search(struct Node* root, float point[], int depth)
{
    if(root == NULL)
        return 0;

    if(root->point[0]==point[0] && root->point[1]==point[1])
        return 1;

    int cd = depth % k;

    if(point[cd] < root->point[cd])
        return search(root->left, point, depth+1);
    else
        return search(root->right, point, depth+1);
}

void displayTree(struct Node* root, int level)
{
    if(root == NULL)
        return;

    for(int i = 0; i < level; i++)
        printf("   ");

    printf("(%.2f, %.2f)\n", root->point[0], root->point[1]);

    if(root->left)
    {
        for(int i = 0; i < level; i++)
            printf("   ");
        printf("L-> ");
        displayTree(root->left, level + 1);
    }

    if(root->right)
    {
        for(int i = 0; i < level; i++)
            printf("   ");
        printf("R-> ");
        displayTree(root->right, level + 1);
    }
}

void loadIrisData(struct Node **root)
{
    FILE *fp = fopen("iris.txt","r");

    if(fp == NULL)
    {
        printf("Error opening iris dataset file.\n");
        return;
    }

    float point[2];

    while(fscanf(fp,"%f %f",&point[0],&point[1]) == 2)
    {
        *root = insert(*root, point, 0);
    }

    fclose(fp);

    printf("Iris dataset loaded into KD Tree.\n");
}

int main()
{
    struct Node* root = NULL;
    int choice;

    while(1)
    {
        printf("\n--- KD TREE MENU ---\n");
        printf("1. Insert Point\n");
        printf("2. Search Point\n");
        printf("3. Find Nearest Neighbor\n");
        printf("4. Display Tree\n");
        printf("5. Load Iris Dataset\n");
        printf("6. Exit\n");

        printf("Enter choice: ");
        scanf("%d",&choice);

        if(choice==1)
        {
            float point[2];

            printf("Enter x y: ");
            scanf("%f %f",&point[0],&point[1]);

            root = insert(root,point,0);

            printf("Point inserted.\n");
        }

        else if(choice==2)
        {
            float point[2];

            printf("Enter x y: ");
            scanf("%f %f",&point[0],&point[1]);

            if(search(root,point,0))
                printf("Point found.\n");
            else
                printf("Point not found.\n");
        }

        else if(choice==3)
        {
            float target[2];

            printf("Enter target x y: ");
            scanf("%f %f",&target[0],&target[1]);

            struct Node* best = NULL;
            float bestDist = FLT_MAX;

            nearestNeighbor(root,target,0,&best,&bestDist);

            if(best!=NULL)
                printf("Nearest Neighbor: (%.2f, %.2f)\n",
                       best->point[0],best->point[1]);
        }

        else if(choice==4)
        {
            printf("\nKD Tree Structure:\n");
            displayTree(root,0);
        }

        else if(choice==5)
        {
            loadIrisData(&root);
        }

        else if(choice==6)
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