#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>
# include "systemfunctions.c"
# include "settings.h"

#define MAX_LOCATIONS 30
#define MAX_DIST 9999

int main()
{
    welcome();
    load_from_files();
    while(1)
    {
    printf("\n\nChoose an option:\n1.Plan a trip\n2.Add a destination\n3.Delete a destination\n4.Search Location\n5.Update Location\n6.Exit\n");
    scanf("%d",&choice);
    switch(choice)
    {
    case 1:
        {
            printf("Which station is closest to you (0-6)? ");
            scanf("%d",& src);
            //based on the activity number a string will be created and compared with every node
            printf("What is your preferred activity?\n 1.Shopping\n 2.Historical Monuments\n 3.Natural masterpieces\n 4.Food\n 5.Man-made wonders\n");
            scanf("%d",&activity);
            InOrder(root,activity);
            printf("Choose your destination: ");
            scanf("%d",&destination);
            dijkstras_distance(src);
            printf("Recommended route for shortest travel distance: ");
            calculate_path(src,destination,least_distance,distance_path);
            dijkstras_cost(src);
            printf("Recommended route for shortest travel cost: ");
            calculate_path(src,destination,least_cost,cost_path);
            dijkstras_time(src);
            printf("Recommended route for shortest travel time: ");
            calculate_path(src,destination,least_time,time_path);
            options2();
            break;
        }
    case 2:
        {
            int rate;
            int dist[MAX_LOCATIONS];
            int cost[MAX_LOCATIONS];
            int time[MAX_LOCATIONS];
            char name[30];
            char category[30];
            printf("Enter the name:");
            scanf("%s",name);
            printf("Enter the category:");
            scanf("%s",category);
            printf("Enter the average rating:");
            scanf("%d",&rate);
            int p;
            location_count++;
            printf("Enter the distance,time and cost to travel to the previous locations from new location:\n");
            for(p=0;p<=last_location;p++)
            {
                scanf("%d %d %d",&dist[p],&cost[p],&time[p]);
                distance_matrix[last_location+1][p]=dist[p];
                cost_matrix[last_location+1][p]=cost[p];
                time_matrix[last_location+1][p]=time[p];

            }
            //create a new node based on the given information
            ld* new=newNode(++last_location,name,category,rate,dist,cost,time);
            root=insert(root,new);
            inorder_add(root);
            write_in_file();
            load_from_files();
            writeLogAdd();
            break;
        }
    case 3:
        {
            int deletion_number;
            printf("Enter the location number to be deleted:");
            scanf("%d",&deletion_number);
            //dfs(root);
            int i;
            for(i=0;i<=last_location;i++)
            {
                distance_matrix[deletion_number][i]=MAX_DIST;
                cost_matrix[deletion_number][i]=MAX_DIST;
                time_matrix[deletion_number][i]=MAX_DIST;
            }
            inorder_delete(root,deletion_number);
            delete_from_bst(root,deletion_number);
            location_count--;
            //Deletion causes the tree to change the balance factor thus
            //recreate the tree after making the appropriate changes in the file
            write_in_file();
            load_from_files();
            writeLogDelete();
            break;
        }
    case 4:
        {
            //Add BFSS for string search like the recommendation engine and display along with ratings
            char search_string[30];
            printf("Enter the name of the location: ");
            scanf("%s",search_string);
            InOrderSearch(root,search_string);
            break;
        }
    case 5:
        {
            int update_no;
            printf("Enter the location number to be updated: ");
            scanf("%d",&update_no);
            update_switch(update_no);
            break;

        }
    case 6:
        {
            write_in_file();
            printf("Thank you for using our services!");
            exit(SUCCESS);
        }
    }
    }
}
