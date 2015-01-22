//structure for storing sid lists
extern struct sid_list;	
//pointer to the head of the current list
extern struct sid_list * c_list_head;
//adding elements to the sid list
extern void add_sid_to_list(struct sid_list * input);
//removing elements from sid list
extern void remove_sid_from_list(struct sid_list * input);
//free memory occupied from used nodes in the sid list
extern void free_sid_list_memory(struct sid_list * input);
//create a node for sid list using  string input
extern struct sid_list * create_sid_list_node(char * input);
	
