#ifndef SYSTEMFUNCTIONS_H_INCLUDED
#define SYSTEMFUNCTIONS_H_INCLUDED

void welcome();
void initialize_matrices();
int height(ld *N);
int max(int a, int b);
int min(int a, int b);
ld* rightRotate(ld *y);
ld *leftRotate(ld *x);
int getBalance(ld *N);
ld* newNode(int no,char name[30],char category[30],int rate,int dist[MAX_LOCATIONS],int cost[MAX_LOCATIONS],int time[MAX_LOCATIONS]);
ld* insert(ld* node, ld* data);
void InOrder(ld *root,int a);
void load_from_files();
void InOrderWrite(ld* node);
void write_in_file();
void inorder(ld *troot);
void inorder_delete(ld * root,int l_no_d);
void inorder_add(ld * root);
ld * delete_from_bst(ld * root, int data);
void print_matrix();
void find_name(ld * root,int locationNumber);
void Heapify(int n);
void dijkstras_distance(int src);
void dijkstras_cost(int src);
void dijkstras_time(int src);
void reverse_array(int arr[],int start,int end);
void calculate_path(int src,int destination,PD shortest_path,int path[]);
void compute(int a[],int n);
void create_fenwick();
int sum(int m);
void query_fenwick(int a,int b);
void create_lookup_table();
void updateRating(ld * root,int newRating,int locationNumber);
void create_adjacency_matrix();
void bfs(int s,int v);
void InOrderArrayCreation(ld* root,int l_no,int i);
void sorting_array_creation();
void bubble_sort(SIA arr[],int n);
void insertionSort(SIA arr[],int n);
int partition(SIA arr[], int low, int high);
void quickSort(SIA arr[], int low, int high);
void merge(SIA arr[], int left, int mid, int right);
void mergeSort(SIA arr[], int left, int right);
void display_arrays();
void create_floyd_matrix();
void floyds_algorithm();
void display_matrix();
int bfss(char s[30],char t[30]);
void InOrderSearch(ld *root, char string[]);
void updateName(ld *root, char string[], int l_no);
void updateDistance(ld *root,int from,int to,int value);
void updateCost(ld *root,int from,int to,int value);
void updateTime(ld *root,int from,int to,int value);
void update_switch(int update_no);
void options2();
void writeLogAdd();
void writeLogDelete();




#endif // SYSTEMFUNCTIONS_H_INCLUDED
