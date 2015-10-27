# Path-Planning
This repo provides implementation for three Path-Planning Algorithms:
 * A* with stricted visited list
 * Lifelong Planning A*
 * D* Lite final version

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

#### Data structure:
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
```ruby
initialize
  q.reset
  at(start).r = 0
  q.push start

update_vertex(s)
  if s.cell != start
    minimum = huge
    foreach neighbour of s.cell
      minimum = min(minimum, (at(neighbour).g + cost()))
    s.r = minimum
  q.remove s.cell
  if s.g != s.r 
    q.push s.cell
  
update_neighbours_of(cell)
  foreach neighbour of cell
    if !at(neighbour).bad
      update_vertex(at(neighbour));
      
compute_shortest_path
  while (!q.empty  and key(at(q.top)) < key(at(goal)) or at(goal).r != at(goal).g
    c = q.pop
    if at(c).g > at(c).r
      at(c).g = at(c).r
    else
      at(c).g = huge 
      update_vertex at c
    update_neighbours_of c
    max_q_size = max(max_q_size, q.size())
    expansions.insert c
  path = build_path
  
plan
  initialize
  compute_shortest_path
  
replan(cells_to_toggle)
  foreach cell of cells_to_toggle
    at(cell).bad = !at(cell).bad
    if !at(cell).bad
      update_vertex at cell
    else
      at(cell).g = at(cell).r = huge
      update_neighbours_of cell
  compute_shortest_path
```

 D* Lite final version
------------------
#### Data structure:
```java
matrix                     2D dynamic array
start, goal                tuple
hfunc                      function object
km                         int
q                          priority queue
old_keys                   hash table
max_q_size                 unsigned int
expansions                 hash table
path                       string
run_time                   long long
```
#### Pseudocode:
```ruby
initialize
  q.reset
  km = at(goal).r = 0
  q.push goal
  old_keys.insert key(goal, { at(goal), km } )
  
update_vertex(s)
  if s.cell != goal
    minimum = huge
    foreach neighbour of s.cell
      minimum = min(minimum, (at(neighbour).g + cost()))
      s.r = minimum
  q.remove s.cell
  if s.g != s.)
    q.push s.cell 
    old_keys.update_with tuple({ s.cell, Key{ s, km })
    
update_neighbours_of(cell)
  foreach neighbour of cell
    if !at(neighbour).bad
      update_vertex(at(neighbour))
      
compute_shortest_path
  while !q.empty() && ((key(at(q.top)) < key(at(start)) || at(start).r != at(start).g))
    c = q.pop
    if old_keys.at(c) < key(at(c), km)
      q.push c
      old_keys.update_with tuple( c, Key{ at(c), km })
    else if at(c).g > at(c).r
      at(c).g = at(c).r
      update_neighbours_of c
    else
      at(c).g = huge
      update_vertex at c
      update_neighbours_of c
    max_q_size = max(max_q_size, q.size)
    expansions.insert c
    
initial_plan
  initialize
  compute_shortest_path
  
plan(changes, move_to, use_path)
  initial_plan
  last = start
  curr = start
  i = 0
  while curr != goal
    curr = min(neighbours of curr)
    move_to curr
    if i != changes.length
      km += hfunc(this_loop.last, this_loop.curr)
      last = curr
      foreach cell of changes[i]
        at(cell).bad = !at(cell).bad
        if !at(cell).bad
          update_vertex at cell
        else
          at(cell).g = at(cell).r = huge
        update_neighbours_of cell
      ++i
      compute_shortest_path
    use_path build_path(this_loop.curr, goal)
```
