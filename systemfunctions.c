# include "settings.h"
#define MAX_LOCATIONS 30
#define MAX_DIST 9999

int choice               =0; // defines the choice from the user
int choice2              =0; // defines the choice from the user for the second menu
int choice3              =0; // Defines the choice from the user for the third menu
int choice4              =0; // Defines the choice from the user for the fourth meu
int location_count       =0; // number of locations in the file
int last_location        =0; //The location number of the last location in the file
// this number will be incremented but never decremented as new locations will always be added with a new number
int src                  =0; // source for dijkstra's
int destination          =0; // destination of the user
int activity             =0; // Activity number chosen by the user
int route_len            =0; // The length of the shortest path calculated by dijkstra's
int no_of_close_locations=0; // Number of close locations found by BFS

// Structure to store all the information from the files and insert into the tree
struct location_data
{
    int location_no;
    char name[30];
    char category[30];
    int rating;
    int cost[MAX_LOCATIONS];
    int dist[MAX_LOCATIONS];
    int time[MAX_LOCATIONS];
    struct location_data *right;
    struct location_data *left;
    int height;
};
typedef struct location_data ld;

//Heap structure to heapify in dijkstras
struct heap
{
    int dist;
    int path;
    int index;
};
typedef struct heap HP;

//Structure to store the result of dijkstra
struct PATH_DIST
{
    int PATH[MAX_LOCATIONS];
    int DIST[MAX_LOCATIONS];
};
typedef struct PATH_DIST PD;

//to store the information and sort the array
struct sorting_info_array
{
    int location_number;
    int rating;
    int cost;
    int time;
    int dist;
};
typedef struct sorting_info_array SIA;

int cost_matrix[MAX_LOCATIONS][MAX_LOCATIONS];
int distance_matrix[MAX_LOCATIONS][MAX_LOCATIONS];
int time_matrix[MAX_LOCATIONS][MAX_LOCATIONS];
int adjacency_matrix[MAX_LOCATIONS][MAX_LOCATIONS];

PD least_distance; //Result of dijkstra for the distance matrix
PD least_cost;    //Result of dijkstra for the cost matrix
PD least_time;   //Result of dijkstra for the time matrix

HP Heap[MAX_LOCATIONS]; // heap declaration

int distance_path[MAX_LOCATIONS]; // shortest bath based on distance
int cost_path[MAX_LOCATIONS];    // shortest bath based on cost
int time_path[MAX_LOCATIONS];   // shortest bath based on time

int close_by_locations[MAX_LOCATIONS];

ld * root=NULL;//root node of the tree

int BIT[MAX_LOCATIONS];//Binary Indexed Tree array/ fenwick tree array
//default values will be zero due to global declaration
int fenwick_tree_array[MAX_LOCATIONS];

SIA sortingArray[MAX_LOCATIONS];

int floyd[MAX_LOCATIONS][MAX_LOCATIONS]; //Matrix to apply floyds algorithm

int lookup[MAX_LOCATIONS][MAX_LOCATIONS]; // Matrix to create a lookup table

/*
Function Name: welcome
Input Params:  NILL
Return Type:   void
Description:   displays a welcome message
*/
void welcome()
{
    int  i = 0;
    for(i=0;i<40;i++)
        printf("_-");

    printf("\n\n\t\t\t\tMUMBAI TOUR GUIDE\n\n\n");

    for(i=0;i<40;i++)
        printf("_-");

    printf("\n\n");

}

/*
Function Name: writeLogAdd
Input Params:  NILL
Return Type:   void
Description:   writes in the log file when a location node is added
*/
void writeLogAdd()
{
    FILE* logFile = fopen("logfile.txt", "a+");
    if (logFile != NULL)
    {
        time_t currentTime;
        time(&currentTime);
        // Write the timestamp to the log file
        fprintf(logFile, "[%ld] New node added\n", (long)currentTime);
        fclose(logFile);
    }
    else
    {
        printf("Error opening log file.\n");
        exit(FILE_OPEN_ERROR);
    }
}

/*
Function Name: writeLogDelete
Input Params:  NILL
Return Type:   void
Description:   write in the log file when a location node is deleted
*/
void writeLogDelete()
{
    FILE* logFile = fopen("logfile.txt", "a+");
    if (logFile != NULL)
    {
        time_t currentTime;
        time(&currentTime);
        // Write the timestamp to the log file
        fprintf(logFile, "[%ld] Node deleted\n", (long)currentTime);
        fclose(logFile);
    }
    else
    {
        printf("Error opening log file.\n");
        exit(FILE_OPEN_ERROR);
    }
}

/*
Function Name: initialize_matrices
Input Params:  NILL
Return Type:   void
Description:   initializes the cost,distance and time matrices
               with 9999 to represent infinity for applying dijkstra's algorithm
*/
void initialize_matrices()
{
    int i,j;
    for(i=0;i<20;i++)
    {
        for(j=0;j<20;j++)
        {
            cost_matrix[i][j]=MAX_DIST;
            distance_matrix[i][j]=MAX_DIST;
            time_matrix[i][j]=MAX_DIST;
            //printf("%d ",cost_matrix[i][j]);
        }
        //printf("\n");
    }
}


/*
Function Name: height
Input Params:  Root of the tree
Return Type:   integer,height of the tree
Description:   recursively calculates the height of the tree by
               calculating the height of the the left and right sub-trees
*/
int height(ld *N)
{
	if (N == NULL)
		return 0;
	return N->height;
}

/*
Function Name: max
Input Params:  two integers a and b
Return Type:   integer
Description:   returns the integer with maximum value
*/
int max(int a, int b)
{
	return (a > b)? a : b;
}

/*
Function Name: min
Input Params:  integers a and b
Return Type:   integer
Description:   returns the integer with the smaller value
*/
int min(int a, int b)
{
	return (a < b)? a : b;
}

/*
Function Name: rightRotate
Input Params:  root of the sub-tree
Return Type:   new root of the subtree after rotation
Description:   rotates the subtree in the clockwise direction
*/
ld* rightRotate(ld *y)
{
	ld *x = y->left;
	ld *z = x->right;

	// Perform rotation
	x->right = y;
	y->left = z;

	// Update heights
	y->height = max(height(y->left),
					height(y->right)) + 1;
	x->height = max(height(x->left),
					height(x->right)) + 1;

	// Return new root
	return x;
}

/*
Function Name: leftRotate
Input Params:  root
Return Type:   Updated root of the subtree after rotation
Description:   rotates the subtree in counter-clockwise
               direction to maintain its balance factor
*/
ld *leftRotate(ld *x)
{
	ld *y = x->right;
	ld *z = y->left;

	// Perform rotation
	y->left = x;
	x->right = z;

	// Update heights
	x->height = max(height(x->left),
					height(x->right)) + 1;
	y->height = max(height(y->left),
					height(y->right)) + 1;

	// Return new root
	return y;
}

/*
Function Name: getBalance
Input Params:  Root of the subtree
Return Type:   integer
Description:   calculates the balance-factor of each node of the
               of the tree by subtracting the height of the right subtree
               from the height of the left subtree
*/
int getBalance(ld *N)
{
	if (N == NULL)
		return 0;
	return height(N->left) - height(N->right);
}

/*
Function Name: newNode
Input Params:  name string,category string,integer rating of the location,
               distance array representing the distance from the location to every other location
               ,a similar array for cost and time
Return Type:   the pointer of the node with the given information
Description:   creates a new node with the given information
               to be inserted in the tree
*/
ld* newNode(int no,char name[30],char category[30],int rate,int dist[MAX_LOCATIONS],int cost[MAX_LOCATIONS],int time[MAX_LOCATIONS])
	{
        struct location_data* node = (struct location_data*)
						malloc(sizeof(struct location_data));
	    node->location_no = no;
	    strcpy(node->name,name);
	    strcpy(node->category,category);
	    node->rating=rate;
	    int i;
	    for(i=0;i<last_location;i++)
	    {
	        node->dist[i]=dist[i];
	        node->cost[i]=cost[i];
	        node->time[i]=time[i];
	    }

	    node->left = NULL;
	    node->right = NULL;
	    node->height = 1; // new node is initially added at leaf

	    //printf("%d %s %s %d %d %d %d\n",node->location_no,node->name,node->category,node->rating,node->dist[0],node->cost[0],node->time[0]);
	    return(node);
    }

/*
Function Name: insert
Input Params:  Root of the tree, item data to be entered
Return Type:   Updated root of the tree
Description:   recursively inserts the new node into the tree
*/
ld* insert(ld* node, ld* data)
{
	// Perform the normal BST insertion
	if (node == NULL)
		return(data);

	if (data->location_no < node->location_no)
		node->left = insert(node->left, data);
	else if (data->location_no > node->location_no)
		node->right = insert(node->right, data);
	else //Location number needs to be unique
		return node;

	//Update height of this ancestor node
	node->height = 1 + max(height(node->left),
						height(node->right));

	//Get the balance factor of this ancestor node to check whether this node became unbalanced
	int balance = getBalance(node);

	// R Rotation
	if (balance > 1 && data->location_no < node->left->location_no)
		return rightRotate(node);

	// L Rotation
	if (balance < -1 && data->location_no > node->right->location_no)
		return leftRotate(node);

	// L R Rotation
	if (balance > 1 && data->location_no > node->left->location_no)
	{
		node->left = leftRotate(node->left);
		return rightRotate(node);
	}

	// R L Rotation
	if (balance < -1 && data->location_no < node->right->location_no)
	{
		node->right = rightRotate(node->right);
		return leftRotate(node);
	}

	return node;
}

/*
Function Name: InOrder
Input Params:  Root of the tree, the option to be searched in the tree
Return Type:   void
Description:   performs inorder traversal of the tree and prints the
               name and location number that match the category of choice
*/
void InOrder(ld *root,int a)
{
	char str[30];
	// assign a value for the string to compare
	if(a==1)
        strcpy(str,"shopping");
	else if(a==2)
        strcpy(str,"historical_monuments");
    else if(a==3)
        strcpy(str,"natural_masterpieces");
    else if(a==4)
        strcpy(str,"food");
    else if(a==5)
        strcpy(str,"man_made_wonders");
    else
    {
        printf("No such category exists!");
        exit(1);
    }
	if(root != NULL)
	{
		InOrder(root->left,a);
		if(strcmp(root->category,str)==0)
            printf("%d. %s\n", root->location_no,root->name);
		InOrder(root->right,a);
	}
}

/*
Function Name: load_from_files
Input Params:  NULL
Return Type:   void
Description:   opens the file and loads data from the file into
               respective structures to be inserted into the tree
               and create the matrices
*/
void load_from_files()
{
    initialize_matrices();
    FILE* li;
    li=fopen("location_data.txt","r");
    if(li == NULL)
    {
        printf("File open error\n");
        exit(FILE_OPEN_ERROR);
    }
    fscanf(li,"%d\t%d\n",&location_count,&last_location);
    int i,j;
    int no,rate;
    int dist[MAX_LOCATIONS];
    int cost[MAX_LOCATIONS];
    int time[MAX_LOCATIONS];
    char name[30];
    char category[30];
    // The size of the matrix will always increase and
    //never decrease as the location numbers and indexes will have
    //to be changed and traversing the locations will not be possible if they loose
    // their unique identity
    for(i=0;i<=last_location;i++)
    {
        fscanf(li,"%d\t%s\t%s\t%d\n",&no,name,category,&rate);
        for(j=0;j<=last_location;j++)
        {
            fscanf(li,"%d ",&dist[j]);
            distance_matrix[no][j]=dist[j];
            //no is used instead of i to traverse as if the node was previously
            // deleted all of its connected node will be made at a distance of infinity
            //this is achieved by initializing the matrices
            // so while loading the new nodes and the matrix
            //the location number is used to signify the distance from that particular location number
        }
        for(j=0;j<=last_location;j++)
        {
            fscanf(li,"%d ",&cost[j]);
            cost_matrix[no][j]=cost[j];
        }
        for(j=0;j<=last_location;j++)
        {
            fscanf(li,"%d ",&time[j]);
            time_matrix[no][j]=time[j];
        }

        ld* new=newNode(no,name,category,rate,dist,cost,time);
        root=insert(root,new);

    }
    fclose(li);
}

/*
Function Name: InOrderWrite
Input Params:  Root of the tree
Return Type:   void
Description:   Writes the data from each node back to the file using inorder traversal
*/
void InOrderWrite(ld* node)
{
    if(node!=NULL)
    {
        InOrderWrite(node->left);
        FILE* li;
        li=fopen("location_data.txt","a+");
        if(li == NULL)
        {
            printf("File open error\n");
            exit(FILE_OPEN_ERROR);
        }
        fprintf(li,"%d\t%s\t%s\t%d\n",node->location_no,node->name,node->category,node->rating);
        int j;
        for(j=0;j<=last_location;j++)
            {
                fprintf(li,"%d ",distance_matrix[node->location_no][j]);
            }
        fprintf(li,"\n");
        for(j=0;j<=last_location;j++)
            {
                fprintf(li,"%d ",cost_matrix[node->location_no][j]);
            }
        fprintf(li,"\n");
        for(j=0;j<=last_location;j++)
            {
                fprintf(li,"%d ",time_matrix[node->location_no][j]);
            }
        fprintf(li,"\n\n");
        fclose(li);
        InOrderWrite(node->right);
    }
}

/*
Function Name: write_in_file
Input Params:  nill
Return Type:   void
Description:   writes the data back into the file it read from
*/
void write_in_file()
{
     FILE* li;
    li=fopen("location_data.txt","w+");
    if(li == NULL)
    {
        printf("File open error\n");
        exit(FILE_OPEN_ERROR);
    }
    fprintf(li,"%d\t%d\n",location_count,last_location);
    fclose(li);
    InOrderWrite(root);
}

/*
Function Name: dfs
Input Params:  Root of the tree
Return Type:   void
Description:   performs depth-first search of the tree
*/
void dfs(ld *root)
{
	if(root != NULL)
    	{
        	printf("%d\t%s\n",root->location_no,root->name);
        	dfs(root->left);
        	dfs(root->right);
	    }

}

/*
Function Name: inorder_delete
Input Params:  Root of the tree, item data to be deleted
Return Type:   void
Description:   changes all the values of the distance,time and
               cost arrays to reflect the deletion of  particular node
*/
void inorder_delete(ld * root,int l_no_d)
{
    if(root !=NULL)
    {
        inorder_delete(root->left,l_no_d);
        root->dist[l_no_d]=MAX_DIST;
        distance_matrix[root->location_no][l_no_d]=MAX_DIST;
        root->time[l_no_d]=MAX_DIST;
        cost_matrix[root->location_no][l_no_d]=MAX_DIST;
        root->cost[l_no_d]=MAX_DIST;
        time_matrix[root->location_no][l_no_d]=MAX_DIST;
        inorder_delete(root->right,l_no_d);
    }
}

/*
Function Name: inorder_add
Input Params:  Root of the tree
Return Type:   void
Description:   adds the values associated with the new node to the
               existing nodes
*/
void inorder_add(ld * root)
{
    if(root!=NULL)
    {
        int distance,cost,time;
        inorder_add(root->left);
        printf("Enter the distance from location %d to %d",root->location_no,last_location);
        scanf("%d",&distance);
        printf("Enter the cost from location %d to %d",root->location_no,last_location);
        scanf("%d",&cost);
        printf("Enter the time from location %d to %d",root->location_no,last_location);
        scanf("%d",&time);
        root->dist[last_location]=distance;
        distance_matrix[root->location_no][last_location]=distance;
        root->cost[last_location]=cost;
        cost_matrix[root->location_no][last_location]=cost;
        root->time[last_location]=time;
        time_matrix[root->location_no][last_location]=time;
        inorder_add(root->right);

    }
}

/*
Function Name: delete_from_bst
Input Params:  Root of the tree, item data to be deleted
Return Type:   Updated root of the tree
Description:   Deletes the specified data and re-adjusts the
               tree structure according to bst tree constraints
*/

ld * delete_from_bst(ld * root, int data)
{
    ld * currnode, *parent, *successor, *p;

    // Check if the tree is empty
    if(root == NULL)
    {
        printf("Tree is empty\n");
        return root;
    }

    // Traverse and reach the appropriate part of the tree
    parent = NULL;
    currnode = root;
    while (currnode != NULL && data != currnode->location_no)
    {
        parent = currnode;
        if(data < currnode->location_no)
            currnode  = currnode->left;
        else
            currnode = currnode->right;
    }

    // If the data is not present in the tree
    if(currnode == NULL)  {
        printf("Location number not found\n");
        return root;
    }

    // Check and manipulate if either left subtree is absent,
    // or right subtree is absent
    // or both are present
    if(currnode->left == NULL)
        p = currnode->right;
    else if (currnode->right == NULL)
        p = currnode->left;
    else
    {
	 // Process of finding the inorder successor
        successor = currnode->right;
        while(successor->left != NULL)
            successor = successor->left;

        successor->left = currnode->left;
        p = currnode->right;
    }

    // The case of root deletion
    if (parent == NULL) {
        free(currnode);
        return p;
    }

    if(currnode == parent ->left)
        parent->left = p;
    else
        parent->right = p;

    free(currnode);
    return root;
}

/*
Function Name: print_matrix
Input Params:  NULL
Return Type:   void
Description:   print the elements of the matrix
*/
void print_matrix()
{
   int i,j;
   for(i=0;i<=last_location;i++)
    {
        for(j=0;j<=last_location;j++)
        {
            printf("%d ",distance_matrix[i][j]);
        }
        printf("\n");
    }
}

/*
Function Name: find_name
Input Params:  Root of the tree, location number
Return Type:   void
Description:   print the name associated with the location number
*/
void find_name(ld * root,int locationNumber)
{
    if(root != NULL)
    	{
        	find_name(root->left,locationNumber);
        	if(root->location_no==locationNumber)
            {
                printf("%s ",root->name);
            }
        	find_name(root->right,locationNumber);
	    }
}

/*
Function Name: Heapify
Input Params:  number of elements in the array
Return Type:   void
Description:   arranges the elements in the array to resemble a minimum-heap
*/
void Heapify(int n)
 {
    int i,k,heap,j;
    HP v;
    for(i=n/2;i>=1;i--)
    {
        k = i;
        v = Heap[i];
        heap = 0;

        while(!heap && (2*k <= n)) {
            j=2*k;
            if(j<n) {
                if(Heap[j+1].dist < Heap[j].dist) {
                    j=j+1;
                }
            }
            if(v.dist <= Heap[j].dist) {
                heap=1;
            }else {
                Heap[k]=Heap[j];
                k=j;
            }
        }
        Heap[k]=v;
    }
}

/*
Function Name: dijkstras_distance
Input Params:  source to apply dijkstras from
Return Type:   void
Description:   creates a distance and path array which represents
               the least distance from the source to a particular location
*/
void dijkstras_distance(int src)
{
    int i,x,j,mini;
    HP u;
    x=last_location+1;
    for(i=1;i<=last_location+1;i++)
    {
        Heap[i].index = i-1;
        Heap[i].dist = distance_matrix[src][i-1];
        Heap[i].path=src;
    }

        Heapify(last_location+1);

        for(i=1;i<=last_location+1;i++)
        {
            u = Heap[1];
            Heap[1] = Heap[x];
            x--;
            least_distance.PATH[u.index]=u.path;
            least_distance.DIST[u.index]=u.dist;

            for(j=1;j<=x;j++)
            {
                mini = min(Heap[j].dist,(u.dist+distance_matrix[u.index][Heap[j].index]));
                if(mini != Heap[j].dist)
                {
                    Heap[j].dist = mini;
                    Heap[j].path = u.index;
                }
            }
            Heapify(x);
        }
    if(least_distance.DIST[destination]>=MAX_DIST)
    {
        printf("no path found! (Disconnected node)");
        exit(FAILURE);
    }
    return;
}

/*
Function Name: dijkstras_cost
Input Params:  source to apply dijkstras from
Return Type:   void
Description:   creates a distance and path array which represents
               the least cost from the source to a particular location
*/
void dijkstras_cost(int src)
{
    int i,x,j,mini;
    HP u;
    x=last_location+1;
    for(i=1;i<=last_location+1;i++)
    {
        Heap[i].index = i-1;
        Heap[i].dist = cost_matrix[src][i-1];
        Heap[i].path=src;
    }

        Heapify(last_location+1);

        for(i=1;i<=last_location+1;i++)
        {
            u = Heap[1];
            Heap[1] = Heap[x];
            x--;
            least_cost.PATH[u.index]=u.path;
            least_cost.DIST[u.index]=u.dist;

            for(j=1;j<=x;j++)
            {
                mini = min(Heap[j].dist,(u.dist+cost_matrix[u.index][Heap[j].index]));
                if(mini != Heap[j].dist)
                {
                    Heap[j].dist = mini;
                    Heap[j].path = u.index;
                }
            }
            Heapify(x);
        }
    if(least_time.DIST[destination]>=MAX_DIST)
    {
        printf("no path found! (Disconnected node)");
        exit(FAILURE);
    }
    printf("\nDISCLAIMER:The costs only include the travel charges\n");
    return;
}

/*
Function Name: dijkstras_time
Input Params:  source to apply dijkstras from
Return Type:   void
Description:   creates a distance and path array which represents
               the least time from the source to a particular location
               and convert the time from minutes to hours
*/
void dijkstras_time(int src)
{
    int i,x,j,mini;
    HP u;
    x=last_location+1;
    for(i=1;i<=last_location+1;i++)
    {
        Heap[i].index = i-1;
        Heap[i].dist = time_matrix[src][i-1];
        Heap[i].path=src;
    }

        Heapify(last_location+1);

        for(i=1;i<=last_location+1;i++)
        {
            u = Heap[1];
            Heap[1] = Heap[x];
            x--;
            least_time.PATH[u.index]=u.path;
            least_time.DIST[u.index]=u.dist;

            for(j=1;j<=x;j++)
            {
                mini = min(Heap[j].dist,(u.dist+time_matrix[u.index][Heap[j].index]));
                if(mini != Heap[j].dist)
                {
                    Heap[j].dist = mini;
                    Heap[j].path = u.index;
                }
            }
            Heapify(x);
        }
    if(least_time.DIST[destination]>=MAX_DIST)
    {
        printf("no path found! (Disconnected node)");
        exit(FAILURE);
    }
    printf("\nThe least expected time is %d hours and %d minutes\n",(least_time.DIST[destination]/60),least_time.DIST[destination]%60);
    return;
}

/*
Function Name: reverse_array
Input Params:  the array to be reversed,start index,end index
Return Type:   void
Description:   reverses the array elements' order
*/
void reverse_array(int arr[],int start,int end)
{
    int temp;
    int length=end;
    while(start < end)
    {
        temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
    int z;
    for (z=0;z<length+1;z++)
    {
        printf("%d.",arr[z]);
        find_name(root,arr[z]);
        printf("->");
    }
    printf("end\n");
}

/*
Function Name: calculate_path
Input Params:  source number,destination number, the path-distance array result of dijkstras algorithm
               and the array to store the final shortest path order
Return Type:   void
Description:   calculates the shortest path to be followed for the least cost
               based on the result of the distance-path array formed by dijkstras algorithm
*/
void calculate_path(int src,int destination,PD shortest_path,int path[])
{
    route_len=0;
    int last=destination;
    while(last != src)
    {
        path[route_len] = last;
        last = shortest_path.PATH[last];
        route_len++;
    }

    path[route_len] = src;

    reverse_array(path,0,route_len);
}

/*
Function Name: compute
Input Params:  array with the values to calculate partial
               sum along with the number of elements in the array
Return Type:   void
Description:   creates the BIT[] array to calculate the
               partial sum of the elements in the given array
*/
void compute(int a[],int n)
{
    int i,x;
    for(i=1;i<=n;i++)
    {
        for(x=i; x <= n; x += x&-x)
          BIT[x] += a[i];
    }
}

/*
Function Name: create_fenwick
Input Params:  nill
Return Type:   void
Description:   create an array based on the cost-matrix
               and call the compute function to apply the fenwick logic
*/
void create_fenwick()
{
    int i;
    for (i=1;i<=route_len;i++)
    {
        fenwick_tree_array[i]=cost_matrix[cost_path[i-1]][cost_path[i]];
        printf("%d.",i);
        find_name(root,cost_path[i-1]);
        printf("\n");
    }
    compute(fenwick_tree_array,route_len+1);
}

/*
Function Name: sum
Input Params:  index upto which the sum needs to be claculated
Return Type:   integer sum
Description:   computes the sum upto the given index using the BIT array
               formed based on the fenwick logic
*/
int sum(int m)
{
     int x;
     int sum = 0;
     for(x=m; x > 0; x -= x&-x)
         sum += BIT[x];
     return sum;
}

/*
Function Name: query_fenwick
Input Params:  integer indices to calculate the partial sum between
Return Type:   void
Description:   based on the input integers calls the appropriate
               query function to find the partial sum
*/
void query_fenwick(int a,int b)
{
    if((a==-1)||(b==-1))
    {
        return;
    }
    else if(a>b)
    {
       printf("%d\n",sum(a-1)-sum(b-1));
       //We use -1 for the query as it signifies the cost between the locations and not the location numbers
       // so the array is shifted by 1 index
       return;
    }
    else if(b>a)
    {
        printf("%d\n",sum(b-1)-sum(a-1));
        return;
    }
    else
    {
        printf("0\n");
        return;
    }
}

/*
Function Name: updateRating
Input Params:  Root of the tree, new rating, location number to change the rating of
Return Type:   void
Description:   updates the rating of a particular location
               by inorder traversal of the tree
*/
void updateRating(ld * root,int newRating,int locationNumber)
{
    if(root != NULL)
    	{
        	updateRating(root->left,newRating,locationNumber);
        	if(root->location_no==locationNumber)
            {
                root->rating=(root->rating+newRating)/2;
            }
        	updateRating(root->right,newRating,locationNumber);
	    }
}

/*
Function Name: create_lookup_table
Input Params:  nill
Return Type:   void
Description:   creates a lookup table to store the value of
               maximum time of travelling between two adjacent nodes
               i.e. what is the max time between destinamtion
*/
void create_lookup_table()
{
    int i,j,k;
    int arr[MAX_LOCATIONS];
    for(i=0;i<route_len;i++)
    {
        arr[i]=time_matrix[time_path[i]][time_path[i+1]];
        printf("%d. ",i+1);
        find_name(root,time_path[i]);
        printf("\n");
    }
    for(i=0;i<route_len;i++)
    {
        for(j=i;j<route_len;j++)
        {
            int max=arr[i];
            for(k=i;k<=j;k++)
            {
                if(arr[k]>max)
                {
                    max=arr[k];
                }
            }
            lookup[i][j]=max;
        }
    }

}

/*
Function Name: create_adjacency_matrix
Input Params:  nill
Return Type:   void
Description:   creates a matrix where if the edge in present
                i.e. distance !=0 the the value is 1 else 0
*/
void create_adjacency_matrix()
{
    int i,j;
    for(i=0;i<=last_location;i++)
    {
        for(j=0;j<=last_location;j++)
        {
            if(distance_matrix[i][j]!=MAX_DIST)
            {
                adjacency_matrix[i][j]=1; // The matrix will automatically be initialized with 0 due to global declaration
            }
        }
    }
}

/*
Function Name: bfs(Breadth First Search)
Input Params:  source location and the number of vertices
Return Type:   void
Description:   performs bfs and creates an array of locations
               which are at a distance of less than 5 kms
*/
void bfs(int s,int v)
{
    int visited[MAX_LOCATIONS];
    int queue[MAX_LOCATIONS];//queue initialized
    int rear=0,front=0,u,i;
    for(i=0;i<v;i++)
    {
        visited[i]=0;
    }
    queue[rear]=s;
    visited[s]=1;

    while(front<=rear)
    {
        u=queue[front];
        close_by_locations[no_of_close_locations]=u;
        no_of_close_locations++;
        front++;
        for(i=0;i<v;i++)
        {
            if(adjacency_matrix[u][i]==1 && visited[i]==0 && distance_matrix[destination][i]<=5)
                //find the locations at a distance of less than 5 kms
            {
                visited[i]=1;
                rear++;
                queue[rear]=i;
            }
        }
    }
}

/*
Function Name: InOrderArrayCreation
Input Params:  Root of the tree, location number, index to store at in the created array
Return Type:   void
Description:   creates an array of structures which contains
               the rating, cost and time and location number
               of the closest locations
*/
void InOrderArrayCreation(ld* root,int l_no,int i)
{
    if(root!=NULL)
    {
        InOrderArrayCreation(root->left,l_no,i);
        if(root->location_no==l_no)
        {
            sortingArray[i].location_number=root->location_no;
            sortingArray[i].cost=cost_matrix[destination][l_no];
            sortingArray[i].time=time_matrix[destination][l_no];
            sortingArray[i].dist=distance_matrix[destination][l_no];
            sortingArray[i].rating=root->rating;
        }
        InOrderArrayCreation(root->right,l_no,i);
    }
}

/*
Function Name: sorting_array_creation
Input Params:  nill
Return Type:   void
Description:   aids in the array creation by calling
               the above recursive function with new arguments
*/
void sorting_array_creation()
{
    int i;
    for(i=0;i<no_of_close_locations;i++)
    {
        InOrderArrayCreation(root,close_by_locations[i],i);
    }
}

/*
Function Name: bubble_sort
Input Params:  sorting information array (SIA), number of elements in the array
Return Type:   void
Description:   sorts the array in ascending order
*/
void bubble_sort(SIA arr[],int n) //Sorting based on rating
{
    int i,j;
    SIA temp;
    for(i=0;i<n-1;i++)
    {
        for(j=0;j<n-i-1;j++)
        {
            if(arr[j+1].rating<arr[j].rating)
            {
                temp=arr[j];
                arr[j]=arr[j+1];
                arr[j+1]=temp;
            }
        }
    }
}

/*
Function Name: insertionSort
Input Params:  sorting information array (SIA), number of elements in the array
Return Type:   void
Description:   sorts the array in ascending order
*/
void insertionSort(SIA arr[],int n)
{
    int i,j;
    for(i=0;i<n;i++)
    {
        SIA v=arr[i];
        j=i-1;
        while(j>=0 && arr[j].dist>v.dist)
        {
            arr[j+1]=arr[j];
            j--;
        }
    arr[j+1]=v; //copy replacement method instead of swapping every time
    }
}

/*
Function Name: partition
Input Params:  sorting information array(SIA),lower index, upper index
Return Type:   integer
Description:   returns an integer to split the array
*/
int partition(SIA arr[], int low, int high)
{
    SIA pivot = arr[low];// Choosing the pivot as the first element
    SIA temp;
    int i = low;
    int j = high;

    while (i < j)
    {
        while (arr[j].time > pivot.time)
        {
            j--;
        }
        while (i < j && arr[i].time <= pivot.time)
        {
            i++;
        }
        if (i < j)
        {
            temp=arr[i];
            arr[i]=arr[j];
            arr[j]=temp;
        }
    }
    temp=arr[low]; // Placing the pivot in its correct position
    arr[low]=arr[j];
    arr[j]=temp;
    return j;
}

/*
Function Name: quickSort
Input Params:  sorting information array, lower index,upper index
Return Type:   void
Description:   sorts the array in ascending order
*/
void quickSort(SIA arr[], int low, int high) //sorting based on time
{
    if (low < high) {
        int pi = partition(arr, low, high);

        // Recursively sort elements before and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

/*
Function Name: merge
Input Params:  sorting information array(SIA),left index,middle index, right index
Return Type:   void
Description:   sorts the array by dividing it into 2 parts
*/
void merge(SIA arr[], int left, int mid, int right)
{
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temporary arrays
    SIA L[n1], R[n2];

    // Copy data to temp arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    // Merge the temp arrays back into arr[left..right]
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = left; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i].cost <= R[j].cost)
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if any
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if any
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

/*
Function Name: merge
Input Params:  sorting information array(SIA),left index, right index
Return Type:   void
Description:   sorts the array by dividing it into 2 parts
*/
void mergeSort(SIA arr[], int left, int right) //sorting based on cost
{
    if (left < right)
    {
        // Same as (left+right)/2, but avoids overflow for large left and right
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        // Merge the sorted halves
        merge(arr, left, mid, right);
    }
}

/*
Function Name: display_arrays
Input Params:  nill
Return Type:   void
Description:   provides a switch for the user to choose
               the parameter to sort array into
*/
void display_arrays()
{
    int stop_loop=0;
    int i;
    while(stop_loop==0)
    {
        printf("Explore locations based on 1.Ratings 2.Travelling time 3.Cost 4.Distance 5.Return :");
        scanf("%d",&choice3);
        switch(choice3)
        {
        case 1:
            {
                bubble_sort(sortingArray,no_of_close_locations);
                for(i=0;i<no_of_close_locations;i++)
                {
                    find_name(root,sortingArray[i].location_number);
                    printf("\t%d\n",sortingArray[i].rating);
                }
                break;
            }
        case 2:
            {
                quickSort(sortingArray,0,no_of_close_locations-1);
                for(i=0;i<no_of_close_locations;i++)
                {
                    find_name(root,sortingArray[i].location_number);
                    printf("\t%d\n",sortingArray[i].time);
                }
                break;
            }
        case 3:
            {
                mergeSort(sortingArray,0,no_of_close_locations-1);
                 for(i=0;i<no_of_close_locations;i++)
                {
                    find_name(root,sortingArray[i].location_number);
                    printf("\t%d\n",sortingArray[i].cost);
                }
                break;
            }
        case 4:
            {
                insertionSort(sortingArray,no_of_close_locations);
                for(i=0;i<no_of_close_locations;i++)
                {
                    find_name(root,sortingArray[i].location_number);
                    printf("\t%d\n",sortingArray[i].dist);
                }
                break;
            }
        case 5:
            {
                stop_loop=1;
                break;
            }
        }
    }
}

/*
Function Name: create_floyd_matrix
Input Params:  nill
Return Type:   void
Description:   create a matrix to apply floyd's algorithm based on the cost matrix
*/
void create_floyd_matrix()
{
    int i,j;
    for(i=0;i<no_of_close_locations;i++)
    {
        for(j=0;j<no_of_close_locations;j++)
        {
            floyd[i][j]=cost_matrix[close_by_locations[i]][close_by_locations[j]];
        }
    }
}

/*
Function Name: floyds_algorithm
Input Params:  nill
Return Type:   void
Description:   displays shortest cost from every node
               to every other node by dynamic programming
*/
void floyds_algorithm()
{
    int i,j,k;
    for(k=0;k<no_of_close_locations;k++)
    {
        for(i=0;i<no_of_close_locations;i++)
        {
            for(j=0;j<no_of_close_locations;j++)
            {
                floyd[i][j]=min(floyd[i][j],floyd[i][k]+floyd[k][j]);
            }
        }
    }
}

/*
Function Name: display_matrix
Input Params:  nill
Return Type:   void
Description:   display the results of floyd's algorithm along with the
               names that the index represent
*/
void display_matrix()
{
    int i,j;
    for(i=0;i<no_of_close_locations;i++)
    {
        for(j=0;j<no_of_close_locations;j++)
        {
            printf("%d\t",floyd[i][j]);
        }
        printf("\n");
    }
    printf("where the indexes are:\n");
    for(i=0;i<no_of_close_locations;i++)
    {
        printf("%d.",i);
        find_name(root,close_by_locations[i]);
        printf("   ");
    }
    printf("\n");
}

/*
Function Name: bfss(brute force string search)
Input Params:  substring and the text
Return Type:   index of the text where the pattern was found or -1 if not found
Description:   find the substring in the given text by comparing each character
*/
int bfss(char s[30],char t[30])
{
    int n=strlen(t);
    int m=strlen(s);
    int i,j;
    for(i=0;i<=n-m;i++)
    {
        j=0;
        while(j<m && s[j]==t[i+j])
        {
            j++;
        }
        if(j==m)
        {
            return i;
        }
    }
    return -1;
}

/*
Function Name: computeLPSArray
Input Params:  substring pointer, substring length,pointer to longest prefix-suffix
Return Type:   void
Description:   calculate the longest prefix suffix
*/
void computeLPSArray(char *pattern, int M, int *lps) {
    int len = 0;
    lps[0] = 0;

    int i = 1;
    while (i < M) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

/*
Function Name: KMPSearch
Input Params:  substring and text
Return Type:   integer index
Description:   search the substring in the given text and return the index where
               it was found else return -1
*/
int KMPSearch(char *pattern, char *text) {
    int M = strlen(pattern);
    int N = strlen(text);

    int *lps = (int *)malloc(sizeof(int) * M);
    if (lps == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }

    computeLPSArray(pattern, M, lps);

    int i = 0;  // index for text[]
    int j = 0;  // index for pattern[]

    while (i < N)
    {
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }

        if (j == M)
        {
            free(lps);
            return i - j;
            // Pattern found at index i - j
        } else if (i < N && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    free(lps);
    // Pattern not found
    return -1;
}

/*
Function Name: InOrderSearch
Input Params:  root of the tree, substring to be searched
Return Type:   void
Description:   performs inorder traversal of the
               tree to find the matching substring with the help of bfss
*/
void InOrderSearch(ld *root, char string[])
{
    if(root!=NULL)
    {
        InOrderSearch(root->left,string);
        if(KMPSearch(string,root->name)!=-1)
        {
            printf("%d.%s\t%d\n",root->location_no,root->name,root->rating);
        }
        InOrderSearch(root->right,string);
    }
}

/*
Function Name: updateName
Input Params:  root of the tree,new name,location number
Return Type:   void
Description:   performs inorder traversal of the tree and changes
               the name of the location if the location number is matched
*/
void updateName(ld *root, char string[], int l_no)
{
    if(root!=NULL)
    {
        updateName(root->left,string,l_no);
        if(root->location_no==l_no)
        {
           strcpy(root->name,string);
        }
        updateName(root->right,string,l_no);
    }
}

/*
Function Name: updateDistance
Input Params:  root of the tree,distance from,distance to,new distance
Return Type:   void
Description:   performs inorder traversal of the tree and changes
               the distance of the location if the location number is matched
*/
void updateDistance(ld *root,int from,int to,int value)
{
    if(root!=NULL)
    {
        updateDistance(root->left,from,to,value);
        if(root->location_no==from)
        {
            root->dist[to]=value;
        }
        updateDistance(root->right,from,to,value);
    }
}

/*
Function Name: updateCost
Input Params:  root of the tree,cost from,cost to,new cost
Return Type:   void
Description:   performs inorder traversal of the tree and changes
               the cost of the location if the location number is matched
*/
void updateCost(ld *root,int from,int to,int value)
{
    if(root!=NULL)
    {
        updateCost(root->left,from,to,value);
        if(root->location_no==from)
        {
            root->cost[to]=value;
        }
        updateCost(root->right,from,to,value);
    }
}

/*
Function Name: updateTime
Input Params:  root of the tree,time from,cost to,new time
Return Type:   void
Description:   performs inorder traversal of the tree and changes
               the time of the location if the location number is matched
*/
void updateTime(ld *root,int from,int to,int value)
{
    if(root!=NULL)
    {
        updateTime(root->left,from,to,value);
        if(root->location_no==from)
        {
            root->time[to]=value;
        }
        updateTime(root->right,from,to,value);
    }
}

/*
Function Name: update_switch
Input Params:  location number to be updated
Return Type:   void
Description:   give the user choice to choose what they want to update the the location information
*/
void update_switch(int update_no)
{
    int stop_loop=0;
    while(stop_loop==0)
    {
        printf("Update\n1.Name\n2.Distance\n3.Cost\n4.Time\n5.Return\n");
        scanf("%d",&choice4);
        switch(choice4)
        {
        case 1:
            {
                char name[30];
                printf("Enter new name: ");
                scanf("%s",name);
                updateName(root,name,update_no);
                break;
            }
        case 2:
            {
                int to_from,new_distance,location;
                printf("Update distance 'to' the location or 'from' the location(0/1)? ");
                scanf("%d",&to_from);
                printf("Other location number: ");
                scanf("%d",&location);
                printf("Enter the new distance: ");
                scanf("%d",&new_distance);
                if(to_from==0)
                {
                    distance_matrix[update_no][location]=new_distance;
                    updateDistance(root,update_no,location,new_distance);
                }
                else if(to_from==1)
                {
                     distance_matrix[location][update_no]=new_distance;
                     updateDistance(root,location,update_no,new_distance);
                }
                else
                {
                    printf("Invalid\n");
                }
                break;
            }
        case 3:
            {
                int to_from,new_cost,location;
                printf("Update cost 'to' the location or 'from' the location(0/1)? ");
                scanf("%d",&to_from);
                printf("Other location number: ");
                scanf("%d",&location);
                printf("Enter the new cost: ");
                scanf("%d",&new_cost);
                // if to_from=1, the the distance provide is to the other location from the current one
                if(to_from==0)
                {
                    cost_matrix[update_no][location]=new_cost;
                    updateCost(root,update_no,location,new_cost);
                }
                else if(to_from==1)
                {
                     cost_matrix[location][update_no]=new_cost;
                     updateDistance(root,location,update_no,new_cost);
                }
                else
                {
                    printf("Invalid\n");
                }
                break;
            }
        case 4:
            {
                int to_from,new_time,location;
                printf("Update time 'to' the location or 'from' the location(0/1)? ");
                scanf("%d",&to_from);
                printf("Other location number: ");
                scanf("%d",&location);
                printf("Enter the new time: ");
                scanf("%d",&new_time);
                if(to_from==0)
                {
                    time_matrix[update_no][location]=new_time;
                    updateTime(root,update_no,location,new_time);
                }
                else if(to_from==1)
                {
                     time_matrix[location][update_no]=new_time;
                     updateDistance(root,location,update_no,new_time);
                }
                else
                {
                    printf("Invalid\n");
                }
                break;
            }
        case 5:
            {
                stop_loop=1;
                break;
            }
        }
    }

}

/*
Function Name: options2
Input Params:  nill
Return Type:   void
Description:   gives the user a second set of options to choose from
*/
void options2()
{
    int continueIteration=1;
    create_adjacency_matrix();
    bfs(destination,last_location+1);
    sorting_array_creation();
    while(continueIteration!=0)
    {
        printf("\nChoose an option:-\n 1.View intermediate costs\n 2.View maximum intermidiate travelling time\n 3.Rate the destination\n 4.view close-by locations\n 5.View lowest cost matrix\n 6.back :\n ");
        scanf("%d",&choice2);
        switch(choice2)
        {
        case 1:
            {
                create_fenwick();
                int a,b;
                while(!((a==-1)||(b==-1)))
                {
                    printf("Enter the intermediate locations to display the cost (-1 to return): ");
                    scanf("%d",&a);
                    if(a==-1)
                    {
                        break;
                    }
                    scanf("%d",&b);
                    if(b==-1)
                    {
                        break;
                    }
                    query_fenwick(a,b);
                }
                break;
            }
        case 2:
            {
                create_lookup_table();
                int x,y;
                printf("Enter the path indices to compare:");
                scanf("%d %d",&x,&y);
                printf("Maximum travelling time between intermediate locations =%d\n",lookup[x-1][y-2]);
                //-2 because the indices are starting from 0 and not 1 and also the distance is between the location numbers so the array shifts by 1
                break;
            }
        case 3:
            {
                int newRating;
                printf("Enter the rating: ");
                scanf("%d",&newRating);
                updateRating(root,newRating,destination);
                break;
            }
        case 4:
            {
                printf("\n");
                display_arrays();
                break;
            }
        case 5:
            {
                create_floyd_matrix();
                floyds_algorithm();
                display_matrix();
                break;
            }
        case 6:
            {
                continueIteration=0;
                break;
            }
        }
    }

}
