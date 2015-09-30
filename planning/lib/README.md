 A* with stricted visited list
------------------

#### Data structure:
 ```java
 q                          priority queue
 max_q_size                 unsigned int
 expansions                 hash table
 final_path                 string
 run_time                   long long
 is_found                   bool
 ```
 
#### Pseudocode:
```ruby
q.push start
while q is not empty and q.top != goal
  curr = q.pop
  if curr has not been visited
    expansions.insert curr.state
    foreach neighbour of curr
      if neighbour has not been visited
        if q does not contain neighbour
          q.push neighbour
        else
          update q with neighbour
if q.empty
  final_path = ""
else
  final_path = q.top.path
```



 Lifelong Planning A*
------------------
```java
matrix                     2D dynamic array
start, goal                tuple
hfunc                      function object
q                          priority queue
max_q_size                 unsigned int
expansions                 hash table
path                       string
run_time                   long long
```

#### Pseudocode:
----------------------
```ruby
initialize()
q.reset();
at(start).r = 0;
q.push(start);
```
