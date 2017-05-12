# concurrent_queue
A simple wapper for deque that allows for concurrent push and pop from queue, without busy waiting.

This has a modification to a classic queue where an additional function close()
is present that can signal and end of the queue for try_pop.

This is useful in a situation of producer and consumer for the producer
to signal the consumer the end of data and close gracefully.
