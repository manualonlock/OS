# include <stdio.h>
# include <stdbool.h>

typedef struct node_start{
    /*
     * metadata located at the start of the node
     */
    struct node_start *prev;
    struct node_start *next;
    bool is_free;
    unsigned long size;
} node_start;


typedef struct node_end{
    /*
     * metadata located at the end of the node
     */
    bool is_free;
    unsigned long size;
} node_end;


const size_t NODE_START_SIZE = sizeof (node_start);
const size_t NODE_END_SIZE = sizeof (node_end);
const size_t NODE_METADATA_SIZE = NODE_START_SIZE + NODE_END_SIZE;

const short LEFT_SIDE = 1;
const short RIGHT_SIDE = 2;

/*
 * Node pointer shift
 */
#define NODE_ADDRESS_SHIFT(node , p)  &((void *) node)[p]

/*
 * getting the numeric address value from a pointer
 */
#define ADDRESS_TO_NUMBER(address)  (unsigned long) address

/*
 * the address of the metadata located at the end of the node
 */
#define NODE_END_METADATA_ADDRESS(node) NODE_ADDRESS_SHIFT(node, NODE_START_SIZE + node->size)


/*
 * Start of memory that node allocates
 */
#define NODE_MEMORY_START(node) NODE_ADDRESS_SHIFT(node, NODE_START_SIZE)

/*
 * Getting node entire size
 */
#define NODE_SIZE(node) node->size + NODE_METADATA_SIZE

/*
 * Address of the end of node
 */
#define NODE_END_ADDRESS(node) NODE_ADDRESS_SHIFT(node, NODE_SIZE(node))


node_start *head = NULL;


void update_node_occupation(node_start *node, bool is_free)
{
    node->is_free = is_free;

    node_end *end_of_node = NODE_END_METADATA_ADDRESS(node);
    end_of_node->is_free = is_free;
    end_of_node->size = node->size;
}

void setup_heap(void *buf, size_t size)
{
    /*
     * setup_head
     * @buf: start of heap buffer
     * @size: size of buffer
     */
    head = buf;
    head->size = size - NODE_METADATA_SIZE;
    update_node_occupation(head, true);
}


void *create_node(node_start *node, size_t size)
{
    size_t size_to_cut = size + NODE_METADATA_SIZE;
    node_start *new_node = NODE_END_ADDRESS(node) - size_to_cut;

    node->size -= size_to_cut;
    update_node_occupation(node, true);

    node_start *previous_next = node->next;

    node->next = new_node;
    new_node->next = previous_next;
    new_node->size = size;
    update_node_occupation(new_node, false);
    new_node->prev = node;

    if (previous_next)
    {
        previous_next->prev = new_node;
    }
    return NODE_MEMORY_START(new_node);
}


void *_malloc(size_t size)
{
    /*
     * Custom memory allocator
     */
    node_start *current_node = head;
    size_t size_needed = size + NODE_METADATA_SIZE;

    while (current_node)
    {
        if (current_node->size == size && current_node->is_free)
        {
            update_node_occupation(current_node, false);
            return NODE_MEMORY_START(current_node);
        }
        if (current_node->size < size_needed || current_node->is_free == false)
        {
            current_node = current_node->next;
            continue;
        }
        return create_node(current_node, size);
    }
    return NULL;
}


void MergeNode(int side, node_start *node) {
    if (!node->is_free) return;

    if (side == LEFT_SIDE) {

        node->size += node->next->size + NODE_METADATA_SIZE;
        node->next = node->next->next;
        if (node->next)
        {
            node->next->prev = node;
        }
        if (node->prev) {
            return MergeNode(LEFT_SIDE, node->prev);
        }
    }
    if (side == RIGHT_SIDE) {
        node->prev->size += node->size + NODE_METADATA_SIZE;
        node->prev->next = node->next;

        if (node->next) {
            return MergeNode(RIGHT_SIDE, node->next);
        }
    }

    }


void _free(void *p)
{
    /*
     * Custom memory free function
     */
    node_start *root_node = (void *) (ADDRESS_TO_NUMBER(p) - NODE_START_SIZE);
    update_node_occupation(root_node, true);
    if (root_node->prev) MergeNode(LEFT_SIDE, root_node->prev);
    if (root_node->next) MergeNode(RIGHT_SIDE, root_node->next);
}
