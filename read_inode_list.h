/*struct sid_obj{
  unsigned long inode_number;
  
  int n_sids;
  struct sid_obj * next; 
  char ** sids;
};
*/
//extern struct sid_obj ** sid_hash_k;
//extern struct sid_obj * get_node_k(unsigned long inode_number);
//extern void add_node_k(struct sid_obj * ptr);
extern void add_sid_with_inode_k(unsigned long inode_number , char * sid);